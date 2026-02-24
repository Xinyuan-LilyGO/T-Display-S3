#ifndef SYNTH_ENGINE_H
#define SYNTH_ENGINE_H

// ── Simple polyphonic I2S synthesizer for ESP32-S3 ────────────────────────────
// Hardware: PCM5102A DAC connected via I2S (I2S_BCK_PIN, I2S_WS_PIN, I2S_DATA_OUT_PIN)
// Waveform: sine wave via lookup table (fast, no floating-point sin per sample)
// Polyphony: up to 8 voices
// Release:   ~150 ms fade-out on NoteOff (prevents clicks)
// Thread safety: FreeRTOS queue — noteOn/noteOff are safe from any task

#include <Arduino.h>
#if ESP_ARDUINO_VERSION_MAJOR >= 3
#include <driver/i2s_std.h>
#else
#include <driver/i2s.h>
#endif
#include <cmath>
#include <cstring>
#include "mapping.h"

#if ESP_ARDUINO_VERSION_MAJOR < 3
static const i2s_port_t SYNTH_PORT     = I2S_NUM_0;
#endif
static const int        SYNTH_SR       = 44100;
static const int        SYNTH_BUF      = 256;    // samples per DMA buffer
static const int        SYNTH_VOICES   = 8;
static const int        SYNTH_SIN_LEN  = 512;

class SynthEngine {
public:
    void begin() {
        _buildSinTable();

#if ESP_ARDUINO_VERSION_MAJOR >= 3
        // New I2S driver (ESP-IDF 5.x)
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        chan_cfg.dma_desc_num  = 8;
        chan_cfg.dma_frame_num = SYNTH_BUF;
        i2s_new_channel(&chan_cfg, &_tx_handle, nullptr);

        i2s_std_config_t std_cfg = {};
        std_cfg.clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SYNTH_SR);
        std_cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        std_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
        std_cfg.gpio_cfg.bclk = (gpio_num_t)I2S_BCK_PIN;
        std_cfg.gpio_cfg.ws   = (gpio_num_t)I2S_WS_PIN;
        std_cfg.gpio_cfg.dout = (gpio_num_t)I2S_DATA_OUT_PIN;
        std_cfg.gpio_cfg.din  = I2S_GPIO_UNUSED;
        i2s_channel_init_std_mode(_tx_handle, &std_cfg);
        i2s_channel_enable(_tx_handle);
#else
        // Legacy I2S driver (ESP-IDF 4.x)
        i2s_config_t cfg = {};
        cfg.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        cfg.sample_rate          = SYNTH_SR;
        cfg.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
        cfg.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
        cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        cfg.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;
        cfg.dma_buf_count        = 8;
        cfg.dma_buf_len          = SYNTH_BUF;
        cfg.use_apll             = false;
        cfg.tx_desc_auto_clear   = true;
        i2s_driver_install(SYNTH_PORT, &cfg, 0, nullptr);

        i2s_pin_config_t pins = {};
        pins.bck_io_num   = I2S_BCK_PIN;
        pins.ws_io_num    = I2S_WS_PIN;
        pins.data_out_num = I2S_DATA_OUT_PIN;
        pins.data_in_num  = I2S_PIN_NO_CHANGE;
        i2s_set_pin(SYNTH_PORT, &pins);
        i2s_zero_dma_buffer(SYNTH_PORT);
#endif

        memset(_voices, 0, sizeof(_voices));
        _queue = xQueueCreate(64, sizeof(NoteMsg));

        // Pin audio task to core 1 (same as loop, OK — FreeRTOS time-slices)
        xTaskCreatePinnedToCore(_task, "synth", 4096, this, 10, nullptr, 1);
    }

    // Safe to call from loop() or any task
    void noteOn(uint8_t note, uint8_t vel) {
        if (vel == 0) { noteOff(note); return; }
        NoteMsg m{note, vel};
        xQueueSend(_queue, &m, 0);
    }
    void noteOff(uint8_t note) {
        NoteMsg m{note, 0};
        xQueueSend(_queue, &m, 0);
    }

