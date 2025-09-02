Install first

flashrom -p serprog:dev=/dev/ttyUSB0:115200 -r backup.bin
flashrom -p serprog:dev=/dev/ttyUSB0:115200 -w unlocked_backup.bin


⚡ Arduino Uno serprog.ino

Upload this with Arduino IDE.
Baud is 115200 by default (can be increased if stable).
Make sure your Uno is wired to the SPI flash at 3.3 V only.

---

🔌 Wiring Recap

D13 → CLK

D12 → DO (MISO)

D11 → DI (MOSI)

D10 → CS#

GND → GND

3.3 V → VCC

3.3 V → WP# and HOLD#


Use a SOIC-8 clip for clean connection.


---

🖥️ PC Side (Linux/Windows with flashrom)

Detect chip:

flashrom -p serprog:dev=/dev/ttyUSB0:115200 -R

Backup:

flashrom -p serprog:dev=/dev/ttyUSB0:115200 -r backup.bin

Write unlocked dump:

flashrom -p serprog:dev=/dev/ttyUSB0:115200 -w unlocked_backup.bin

The Uno is slow — a full 32 MB dump may take ~1 hour. If you want speed, a CH341A programmer is better.

Always verify the backup works (compare file size = 33,554,432 bytes for MX25L256).

If flashrom fails with timeouts, lower baud (57600) or shorten clip wiring