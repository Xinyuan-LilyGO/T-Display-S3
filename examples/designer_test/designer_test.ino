// designer_test.ino
// Phase 3 - LittleFS + WiFi upload server + JPEG decode + button image browser.

#include "Arduino.h"
#include <FS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TJpg_Decoder.h>
#include "TFT_eSPI.h"
#include <OneButton.h>
#include "pin_config.h"
#include "config.h"
#include "index_html.h"

/* ST7789V init sequence (same as tft example for the new LCD module) */
#define LCD_MODULE_CMD_1

TFT_eSPI tft = TFT_eSPI();

#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif

// -- helpers ------------------------------------------

void initTFT()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    tft.begin();

#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < (lcd_st7789v[i].len & 0x7f); j++) {
            tft.writedata(lcd_st7789v[i].data[j]);
        }
        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
#endif

    tft.setRotation(3);   // landscape - 320 wide x 170 tall on hardware
    tft.setSwapBytes(true);

    pinMode(PIN_LCD_BL, OUTPUT);
    digitalWrite(PIN_LCD_BL, HIGH);
}

void showStatusLine(const char *msg, uint16_t fg = TFT_WHITE, uint16_t bg = TFT_BLACK)
{
    tft.setTextColor(fg, bg);
    tft.setTextSize(1);
    tft.setCursor(4, 4);
    tft.print(msg);
}

bool mountLittleFS()
{
    // Explicitly name the partition (matches CSV label "littlefs");
    // basePath "/littlefs", maxOpenFiles 10, partitionLabel "littlefs"
    if (!LittleFS.begin(true, "/littlefs", 10, "littlefs")) {
        Serial.println("[LittleFS] begin() failed - check partition table was flashed");
        // Try once more without a label (subtype-based search)
        if (!LittleFS.begin(true)) {
            Serial.println("[LittleFS] Retry also failed");
            return false;
        }
        Serial.println("[LittleFS] Mounted via subtype search (no label)");
    } else {
        Serial.println("[LittleFS] Mounted via partition label");
    }

    Serial.printf("[LittleFS] Total: %u KB  Used: %u KB\n",
                  LittleFS.totalBytes() / 1024,
                  LittleFS.usedBytes()  / 1024);

    // Ensure /images directory exists
    if (!LittleFS.exists(IMAGES_DIR)) {
        LittleFS.mkdir(IMAGES_DIR);
        Serial.println("[LittleFS] Created " IMAGES_DIR);
    }
    return true;
}

// -- Arduino entry points ------------------------------------------

WebServer server(80);
bool apMode = false;

// -- Image browser state ------------------------------------------

#define MAX_IMAGES 64
static char imageList[MAX_IMAGES][64];
static int  imageCount = 0;
static int  imageIndex = 0;
static bool imageListDirty = true;   // set true after upload/delete

OneButton btn1(PIN_BUTTON_1, true);  // prev image  (active-low)
OneButton btn2(PIN_BUTTON_2, true);  // next image  (active-low)

// -- TJpg_Decoder pixel output callback ------------------------------------------
// Called by the decoder for each block of decoded pixels; writes straight to TFT.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    if (y >= tft.height()) return false;
    tft.pushImage(x, y, w, h, bitmap);
    return true;
}

// -- Image list ------------------------------------------

void loadImageList()
{
    imageCount = 0;
    File dir = LittleFS.open(IMAGES_DIR);
    if (!dir || !dir.isDirectory()) return;

    File f = dir.openNextFile();
    while (f && imageCount < MAX_IMAGES) {
        if (!f.isDirectory()) {
            String n = String(f.name());
            String lower = n; lower.toLowerCase();
            if (lower.endsWith(".jpg") || lower.endsWith(".jpeg") || lower.endsWith(".bmp")) {
                n.toCharArray(imageList[imageCount], sizeof(imageList[0]));
                imageCount++;
            }
        }
        f = dir.openNextFile();
    }
    // Sort alphabetically (simple insertion sort, small list)
    for (int i = 1; i < imageCount; i++) {
        char tmp[64];
        strncpy(tmp, imageList[i], sizeof(tmp));
        int j = i - 1;
        while (j >= 0 && strcmp(imageList[j], tmp) > 0) {
            strncpy(imageList[j + 1], imageList[j], sizeof(imageList[0]));
            j--;
        }
        strncpy(imageList[j + 1], tmp, sizeof(imageList[0]));
    }
    imageListDirty = false;
    Serial.printf("[images] Found %d image(s)\n", imageCount);
}

