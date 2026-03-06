# T-Display-S3 Explorer + Audio Synthesis

## English

### Overview

This example combines the Gingoduino musical interface (pure music theory) with **real-time audio synthesis** via I2S.

Music is generated mathematically (sine wave synthesis) and sent to the PCM5102 DAC, which converts it to analog audio on the 3.5mm output.

### Required Hardware

- **LilyGo T-Display-S3** (ESP32-S3 + ST7789 display)
- **T-Display-S3 MIDI Shield v1.1** with PCM5102 DAC module
- Headphones or speakers (3.5mm input)

### Required Libraries

| Library | Source |
|---------|--------|
| **TFT_eSPI** (by Bodmer, v2.5.43+) | Arduino Library Manager |
| **Gingoduino** | Arduino Library Manager or symlink |
| `driver/i2s.h` | Included with ESP32 Arduino core (no install needed) |

#### TFT_eSPI configuration (mandatory)

After installing TFT_eSPI, you must configure it for the T-Display-S3.
Locate the library folder and edit `User_Setup_Select.h`:

- **Windows:** `C:\Users\<YourName>\Documents\Arduino\libraries\TFT_eSPI\User_Setup_Select.h`
- **macOS/Linux:** `~/Documents/Arduino/libraries/TFT_eSPI/User_Setup_Select.h`

```cpp
// Comment out the default setup:
//#include <User_Setup.h>

// Uncomment the T-Display-S3 setup:
#include <User_Setups/Setup206_LilyGo_T_Display_S3.h>
```

> **Note:** This change must be repeated every time the TFT_eSPI library is updated.
> Without this step the display will remain black.

#### I2S Pinout (confirmed on V1.1 hardware)
```
T-Display-S3         R (470Ohm)   PCM5102
GPIO_11 (BCK)  ---   R15   --->  pin 13 (BCK)
GPIO_12 (DOUT) ---   R14   --->  pin 14 (DIN)
GPIO_13 (LRCK) ---   R13   --->  pin 15 (LRCK)
                              pin 12 (SCK) -> GND (3-wire mode, no MCLK)
GND            ---            GND
```

### Controls

| Button | Action |
|--------|--------|
| **BOOT (LEFT)** | Switch page, stop audio |
| **KEY (RIGHT)** | Cycle items + play audio<br/>On Sequence page: PLAY/STOP |

### Page by Page

#### 1. Note Explorer (C C# D D# E F F# G G# A A# B)
- **On cycle**: Plays the current note (500ms)
- **Result**: Sine wave at note frequency (octave 4)

#### 2. Interval Explorer
- **On cycle**: Plays interval as 2 simultaneous notes
- **Result**: C4 + interval, e.g.: C4 + E4 (M3)

#### 3. Chord Explorer (CM, Cm, C7, Dm7, etc)
- **On cycle**: Plays the full chord
- **Result**: Polyphony (up to 4 voices)

#### 4. Scale Explorer
- **On cycle**: Plays the scale as arpeggio (200ms per note)
- **Result**: Ascending sequence: C D E F G A B C

#### 5. Harmonic Field
- **On cycle**: Plays all 7 field chords as a progression
- **Result**: I-ii-iii-IV-V-vi-vii, e.g. in C Major: C-Dm-Em-F-G-Am-Bdim

#### 6. Fretboard
- No audio (focus on chord diagram)

#### 7. Sequence
- **Predefined sequences**: I-IV-V-I, ii-V-I Jazz, Simple Melody, Rests & Notes, Bossa
- **PLAY button**: Press RIGHT to execute the sequence
- **Result**: Plays each event with its duration

### Audio Architecture

#### Synthesis
```
FreeRTOS audioTask (core 1)
    |
Event scheduler (non-blocking arpeggios/progressions)
    |
4-voice polyphonic engine (per-voice ADSR)
    |
Sine wave oscillator (phase accumulation)
    |
Soft-clip mixer (tanh limiter)
    |
16-bit PCM samples (44.1kHz)
    |
I2S driver -> PCM5102 DAC
    |
3.5mm output (stereo)
```

