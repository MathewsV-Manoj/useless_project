<p align="center">
  <img src="./useless-project-banner.png" width="100%">
</p>


# ANTI-HUMAN CAPTCHA🎯

## Basic Details

### Team Name: TUSKERS

### Team Members
- **Member 1:** Mathews V Manoj - Muthoot Institute of Technology and Science
- **Member 2:** Shone Reji - Muthoot Institute of Technology and Science

### Project Description
A bureaucratic "reverse CAPTCHA" that treats being human as a suspicious activity. It runs entirely in your browser using your **laptop webcam** — subjecting you to three solemn verification stages, and the moment it decides you are in fact human, it snaps your photo and books you into a stylized orange-jumpsuit mugshot with an interactive Officer Tom who catches the human at the end. No hardware required — an optional ESP32-S3 build exists purely as a bonus.


### The Problem (that doesn't exist)

### The Solution (that nobody asked for)
The **Humanity Verification Authority** — a fully offline, self-contained enforcement agency that runs right in **Chrome on your laptop**. It runs Latency, Computation, Deviation, and Deliberation tests specifically rigged so that even the *best possible human* only reaches 95.3% machine-likeness. You fail for having a nervous system. Upon conviction, your **laptop webcam** photographs you, dresses you in a virtual orange jumpsuit, places you against a height chart, labels you EXHIBIT A, and beeps at you three times. (For the truly committed, a bonus ESP32-S3 hardware build is documented separately.)


## Interdisciplinary fields Included
Embedded Systems & IoT, Web Development, Human–Computer Interaction, Data Processing, Audio Technology, and UI/UX Design.

## Technical Details

### Technologies/Components Used

**For Software (Primary — Laptop Mode):**
- HTML5, CSS3, JavaScript (runs entirely in Chrome)
- `getUserMedia` / laptop webcam capture
- Web Audio API (browser-generated beep)
- Python 3 (for local `http.server`)

**For Hardware (Bonus — Optional ESP32-S3 Mode):**
- C / C++ (Arduino framework)
- ESP-IDF `esp32-camera` driver
- Arduino IDE
- ESP32-S3 development board with PSRAM (ESP32-S3 N16R8)
- OV3660 camera module
- USB cable
- A computer or phone with Wi-Fi

  ## System Architecture

<p align="center">
  <img src="./architecture_diagram.jpeg" width="90%">
</p>

1. ESP32-S3 Camera Module

The ESP32-S3 acts as the embedded processing and camera unit. It provides the Wi-Fi access point, hosts the local web interface, and captures an image when a human is detected.

2. Web-Based Verification Layer

A browser-based interface presents the reverse-CAPTCHA tests and collects the user's behavioural responses without requiring an external server.

3. Behavioural Analysis Layer

The system measures characteristics such as response latency, computation performance, hand movement deviation, and deliberation time to distinguish human-like behaviour from machine-like behaviour.

4. Scoring & Decision Layer

The collected measurements are processed using predefined thresholds and a scoring function to calculate a human-confidence score and produce the final verdict.

5. Human Detection & Trigger Layer

When the calculated result indicates human behaviour, the system triggers the next stage of the process—audio feedback and camera capture.

6. Camera Capture Layer

The ESP32-S3 camera captures the detected person's image. The captured frame is then transferred to the web interface for presentation.

7. Mugshot Processing & Presentation Layer

The captured photograph is transformed into the project's jail/mugshot-style visual, including the orange uniform effect and associated case information.

8. Audio Feedback Layer

The browser generates a beep sound when human behaviour is detected, providing immediate feedback without requiring a physical buzzer.

9. Interactive UI Layer

The interface presents the verification process, verdict, captured mugshot, and interactive elements such as the cat animation, making the system intentionally absurd and engaging. 

### Implementation

#### Run — Laptop Mode (Primary — Recommended)
No hardware needed. Everything runs in your browser.
1. In the project folder, start a local web server:
   ```
   python -m http.server 8000
   ```
2. Open `http://localhost:8000` in **Chrome**.
3. Allow camera access when prompted (the verdict mugshot uses your laptop webcam).
4. Click **BEGIN ATTESTATION** and submit to the verification process.

> Note: the webcam capture requires a secure context, so serving over `localhost` (as above) is important — opening `index.html` directly as a `file://` will block the camera.

#### Run — Bonus Hardware Mode (ESP32-S3)
An optional physical build where an ESP32-S3 hosts its own Wi-Fi access point and captures the mugshot from an OV3660 camera. This is a bonus and is **not required** to run or evaluate the project.

For full hardware wiring, flashing, and setup instructions, see [`SETUP.md`](SETUP.md).


### Project Documentation

**For Software:**

#### Screenshots
![Interface](screenshots/interface.png)
*The HVA verification interface — three solemn tests await.*

![Verdict](screenshots/verdict.png)
*HUMAN — IDENTIFIED. All three beeps fire simultaneously.*

![Mugshot](screenshots/mugshot.png)
*EXHIBIT A: the enhanced booking photo with jumpsuit, height chart, and placard.*

#### Diagrams
```
              ┌──────────────────────┐
              │      ESP32-S3        │
              │   Wi-Fi + Camera     │
              └──────────┬───────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │   HVA Web Interface  │
              └──────────┬───────────┘
                         │
                         ▼
              ┌──────────────────────┐
              │ Verification Stages  │
              │  1. Latency          │
              │  2. Computation      │
              │  3. Deviation        │
              │  4. Deliberation     │
              └──────────┬───────────┘
                         │
                         ▼
                   HUMAN VERDICT
                         │
              ┌──────────┴──────────┐
              ▼                     ▼
        🔊 Browser Beep       📷 ESP32 Capture
                                    │
                                    ▼
                           🟧 Mugshot Display
```

**For Hardware:**

#### Camera Pin Mapping (OV3660 → ESP32-S3)
```
XCLK→15   SIOD→4    SIOC→5
Y9→16    Y8→17     Y7→18
Y6→12    Y5→10     Y4→8
Y3→9     Y2→11
VSYNC→6  HREF→7    PCLK→13
```

### Project Demo

#### Video
_No demo video link has been added yet._

#### Additional Demos
- **Laptop mode:** open `http://localhost:8000` in Chrome after starting the server above.
- (Bonus hardware mode) Direct camera test: `http://192.168.4.1/capture`
- (Bonus hardware mode) Camera status: `http://192.168.4.1/status`


## The Scoring (that guarantees you lose)

```
m = machine_likeness = (threshold / measured) ^ 0.3
confidence_human     = 1 − product(m)
```

| Subject             | Test 1 | Test 2 | Test 3 | Test 4     |
|---------------------|--------|--------|--------|------------|
| Typical adult       | 70.3%  | 83.7%  | 94.0%  | **98.6%**  |
| Best possible human | 64.0%  | 80.2%  | 86.9%  | **95.3%**  |

The verdict is predetermined. The data is not. You fail for having a nervous system.

## Team Contributions
- **Mathews V Manoj:** ESP32-S3 firmware, camera driver configuration, Wi-Fi access point setup, `/capture` and `/status` endpoints, hardware assembly.
- **Shone Reji:** Web interface (HTML/CSS/JS), verification stages logic, Web Audio API beep system, mugshot compositing (jumpsuit overlay, height chart, EXHIBIT A placard), scoring algorithm.

---

Made with ❤️ at TinkerHub Useless Projects

![Static Badge](https://img.shields.io/badge/TinkerHub-UselessProjects--25-blueviolet)
![Static Badge](https://img.shields.io/badge/TinkerHub-24-100000)