// -- JPEG / BMP display ------------------------------------------

static void showBmp(const String &path);  // forward declaration

void showCurrentImage()
{
    if (imageCount == 0) {
        tft.setRotation(3);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(1);
        tft.setCursor(4, 80);
        tft.print("No images - upload via WiFi");
        return;
    }

    String path = String(IMAGES_DIR) + "/" + imageList[imageIndex];
    Serial.printf("[images] Showing %s\n", path.c_str());

    String lower = path; lower.toLowerCase();
    if (lower.endsWith(".jpg") || lower.endsWith(".jpeg")) {
        uint16_t iw = 0, ih = 0;
        TJpgDec.getFsJpgSize(&iw, &ih, path.c_str(), LittleFS);
        Serial.printf("[images] JPEG size %ux%u\n", iw, ih);

        // Auto-rotate display to match image orientation
        if (ih > iw) {
            tft.setRotation(0);   // portrait: 170 wide x 320 tall
        } else {
            tft.setRotation(3);   // landscape: 320 wide x 170 tall
        }
        tft.fillScreen(TFT_BLACK);
        TJpgDec.setJpgScale(1);
        TJpgDec.drawFsJpg(0, 0, path.c_str(), LittleFS);
    } else if (lower.endsWith(".bmp")) {
        showBmp(path);
    }
    // No filename overlay — filenames are visible in the browser UI.
}

// -- BMP display ------------------------------------------
// Supports 24-bit and 32-bit uncompressed BI_RGB BMPs from LittleFS.
// Draws 1:1 from (0,0); image clips at display bounds if larger than screen.
static void showBmp(const String &path)
{
    File f = LittleFS.open(path, "r");
    if (!f) { Serial.println("[BMP] open failed"); return; }

    // File header (14 B) + BITMAPINFOHEADER (40 B) in one read
    uint8_t hdr[54];
    if (f.read(hdr, 54) != 54 || hdr[0] != 'B' || hdr[1] != 'M') {
        Serial.println("[BMP] not a valid BMP");
        f.close(); return;
    }

    // Little-endian helpers
    auto r32 = [&](int o) -> uint32_t {
        return (uint32_t)hdr[o] | ((uint32_t)hdr[o+1]<<8) |
               ((uint32_t)hdr[o+2]<<16) | ((uint32_t)hdr[o+3]<<24);
    };
    auto r16 = [&](int o) -> uint16_t {
        return (uint16_t)hdr[o] | ((uint16_t)hdr[o+1]<<8);
    };

    uint32_t dataOffset = r32(10);
    int32_t  bmpW     = (int32_t)r32(18);
    int32_t  bmpH     = (int32_t)r32(22);   // negative = top-down
    uint16_t bpp      = r16(28);
    uint32_t compress = r32(30);

    Serial.printf("[BMP] %dx%d  bpp=%u  compress=%u  offset=%u\n",
                  bmpW, abs(bmpH), bpp, compress, dataOffset);

    if (bmpW <= 0 || (bpp != 24 && bpp != 32) || compress != 0) {
        Serial.println("[BMP] unsupported format (need 24/32 bpp BI_RGB)");
        tft.setRotation(3); tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.setCursor(4, 80); tft.print("BMP: unsupported format");
        f.close(); return;
    }

    bool     topDown   = (bmpH < 0);
    uint32_t iw        = (uint32_t)bmpW;
    uint32_t ih        = topDown ? (uint32_t)(-bmpH) : (uint32_t)bmpH;
    uint8_t  Bpp       = bpp / 8;                      // 3 or 4
    uint32_t rowStride = ((iw * Bpp) + 3) & ~3u;       // padded to 4 bytes

    // Rotation: portrait vs landscape
    if (ih > iw) tft.setRotation(0); else tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    uint32_t dw = (uint32_t)tft.width();
    uint32_t dh = (uint32_t)tft.height();

    // Draw rows that fit on screen; clip at display bounds
    uint32_t drawH = (ih < dh) ? ih : dh;
    uint32_t drawW = (iw < dw) ? iw : dw;

    uint8_t  *rowBuf = (uint8_t  *)malloc(rowStride);
    uint16_t *pixBuf = (uint16_t *)malloc(drawW * sizeof(uint16_t));
    if (!rowBuf || !pixBuf) {
        Serial.println("[BMP] malloc failed");
        free(rowBuf); free(pixBuf); f.close(); return;
    }

    for (uint32_t dy = 0; dy < drawH; dy++) {
        // BMP rows are bottom-up by default; top-down when height is negative
        uint32_t fileRow = topDown ? dy : (ih - 1 - dy);
        f.seek(dataOffset + fileRow * rowStride);
        if ((uint32_t)f.read(rowBuf, rowStride) != rowStride) break;

        for (uint32_t dx = 0; dx < drawW; dx++) {
            const uint8_t *p = rowBuf + dx * Bpp;  // stored as BGR[A]
            uint8_t b = p[0], g = p[1], r = p[2];
            // Standard RGB565 — tft.setSwapBytes(true) handles byte order to display
            pixBuf[dx] = ((uint16_t)(r & 0xF8) << 8) |
                         ((uint16_t)(g & 0xFC) << 3) |
                         (b >> 3);
        }
        tft.pushImage(0, (int32_t)dy, drawW, 1, pixBuf);
    }

    free(rowBuf);
    free(pixBuf);
    f.close();
    Serial.println("[BMP] done");
}

