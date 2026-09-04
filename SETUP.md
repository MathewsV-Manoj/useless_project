# THE HUMANITY VERIFICATION AUTHORITY

Two files, deliberately separate:

    site    http://localhost:8000     python -m http.server 8000
    board   http://192.168.4.1        wifi HVA-ANNEX / verify00

The board serves only the camera and sounds a buzzer. Keeping the website
off the board is what makes it load instantly and stay responsive.

---

## WHAT CHANGED

- **All LEDs removed.** The board now only sounds a buzzer. ((Our original plan was to implement it as led with loading features etc))
- **Two beeps, two sources.** The ESP32 buzzer, the laptop speaker, and
  the in-page shutter all fire the moment a human is identified.
- **The exhibit is now a full booking mugshot** — the enhanced photo dropped
  onto a height chart, dressed in an orange jumpsuit, with a placard reading
  SUBJECT NN-NNNN and a red HUMAN — IDENTIFIED banner.
- **The photo is enhanced** — contrast, a cool forensic grade, and a light
  sharpen — before it is composited.

---

## STEP 1 — Buzzer (2 min)

Buzzer + leg to **GPIO 14**, - leg to **GND**. No buzzer? Set
`#define BUZZ_PIN -1` near the top of the sketch — the laptop still beeps.

**Not pins 4, 5, 6, 7** — those are camera lines.

## STEP 2 — Upload the sketch (5 min)

((BOARD TO BE SELECTED -- ESP 32 S3 DEVMODULE))
New sketch, paste `hva_camera.ino`

    USB CDC On Boot    Enabled
    Flash Size         16MB (128Mb)
    PSRAM              OPI PSRAM
    Partition Scheme   16M Flash (3MB APP/9.9MB FATFS)

Close Serial Monitor. Upload.

## STEP 3 — Boot check

Serial 115200, press RESET. One chirp from the buzzer. You want:

    camera OK
    buzzer armed
      ADDRESS   http://192.168.4.1

`0x105` -> reseat the ribbon.  `0x101` -> PSRAM not OPI.

## STEP 4 — Turn OFF the phone hotspot

Leave laptop wifi ON — you need it on, joined to the board.

## STEP 5 — Connect

Windows wifi -> **HVA-ANNEX** / **verify00**. Accept "No internet".

    ipconfig

Must read **192.168.4.2**. Disable McAfee VPN if the address still fails.

## STEP 6 — Test the camera alone

    http://192.168.4.1/capture

Photo -> good. Nothing -> watch serial while reloading:
`capture: NNNN bytes` = board is fine, browser side; `fb_get returned null`
= reseat ribbon; nothing = not on the network.

## STEP 7 — Run the site

    cd path\to\hva_final
    python -m http.server 8000

Chrome -> **http://localhost:8000**. Run all four tests. At the verdict the
mugshot builds and all three beeps fire.

## STEP 8 — Aim the camera at the chair.

---

## THE BEEP

Fires on identification from all two at once:
- laptop speaker (`beepLocal`, a two-tone square-wave alert)
- in-page shutter noise

Even with no board attached, the laptop beeps.

---

## SCORING (for your README)

    m = machine_likeness = (threshold / measured) ^ 0.3
    confidence_human     = 1 - product(m)

| Subject | t1 | t2 | t3 | t4 |
|---|---|---|---|---|
| Typical adult | 70.3% | 83.7% | 94.0% | **98.6%** |
| Best possible human | 64.0% | 80.2% | 86.9% | **95.3%** |

Best possible human still lands at 95.3%. The verdict is predetermined; the
data is not. You fail for having a nervous system.
