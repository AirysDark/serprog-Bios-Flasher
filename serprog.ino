/*
 * Arduino Uno - serprog firmware (SPI programmer)
 * Works with flashrom: -p serprog:dev=/dev/ttyUSB0:115200
 *
 * Wiring to 25Q256 BIOS chip (SOIC-8):
 *   CS#   -> D10
 *   MISO  -> D12
 *   MOSI  -> D11
 *   SCK   -> D13
 *   GND   -> GND
 *   VCC   -> 3.3V  (never 5V!)
 *   WP#   -> 3.3V
 *   HOLD# -> 3.3V
 */

#include <SPI.h>

#define PIN_CS 10
#define BAUD   115200

// serprog command definitions (subset)
#define S_CMD_NOP        0x00
#define S_CMD_Q_IFACE    0x01
#define S_CMD_Q_CMDMAP   0x02
#define S_CMD_Q_PGMNAME  0x03
#define S_CMD_Q_SERBUF   0x04
#define S_CMD_Q_BUSTYPE  0x05
#define S_CMD_SYNCNOP    0x10
#define S_CMD_S_BUSTYPE  0x11
#define S_CMD_O_SPIOP    0x13
#define S_CMD_SYNC       0x14

#define S_ACK 0x06
#define S_NAK 0x15

void cs_low()  { digitalWrite(PIN_CS, LOW); }
void cs_high() { digitalWrite(PIN_CS, HIGH); }

void setup() {
  pinMode(PIN_CS, OUTPUT);
  cs_high();
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  Serial.begin(BAUD);
}

void loop() {
  if (!Serial.available()) return;
  uint8_t cmd = Serial.read();
  switch (cmd) {
    case S_CMD_NOP:
      Serial.write(S_ACK);
      break;
    case S_CMD_SYNC:
    case S_CMD_SYNCNOP:
      Serial.write(S_ACK);
      break;
    case S_CMD_Q_IFACE:
      Serial.write((uint8_t)1); // version LSB
      Serial.write((uint8_t)0); // version MSB
      break;
    case S_CMD_Q_CMDMAP:
      for (int i=0; i<32; i++) {
        uint8_t b = 0;
        if (i==0) b = 0x1F; // supports basic cmds
        if (i==1) b = 0x03; // SPI
        Serial.write(b);
      }
      break;
    case S_CMD_Q_PGMNAME:
      Serial.write("ArduinoUnoSerprog\0");
      break;
    case S_CMD_Q_SERBUF:
      Serial.write((uint8_t)0xFF);
      Serial.write((uint8_t)0x00); // 255 buffer
      break;
    case S_CMD_Q_BUSTYPE:
      Serial.write((uint8_t)1); // SPI
      break;
    case S_CMD_S_BUSTYPE:
      (void)Serial.read(); // discard arg
      Serial.write(S_ACK);
      break;
    case S_CMD_O_SPIOP: {
      // SPI operation: len write(2) + len read(2)
      while (Serial.available() < 4);
      uint16_t outlen = Serial.read() | (Serial.read() << 8);
      uint16_t inlen  = Serial.read() | (Serial.read() << 8);

      uint8_t buf[256];
      for (uint16_t i=0; i<outlen; i++) {
        while (!Serial.available());
        buf[i] = Serial.read();
      }
      cs_low();
      for (uint16_t i=0; i<outlen; i++) SPI.transfer(buf[i]);
      for (uint16_t i=0; i<inlen; i++) {
        Serial.write(SPI.transfer(0xFF));
      }
      cs_high();
      break;
    }
    default:
      Serial.write(S_NAK);
      break;
  }
}