void nextImage()
{
    if (imageCount == 0) return;
    imageIndex = (imageIndex + 1) % imageCount;
    showCurrentImage();
}

void prevImage()
{
    if (imageCount == 0) return;
    imageIndex = (imageIndex - 1 + imageCount) % imageCount;
    showCurrentImage();
}

void handleRoot()
{
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleList()
{
    String json = "[";
    File dir = LittleFS.open(IMAGES_DIR);
    bool first = true;
    if (dir && dir.isDirectory()) {
        File f = dir.openNextFile();
        while (f) {
            if (!f.isDirectory()) {
                if (!first) json += ",";
                json += "\"" + String(f.name()) + "\"";
                first = false;
            }
            f = dir.openNextFile();
        }
    }
    json += "]";
    server.send(200, "application/json", json);
}

void handleDelete()
{
    if (!server.hasArg("f")) { server.send(400, "text/plain", "Missing filename"); return; }
    String name = server.arg("f");
    // Sanitize: reject any path traversal
    if (name.indexOf("..") >= 0 || name.indexOf("/") >= 0) {
        server.send(400, "text/plain", "Invalid filename"); return;
    }
    String path = String(IMAGES_DIR) + "/" + name;
    if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        server.send(200, "text/plain", "Deleted");
        Serial.printf("[server] Deleted %s\n", path.c_str());
        // Keep index in bounds then redisplay
        loadImageList();
        if (imageIndex >= imageCount) imageIndex = max(0, imageCount - 1);
        showCurrentImage();
    } else {
        server.send(404, "text/plain", "Not found");
    }
}

// Multipart upload handler - streams directly to LittleFS
File uploadFile;

void handleUpload()
{
    HTTPUpload &upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        // Sanitize filename: strip directory components
        String origName = upload.filename;
        int lastSlash = origName.lastIndexOf('/');
        if (lastSlash >= 0) origName = origName.substring(lastSlash + 1);
        // Only allow .jpg / .jpeg / .bmp
        String lower = origName;
        lower.toLowerCase();
        if (!lower.endsWith(".jpg") && !lower.endsWith(".jpeg") && !lower.endsWith(".bmp")) {
            Serial.printf("[server] Rejected non-image upload: %s\n", origName.c_str());
            return;
        }
        String path = String(IMAGES_DIR) + "/" + origName;
        Serial.printf("[server] Upload start: %s\n", path.c_str());
        tft.fillScreen(TFT_BLACK);
        showStatusLine(("Uploading: " + origName).c_str());
        uploadFile = LittleFS.open(path, FILE_WRITE);

    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadFile) uploadFile.write(upload.buf, upload.currentSize);

    } else if (upload.status == UPLOAD_FILE_END) {
        if (uploadFile) {
            uploadFile.close();
            Serial.printf("[server] Upload done: %u bytes\n", upload.totalSize);
        }
    }
}