private:
    struct Voice {
        float   phase;
        float   inc;
        float   amp;
        float   release;   // per-sample decrement (0 = sustain)
        uint8_t note;
        bool    active;
    };
    struct NoteMsg { uint8_t note; uint8_t vel; };

    Voice            _voices[SYNTH_VOICES];
    float            _sinTable[SYNTH_SIN_LEN];
    QueueHandle_t    _queue;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    i2s_chan_handle_t _tx_handle = nullptr;
#endif

    void _buildSinTable() {
        for (int i = 0; i < SYNTH_SIN_LEN; i++)
            _sinTable[i] = sinf(2.0f * M_PI * i / SYNTH_SIN_LEN);
    }

    static float _freq(uint8_t note) {
        return 440.0f * powf(2.0f, (note - 69) / 12.0f);
    }

    void _activateVoice(uint8_t note, uint8_t vel) {
        // Retrigger same note if already active
        for (int i = 0; i < SYNTH_VOICES; i++) {
            if (_voices[i].active && _voices[i].note == note) {
                _voices[i].amp     = (vel / 127.0f) * 0.7f;
                _voices[i].release = 0.0f;
                return;
            }
        }
        // Find free voice, or steal the softest
        int vi = -1;
        float minAmp = 999.0f;
        for (int i = 0; i < SYNTH_VOICES; i++) {
            if (!_voices[i].active) { vi = i; break; }
            if (_voices[i].amp < minAmp) { minAmp = _voices[i].amp; vi = i; }
        }
        if (vi < 0) return;
        _voices[vi].note    = note;
        _voices[vi].phase   = 0.0f;
        _voices[vi].inc     = _freq(note) / SYNTH_SR;
        _voices[vi].amp     = (vel / 127.0f) * 0.7f;
        _voices[vi].release = 0.0f;
        _voices[vi].active  = true;
    }

    void _fadeVoice(uint8_t note) {
        for (int i = 0; i < SYNTH_VOICES; i++) {
            if (_voices[i].active && _voices[i].note == note && _voices[i].release == 0.0f)
                _voices[i].release = _voices[i].amp / (SYNTH_SR * 0.15f);  // 150 ms
        }
    }

    static void _task(void* arg) {
        SynthEngine* s = (SynthEngine*)arg;
        static int16_t buf[SYNTH_BUF * 2];
        NoteMsg msg;

        while (true) {
            // Process pending note events
            while (xQueueReceive(s->_queue, &msg, 0) == pdTRUE) {
                if (msg.vel) s->_activateVoice(msg.note, msg.vel);
                else         s->_fadeVoice(msg.note);
            }

            // Generate audio buffer
            for (int i = 0; i < SYNTH_BUF; i++) {
                float sample = 0.0f;
                for (int v = 0; v < SYNTH_VOICES; v++) {
                    if (!s->_voices[v].active) continue;
                    int ti = (int)(s->_voices[v].phase * SYNTH_SIN_LEN) & (SYNTH_SIN_LEN - 1);
                    sample += s->_voices[v].amp * s->_sinTable[ti];
                    s->_voices[v].phase += s->_voices[v].inc;
                    if (s->_voices[v].phase >= 1.0f) s->_voices[v].phase -= 1.0f;
                    if (s->_voices[v].release > 0.0f) {
                        s->_voices[v].amp -= s->_voices[v].release;
                        if (s->_voices[v].amp <= 0.0f) {
                            s->_voices[v].amp    = 0.0f;
                            s->_voices[v].active = false;
                        }
                    }
                }
                if (sample >  0.95f) sample =  0.95f;
                if (sample < -0.95f) sample = -0.95f;
                int16_t s16 = (int16_t)(sample * 28000.0f);
                buf[i*2]   = s16;
                buf[i*2+1] = s16;
            }

            size_t bw;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
            i2s_channel_write(s->_tx_handle, buf, sizeof(buf), &bw, portMAX_DELAY);
#else
            i2s_write(SYNTH_PORT, buf, sizeof(buf), &bw, portMAX_DELAY);
#endif
        }
    }
};

extern SynthEngine synth;

#endif // SYNTH_ENGINE_H
