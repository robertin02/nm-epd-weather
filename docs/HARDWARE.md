# Hardware

emini Home 0.5.0 supports one device: the **RockBase NM-EPD-420** Devkit with the
three-colour display.

| Part | Details |
| --- | --- |
| Display | 4.2" 400 × 300 e-paper, black, white, and red pigments |
| Chip | ESP32-S3 with native USB |
| Memory | 16 MiB flash, 8 MiB octal PSRAM |
| Controls | USER (GPIO 45) and BOOT (GPIO 0) on the board |
| Sensors | Onboard AHT20 Temperature and Humidity sensor (I2C) |
| Connectivity | 2.4 GHz Wi-Fi |

The original ZECTRIX NOTE4C hardware is not covered by this specific port. A USB
chip ID only tells you the board has an ESP32-S3, and the preflight check in
the [installation guide](INSTALL.md) cannot tell different boards apart. Check
that the device in front of you is a RockBase NM-EPD-420 with the three-colour display
before you install.

## Pins

Pin mapping has been entirely adapted for the RockBase hardware configuration. It follows the manufacturer's
[ESP32-S3 GPIO map](https://github.com/RockBase-iot/NM-EPD-420#33-esp32-s3-gpio-map).

| Function | GPIO |
| --- | --- |
| Display SPI clock, data, chip select | 2, 1, 46 |
| Display data/command, reset, busy | 4, 5, 6 |
| Display power | 21 |
| Buttons: USER, BOOT | 45, 0 |
| Native USB D−, D+ | 19, 20 |
| Battery voltage | 3 (ADC channel 2) |
| Battery ADC Enable | 43 |
| AHT20 I2C SDA, SCL | 39, 38 |
| AHT20 Power Enable | 40 |

## Display

A frame is 30,000 bytes, split into two 15,000-byte layers for a three-colour (black, white, red) display.
Home sends only full refreshes. On the tested unit, powered over
USB, a full change of the image takes time depending on the exact E-Ink panel model. Home skips the refresh
when the new frame is identical to the one on screen.

Dither patterns never place colours in steps finer than 2 pixels, while
black and paper patterns can use single pixels.

Colours in screenshots and on the website are an approximation of the
pigments, not a colorimeter measurement.

## Display driver and time zones

The display driver in `firmware/main/home_panel.c` adapts the pin map and the
command sequence for the RockBase display. Home's changes: it sends only full refreshes, checks
that the panel really starts each refresh, waits for the BUSY line for at most
120 seconds, and stops with a fault instead of retrying when the panel does not
respond.

The time zone table in `firmware/main/generated/home_zones.c` is compiled from
the [IANA Time Zone Database](https://www.iana.org/time-zones), release 2026c,
which is in the public domain.

## Battery indicator

Home reads the battery voltage ten times every 30 seconds and averages the
calibrated readings. On the RockBase NM-EPD-420, it reads ADC channel 2 on GPIO 3, and enables the voltage divider by pulling GPIO 43 high before reading. Readings outside 2,800–4,350 mV count as unknown.
Charger signals must stay stable for a second before the panel shows them.

While not charging, the panel estimates a percentage from the voltage with the
curve clamped to 0–100 %:
`(-V*V + 9016*V - 19189000) / 10000`, where `V` is in millivolts. It is a
rough estimate, not a fuel gauge, and it is hidden while charging. Battery
life has not been measured yet, and this release keeps Wi-Fi on without deep
sleep.

## Environmental Sensor

Unlike the original emini Home which relied solely on internet weather APIs, this port utilizes the onboard AHT20 sensor to display local indoor temperature and humidity. The sensor runs on a dedicated FreeRTOS background task, querying the I2C bus every 10 seconds and feeding the data to the main rendering engine.

## Flash layout

The factory layout on the tested unit, and what emini Home adds:

| Name | Offset | Size | Written by Home |
| --- | --- | --- | --- |
| bootloader | `0x0` | 32 KiB | no |
| partition table | `0x8000` | 3 KiB | yes, adds `home_nvs` |
| nvs (factory) | `0x9000` | 16 KiB | no |
| otadata | `0xD000` | 8 KiB | no |
| phy_init | `0xF000` | 4 KiB | no |
| **home_nvs** | `0x10000` | 64 KiB | yes, created in an unused gap |
| ota_0 | `0x20000` | 4,032 KiB | yes, application |
| ota_1 | `0x410000` | 4,032 KiB | no |
| assets | `0x800000` | 8 MiB | no |

Home keeps its settings, Wi-Fi details, paired browsers and last good data in
`home_nvs`. See [installation](INSTALL.md) for the exact write commands.