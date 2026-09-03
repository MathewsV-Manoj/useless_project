/* =====================================================================
   HVA CAMERA ANNEX  —  ESP32-S3 + OV3660
   Camera, LED ring and buzzer on ONE board, at ONE address.

   CAMERA ANNEX + BUZZER
   ---------------------
   The board serves the camera and sounds a buzzer. No LEDs. The website
   is a separate file you serve yourself.

       board   http://192.168.4.1        (join wifi HVA-ANNEX / verify00)
       site    http://localhost:8000     (python -m http.server 8000)

   The buzzer motif is stepped from loop(); no request handler ever
   sleeps, because a handler that sleeps stalls the web server and a
   stalled server is a failed capture.

   ---------------------------------------------------------------------
   NOTHING IN THIS FILE NEEDS EDITING. Flash it as-is.
   ---------------------------------------------------------------------

   TOOLS SETTINGS (all four matter)
     Board             ESP32S3 Dev Module
     USB CDC On Boot   Enabled
     Flash Size        16MB (128Mb)
     PSRAM             OPI PSRAM          <- camera fails without this
     Partition Scheme  16M Flash (3MB APP/9.9MB FATFS)
   ===================================================================== */

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// ============ ONE ADDRESS, FOREVER ===================================
// The board does not join a network. It IS the network. That means the
// address is the same on every network, in every room, on every day —
// there is no DHCP lease to change, no router to be absent, no captive
// portal, and nothing to configure before a demo.
//
//        http://192.168.4.1
//
// Nothing below needs editing. There are no credentials to fill in.
const char* AP_SSID = "HVA-ANNEX";
const char* AP_PASS = "verify00";        // must be 8+ chars
IPAddress   AP_IP (192,168,4,1);
// =====================================================================

// ============ BUZZER ONLY ============================================
// No LEDs. A single buzzer sounds when the verdict lands.
//   BUZZ_PIN 14 is clear of every camera line below.
//   Set to -1 if you have no buzzer on the board.
#define BUZZ_PIN     14
#define PASSIVE_BUZZ 1     // 0 if a steady tone plays on digitalWrite HIGH
// =====================================================================

// ESP32-S3 camera pin map (ESP32-S3-EYE compatible carriers)
#define PWDN_GPIO_NUM   -1
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM   15
#define SIOD_GPIO_NUM    4
#define SIOC_GPIO_NUM    5
#define Y9_GPIO_NUM     16
#define Y8_GPIO_NUM     17
#define Y7_GPIO_NUM     18
#define Y6_GPIO_NUM     12
#define Y5_GPIO_NUM     10
#define Y4_GPIO_NUM      8
#define Y3_GPIO_NUM      9
#define Y2_GPIO_NUM     11
#define VSYNC_GPIO_NUM   6
#define HREF_GPIO_NUM    7
#define PCLK_GPIO_NUM   13

WebServer server(80);
bool cameraOK = false;



// ---------------------------------------------------------------------
bool initCamera() {
  camera_config_t c;
  c.ledc_channel = LEDC_CHANNEL_0;
  c.ledc_timer   = LEDC_TIMER_0;
  c.pin_d0 = Y2_GPIO_NUM;   c.pin_d1 = Y3_GPIO_NUM;
  c.pin_d2 = Y4_GPIO_NUM;   c.pin_d3 = Y5_GPIO_NUM;
  c.pin_d4 = Y6_GPIO_NUM;   c.pin_d5 = Y7_GPIO_NUM;
  c.pin_d6 = Y8_GPIO_NUM;   c.pin_d7 = Y9_GPIO_NUM;
  c.pin_xclk  = XCLK_GPIO_NUM;   c.pin_pclk  = PCLK_GPIO_NUM;
  c.pin_vsync = VSYNC_GPIO_NUM;  c.pin_href  = HREF_GPIO_NUM;
  c.pin_sccb_sda = SIOD_GPIO_NUM;
  c.pin_sccb_scl = SIOC_GPIO_NUM;
  c.pin_pwdn  = PWDN_GPIO_NUM;   c.pin_reset = RESET_GPIO_NUM;
  c.xclk_freq_hz = 20000000;
  c.pixel_format = PIXFORMAT_JPEG;
  c.grab_mode    = CAMERA_GRAB_LATEST;
  c.fb_location  = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    c.frame_size   = FRAMESIZE_VGA;   // 640x480: far faster over softAP
    c.jpeg_quality = 12;
    c.fb_count     = 2;
  } else {
    Serial.println("!! NO PSRAM — set Tools > PSRAM > OPI PSRAM");
    c.frame_size   = FRAMESIZE_QVGA;
    c.jpeg_quality = 18;
    c.fb_count     = 1;
    c.fb_location  = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&c);
  if (err != ESP_OK) {
    Serial.printf("!! CAMERA INIT FAILED 0x%x\n", err);
    Serial.println("   0x105 = not detected (ribbon or pin map)");
    Serial.println("   0x101 = memory (PSRAM off)");
    return false;
  }

  sensor_t* s = esp_camera_sensor_get();
  if (s) {
    if (s->id.PID == OV3660_PID) {     // OV3660 ships inverted
      s->set_vflip(s, 1);
      s->set_brightness(s, 1);
      s->set_saturation(s, -2);
    }
    s->set_hmirror(s, 1);
  }

  // Throw away the first frames. The sensor needs a moment for exposure
  // and white balance to settle, and the very first grab is often black.
  for(int i=0;i<3;i++){
    camera_fb_t* w = esp_camera_fb_get();
    if(w) esp_camera_fb_return(w);
    delay(90);
  }
  return true;
}