void handleUploadFinish()
{
    if (server.upload().status == UPLOAD_FILE_ABORTED) {
        server.send(500, "text/plain", "Upload aborted");
        return;
    }
    char buf[80];
    snprintf(buf, sizeof(buf), "Upload OK  Free: %u KB",
             (LittleFS.totalBytes() - LittleFS.usedBytes()) / 1024);
    server.send(200, "text/plain", buf);
    Serial.println(buf);
    // Refresh list and jump to the newly uploaded file
    String uploadedName = server.upload().filename;
    int lastSlash = uploadedName.lastIndexOf('/');
    if (lastSlash >= 0) uploadedName = uploadedName.substring(lastSlash + 1);
    loadImageList();
    // Find the uploaded file in the sorted list
    for (int i = 0; i < imageCount; i++) {
        if (uploadedName.equals(imageList[i])) { imageIndex = i; break; }
    }
    showCurrentImage();
}

// -- WiFi ------------------------------------------

void startWiFi()
{
    tft.fillScreen(TFT_BLACK);
    showStatusLine("Connecting WiFi...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint8_t tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 20) {
        delay(500);
        tries++;
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        apMode = false;
        char buf[64];
        snprintf(buf, sizeof(buf), "IP: %s", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] Connected - %s\n", buf);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setTextSize(1);
        tft.setCursor(4, 4);
        tft.print("WiFi OK  ");
        tft.print(WiFi.localIP().toString().c_str());
        tft.setCursor(4, 20);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.print("Upload: http://");
        tft.print(WiFi.localIP().toString().c_str());
    } else {
        // Fall back to AP mode
        apMode = true;
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, AP_PASSWORD[0] ? AP_PASSWORD : nullptr);
        IPAddress apIP = WiFi.softAPIP();
        char buf[64];
        snprintf(buf, sizeof(buf), "AP: %s", AP_SSID);
        Serial.printf("[WiFi] AP mode - %s  IP: %s\n", AP_SSID, apIP.toString().c_str());
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setTextSize(1);
        tft.setCursor(4, 4);
        tft.print("AP: ");
        tft.print(AP_SSID);
        tft.setCursor(4, 20);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.print("Upload: http://");
        tft.print(apIP.toString().c_str());
    }
}

void startServer()
{
    server.on("/",       HTTP_GET,  handleRoot);
    server.on("/list",   HTTP_GET,  handleList);
    server.on("/delete", HTTP_GET,  handleDelete);
    server.on("/upload", HTTP_POST, handleUploadFinish, handleUpload);
    server.begin();
    Serial.println("[server] Started on port 80");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n[designer_test] booting...");

    initTFT();
    tft.fillScreen(TFT_BLACK);
    showStatusLine("Mounting LittleFS...");

    if (!mountLittleFS()) {
        tft.fillScreen(TFT_BLACK);
        showStatusLine("LittleFS FAILED", TFT_RED);
        return;
    }

    // -- TJpg_Decoder setup --
    // tft.setSwapBytes(true) is already called in initTFT(); TJpgDec must NOT
    // also swap, otherwise bytes are swapped twice and colors are wrong.
    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(false);
    TJpgDec.setCallback(tft_output);

    // -- Buttons --
    btn1.attachClick(prevImage);
    btn2.attachClick(nextImage);

    startWiFi();
    delay(1500);
    startServer();

    // Load stored images and show first one (or empty-state message)
    loadImageList();
    showCurrentImage();
}

void loop()
{
    server.handleClient();
    btn1.tick();
    btn2.tick();
}