#### Characteristics
- **Sample rate**: 44.1kHz (CD standard)
- **Bit depth**: 16-bit
- **Polyphony**: up to 4 voices with individual ADSR envelopes
- **Envelope**: Per-voice ADSR (Attack/Decay/Sustain/Release)
- **Strum**: Configurable delay between chord voices (simulates plucking)
- **Mixing**: Soft-clip via tanh() to prevent harsh distortion on chords
- **Scheduler**: Up to 32 scheduled events for non-blocking arpeggios/progressions

#### Non-blocking
- Synthesis runs on **core 1** (dedicated, priority 2)
- Display runs on **core 0** (unaffected)
- All playback functions return immediately (arpeggios, scales, progressions, sequences)
- Display always updates BEFORE audio triggers

### Code Example

```cpp
// Play a note
GingoNote note("C");
playNote(note, octave=4, durationMs=500);

// Play a chord (polyphony)
GingoChord chord("Cm7");
playChord(chord, octave=4, durationMs=500);

// Play scale as arpeggio
GingoScale scale("C", SCALE_MAJOR);
playScaleArpeggio(scale, octave=4);
```

### Limitations & Notes

#### Blocking during playback
- `playScaleArpeggio()` has internal delays that **block loop()** for ~1.4s
- `playSequence()` blocks for the entire sequence duration
- **Future solution**: async playback task (v0.3+)

#### Audio quality
- Simple synthesis (pure sine wave) — no harmonics
- No filters, reverb, or effects
- Click/pop when changing frequencies rapidly (no glide)
- **Future solution**: wavetable oscillator (v0.3+)

#### Volume
- May sound low depending on headphones/speakers
- Adjust in `audioTask()`: change `0.7f` in `sample * 0.7f` to values between 0.5 and 1.0
- **Future solution**: volume control (v0.3+)

#### Latency
- Latency: ~50-100ms (DMA I2S buffer)
- Acceptable for educational exploration
- **Not recommended** for professional real-time synthesis applications

### Troubleshooting

#### No sound
1. Check I2S connections (GPIO 11/12/10)
2. Verify headphones/speakers are connected
3. Serial monitor: check for message "Gingoduino T-Display-S3 Explorer + Audio Synthesis"
4. Try increasing volume in `audioTask()` (0.7f -> 0.9f)

#### Very low sound
- Increase gain in `audioTask()`: `sample * 0.7f` -> `sample * 0.9f`
- Use external amplifier if available

#### Stutter/freeze when playing scale
- Normal! `playScaleArpeggio()` blocks for a few seconds
- Solution: implement async task (roadmap v0.3)

#### Noise/clicks
- I2S buffer too small? Increase `I2S_BUFFER_SIZE` from 512 to 1024
- DMA not configured? Check `i2s_config_t` in code

### Roadmap (v0.3+)

- [ ] Async playback (non-blocking UI)
- [ ] Multiple oscillator types (sawtooth, square, triangle)
- [ ] Low-pass filter (ADSR)
- [ ] Glide/portamento between notes
- [ ] Volume control (potentiometer or slider)
- [ ] Custom sequence recording
- [ ] MIDI input (receive via USB/Serial)

### References

- ESP32 I2S: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2s.html
- PCM5102 Datasheet: https://www.ti.com/lit/ds/symlink/pcm5102a.pdf
- T-Display-S3 Schematic: provided by LilyGo
- Gingoduino: https://github.com/sauloverissimo/gingoduino

---

## Portugues

### Visao Geral

Este exemplo combina a interface musical do Gingoduino (teoria musical pura) com **sintese de audio em tempo real** via I2S.