// ---------------------------------------------------------------------
// LED RING
// The ring DECELERATES as confidence rises: the Authority appears to
// deliberate more heavily the more human you prove to be. That single
// detail is what makes it read as an instrument rather than a light.
// ---------------------------------------------------------------------
String   phase    = "idle";

// Descending "verdict" motif, stepped from loop() so nothing blocks the
// web server. A handler that sleeps stalls the server, and a stalled
// server is a failed /capture — so all timing lives in loop().
int      toneStep = -1;
uint32_t toneAt   = 0;
const int TONE_SEQ[5] = {880, 660, 523, 415, 330};

void beep(int freq, int ms){
  if(BUZZ_PIN < 0) return;
#if PASSIVE_BUZZ
  tone(BUZZ_PIN, freq, ms);
#else
  digitalWrite(BUZZ_PIN, HIGH); delay(ms > 120 ? 120 : ms); digitalWrite(BUZZ_PIN, LOW);
#endif
}

void animate(){
  uint32_t now = millis();
  if(toneStep >= 0 && now - toneAt > 150){
    toneAt = now;
    if(toneStep < 5) beep(TONE_SEQ[toneStep], 140);
    toneStep++;
    if(toneStep >= 5) toneStep = -1;
  }
}

void handleState(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  phase = server.hasArg("p") ? server.arg("p") : "idle";
  if(phase == "denied"){ toneStep = 0; toneAt = 0; }   // descending motif
  else if(phase == "beep"){ beep(1100, 90); }          // single alert beep
  server.send(200, "text/plain", "ok");
}

// ---------------------------------------------------------------------
void handleCapture() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-store");

  if (!cameraOK) { server.send(503, "text/plain", "camera offline"); return; }

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("capture: fb_get returned null");
    server.send(500, "text/plain", "capture failed");
    return;
  }
  Serial.printf("capture: %u bytes\n", (unsigned)fb->len);
  if (fb->format != PIXFORMAT_JPEG) {
    esp_camera_fb_return(fb);
    server.send(500, "text/plain", "not jpeg");
    return;
  }
  server.setContentLength(fb->len);
  server.send(200, "image/jpeg", "");
  server.sendContent((const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html",
    "<body style='background:#DFDBD0;color:#14130E;font-family:monospace;padding:26px'>"
    "<h3 style='letter-spacing:.18em'>HVA CAMERA ANNEX</h3>"
    "<p>camera &nbsp; <b>" + String(cameraOK ? "READY" : "OFFLINE") + "</b></p>"
    "<p>address &nbsp;" + WiFi.softAPIP().toString() + "</p>"
    "<p>clients &nbsp;" + String(WiFi.softAPgetStationNum()) + "</p>"
    "<p><a style='color:#96201A' href='/capture'>/capture</a></p>"
    "<img src='/capture' width='440' style='border:2px solid #14130E'>"
    "</body>");
}

// ---------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\nHVA CAMERA ANNEX");

  cameraOK = initCamera();
  Serial.println(cameraOK ? "camera OK" : "camera FAILED");

  if(BUZZ_PIN >= 0){ pinMode(BUZZ_PIN, OUTPUT); digitalWrite(BUZZ_PIN, LOW); }
  beep(1300, 70);            // one chirp at boot: buzzer is alive
  Serial.println("buzzer armed");

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255,255,255,0));
  WiFi.softAP(AP_SSID, AP_PASS);
  WiFi.setSleep(false);
  delay(600);

  Serial.println();
  Serial.println("=====================================");
  Serial.printf ("  WIFI      %s\n", AP_SSID);
  Serial.printf ("  PASSWORD  %s\n", AP_PASS);
  Serial.print  ("  ADDRESS   http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("=====================================");
  Serial.println();

  server.on("/capture", HTTP_GET, handleCapture);
  server.on("/capture", HTTP_OPTIONS, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(204);
  });
  server.on("/state", handleState);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("server up on both interfaces");
}

// ---------------------------------------------------------------------
void loop() {
  server.handleClient();
  animate();
}
