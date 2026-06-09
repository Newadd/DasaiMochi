# 🍡 Dasai Mochi - Smart Mochi dengan Ekspresi Mata dan Efek Animasi

Proyek ESP32 + OLED SSD1306 + Sensor sentuh TTP223.  
Mochi ini dapat berubah ekspresi (senang, sedih, kaget, ngantuk) dengan sentuhan.

## Fitur
- ✨ Efek denyut jantung pada mata
- 😉 Kedip acak
- 🎆 Partikel bintang saat ekspresi senang
- 🔄 Transisi halus antar ekspresi
- 📦 Auto-build via GitHub Actions

## Wiring
| ESP32 | TTP223 | OLED     |
|-------|--------|----------|
| 3.3V  | VCC    | VCC      |
| GND   | GND    | GND      |
| GPIO4 | OUT    | -        |
| GPIO21| -      | SDA      |
| GPIO22| -      | SCL      |

## Cara Upload
1. Download firmware dari GitHub Actions → Artifacts
2. Gunakan esptool di Termux/PC:
   ```bash
   esptool.py --port /dev/ttyUSB0 write_flash 0x1000 firmware.bin