A musica e gerada matematicamente (sine wave synthesis) e enviada ao DAC PCM5102, que converte para audio analogico na saida 3.5mm.

### Hardware Necessario

- **LilyGo T-Display-S3** (ESP32-S3 + tela ST7789)
- **T-Display-S3 MIDI Shield v1.1** com modulo DAC PCM5102
- Fone de ouvido ou caixa de som (entrada 3.5mm)

### Bibliotecas Necessarias

| Biblioteca | Fonte |
|------------|-------|
| **TFT_eSPI** (by Bodmer, v2.5.43+) | Arduino Library Manager |
| **Gingoduino** | Arduino Library Manager ou symlink |
| `driver/i2s.h` | Incluida no core ESP32 Arduino (sem instalacao) |

#### Configuracao do TFT_eSPI (obrigatoria)

Apos instalar o TFT_eSPI, e necessario configura-lo para o T-Display-S3.
Localize a pasta da biblioteca e edite o `User_Setup_Select.h`:

- **Windows:** `C:\Users\<SeuNome>\Documents\Arduino\libraries\TFT_eSPI\User_Setup_Select.h`
- **macOS/Linux:** `~/Documents/Arduino/libraries/TFT_eSPI/User_Setup_Select.h`

```cpp
// Comente a linha do setup padrao:
//#include <User_Setup.h>

// Descomente a linha do T-Display-S3:
#include <User_Setups/Setup206_LilyGo_T_Display_S3.h>
```

> **Atencao:** Esta alteracao precisa ser refeita toda vez que o TFT_eSPI for atualizado.
> Sem este passo, a tela permanecera preta.

#### Pinagem I2S (conforme schema V1.1)
```
T-Display-S3         R (470Ohm)   PCM5102
GPIO_11 (DOUT) ---   R14   --->  pino 14 (DIN)
GPIO_12 (BCK)  ---   R15   --->  pino 13 (BCK)
GPIO_10 (LRCK) ---   R13   --->  pino 15 (LRCK)
                              pino 12 (SCK) -> GND (3-wire mode, sem MCLK)
GND            ---            GND
```

### Controles

| Botao | Acao |
|-------|------|
| **BOOT (LEFT)** | Muda pagina, para audio |
| **KEY (RIGHT)** | Cicla itens + toca audio<br/>Na pagina Sequence: PLAY/STOP |

### Pagina por Pagina

#### 1. Note Explorer (C C# D D# E F F# G G# A A# B)
- **Ao ciclar**: Toca a nota atual (500ms)
- **Resultado**: Sine wave na frequencia da nota (oitava 4)

#### 2. Interval Explorer
- **Ao ciclar**: Toca intervalo como 2 notas simultaneas
- **Resultado**: C4 + intervalo, exemplo: C4 + E4 (M3)

#### 3. Chord Explorer (CM, Cm, C7, Dm7, etc)
- **Ao ciclar**: Toca o acorde completo
- **Resultado**: Polifonia (ate 4 vozes)

#### 4. Scale Explorer
- **Ao ciclar**: Toca a escala como arpeggio (200ms por nota)
- **Resultado**: Sequencia ascendente: C D E F G A B C

#### 5. Harmonic Field
- **Ao ciclar**: Toca os 7 acordes do campo como progressao
- **Resultado**: I-ii-iii-IV-V-vi-vii, exemplo em C Major: C-Dm-Em-F-G-Am-Bdim

#### 6. Fretboard
- Sem audio (foco no diagrama de acordes)

#### 7. Sequence
- **Sequencias predefinidas**: I-IV-V-I, ii-V-I Jazz, Simple Melody, Rests & Notes, Bossa
- **Botao PLAY**: Pressione RIGHT para executar a sequencia
- **Resultado**: Executa cada evento da sequencia com sua duracao

### Arquitetura de Audio

#### Sintese
```
FreeRTOS audioTask (core 1)
    |
Sine wave oscillator (phase accumulation)
    |
ADSR envelope (attack/release)
    |
16-bit PCM samples (44.1kHz)
    |
I2S driver -> PCM5102 DAC
    |
Saida 3.5mm (estereo)
```

#### Caracteristicas
- **Frequencia**: 44.1kHz (padrao CD)
- **Bit depth**: 16-bit
- **Polifonia**: ate 4 vozes (soma de sines)
- **Envelope**: Attack rapido (instantaneo), Release 200ms

#### Nao-bloqueante
- Sintese roda em **core 1** (dedicado)
- Display roda em **core 0** (nao afetado)
- Audio tem prioridade baixa para nao congelar UI

### Exemplo de Codigo

```cpp
// Tocar uma nota
GingoNote note("C");
playNote(note, octave=4, durationMs=500);

// Tocar um acorde (polifonia)
GingoChord chord("Cm7");
playChord(chord, octave=4, durationMs=500);

// Tocar escala como arpeggio
GingoScale scale("C", SCALE_MAJOR);
playScaleArpeggio(scale, octave=4);
```

### Limitacoes & Notas

#### Bloqueio durante playback
- `playScaleArpeggio()` tem delays internos que **bloqueiam o loop()** por ~1.4s
- `playSequence()` bloqueia durante toda a duracao da sequencia
- **Solucao futura**: task assincrona para playback (v0.3+)

#### Qualidade de audio
- Sintese simples (sine wave pura) — sem harmonics
- Sem filtros, reverb, ou efeitos
- Click/pop ao mudar frequencias rapidamente (sem glide)
- **Solucao futura**: oscilador com wavetable (v0.3+)

#### Volume
- Pode soar baixo dependendo do fone/caixa
- Ajustar em `audioTask()`: mude `0.7f` em `sample * 0.7f` para valores entre 0.5 e 1.0
- **Solucao futura**: botao de volume (v0.3+)

#### Latencia
- Latencia: ~50-100ms (DMA I2S buffer)
- Aceitavel para exploracao educacional
- **Nao recomendado** para aplicacoes profissionais de sintese em tempo real

### Troubleshooting

#### Sem som
1. Conferir conexoes I2S (GPIO 11/12/10)
2. Verificar se fone/caixa esta conectado
3. Serial monitor: verificar mensagem "Gingoduino T-Display-S3 Explorer + Audio Synthesis"
4. Tentar aumentar volume em `audioTask()` (0.7f -> 0.9f)

#### Som muito baixo
- Aumentar ganho em `audioTask()`: `sample * 0.7f` -> `sample * 0.9f`
- Usar amplificador externo se disponivel

#### Engasgo/freeze ao tocar escala
- Normal! `playScaleArpeggio()` bloqueia por alguns segundos
- Solucao: implementar task assincrona (roadmap v0.3)

#### Ruido/clicks
- I2S buffer pequeno? Aumentar `I2S_BUFFER_SIZE` de 512 para 1024
- DMA nao configurado? Verificar `i2s_config_t` no codigo

### Roadmap (v0.3+)

- [ ] Playback assincrono (nao bloqueia UI)
- [ ] Multiplos tipos de oscilador (sawtooth, square, triangle)
- [ ] Filtro low-pass (ADSR)
- [ ] Glide/portamento entre notas
- [ ] Controle de volume (potenciometro ou slider)
- [ ] Gravacao de sequencias custom
- [ ] MIDI input (receber via USB/Serial)

### Referencias

- ESP32 I2S: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2s.html
- PCM5102 Datasheet: https://www.ti.com/lit/ds/symlink/pcm5102a.pdf
- T-Display-S3 Schematic: schema fornecido pelo LilyGo
- Gingoduino: https://github.com/sauloverissimo/gingoduino

---

**Version / Versao**: 0.1.0
**Date / Data**: 2026-02-18
**Author / Autor**: Saulo Verissimo
**License / Licenca**: MIT
