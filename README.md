# nm-epd-weather

**A calm, three-colour poster of your day for the RockBase NM-EPD-420 e-paper devkit.**

`0.6.0` · Forked from [fiedoruk/emini-home](https://github.com/fiedoruk/emini-home) · tested on one NM-EPD-420 · ESP-IDF v6.0.2 · MIT

<p align="center">
  <img src="docs/images/note4c-photo.webp" width="720" alt="A RockBase NM-EPD-420 on a fridge door running nm-epd-weather 0.6.0. The Weather screen in the Print composition shows 15° in Czaplinek, 12–17 °C over 24 hours, cloud cover, a dithered band of the next hours, and the line Dry until 06:00 · Wind 3.4 m/s.">
</p>
<p align="center"><sub>Photo of a device running 0.6.0 · Weather in the Print composition</sub></p>

nm-epd-weather puts the weather forecast, your indoor climate, one headline, a note in your own words,
the sky above you and the air you breathe on the RockBase NM-EPD-420's 400 × 300 display, using its black, white, and red
pigments. You set it up in your phone's browser, without an app or an
account. After that the device does the rest on its own, and no emini.ink
server sits in between.

This project is a custom port of the original `emini-home` repository, adapted specifically for the RockBase NM-EPD-420 which features an ESP32-S3 with 16 MiB flash, 8 MiB PSRAM, and an onboard AHT20 temperature and humidity sensor.

[Install](docs/INSTALL.md) · [Phone panel](docs/PANEL.md) · [Build](docs/BUILD.md) · [Hardware](docs/HARDWARE.md)

> [!WARNING]
> Version 0.6.0 has been installed and tested on one RockBase NM-EPD-420. Read the
> [status](#status) before you install. The preflight check in the
> installation guide tells you to stop if your device's boot data differs
> from the tested one. It cannot tell a monochrome board from a three-colour one, and only the
> three-colour NM-EPD-420 is supported.

## One forecast, three compositions

<p align="center">
  <img src="docs/images/epaper-weather-rhythm.png" width="400" alt="Rhythm composition: a sample forecast for Lisbon with a dithered temperature curve for the next hours">
  <img src="docs/images/epaper-weather-atlas.png" width="400" alt="Atlas composition: the same sample forecast with a large sun and cloud on the left half">
</p>
<p align="center"><sub>Rhythm and Atlas, drawn on a computer by the renderer from sample data. The photo at the top shows Print.</sub></p>

Rhythm draws the coming hours as a curve, and Atlas gives the sky half of the
page. Each screen can keep one composition, or use **In turn** and move on to
the next one each time it comes back to the display. The weather screen shows
the range for the next 24 hours under the temperature and a short line about precipitation,
such as “Rain from 18:00”.

There is no grey on this display, so the renderer mixes the three pigments in
ordered dither patterns to draw warmth, light and cloud. Coloured patterns are
never finer than 2 pixels, while black and paper patterns can still use single
pixels.

## Sky and Air

Two more screens arrived in earlier versions, both switched off until you enable them in the panel.
**Sky** shows sunrise, sunset, the length of the day and the Moon's phase, worked out on the
device from your saved location; nothing is downloaded for it. **Air** shows the European
air quality index, PM2.5 over the next 24 hours, the UV index with a sunscreen hint and,
in Europe, four pollens, from Open-Meteo's Air Quality service (CC BY 4.0). Each has the
same three compositions as the weather.

<p align="center">
  <img src="docs/images/epaper-sky-print.png" width="400" alt="The Sky screen in the Print composition: the sunset time in large type, the length of the day, a warm dome with the sun in its current position and a strip of the whole day from night through dawn, day and dusk">
  <img src="docs/images/epaper-air-print.png" width="400" alt="The Air screen in the Print composition: the European air quality index in large type, the word Good, 24 hourly bars on a warm scale, a UV sun, the UV line and four pollen tiles">
</p>
<p align="center"><sub>Sky (Warsaw, equinox) and Air (Berlin, May), drawn from sample data</sub></p>

## Your brush

Three pigments and no grey mean every tone on this display is a pattern. The
panel lets you choose the brush that paints it, in Settings → Appearance: grain (blue noise,
the default), halftone dots, or the ordered grid.

## One headline and your note

<p align="center">
  <img src="docs/images/epaper-news-print.png" width="400" alt="The News screen: one headline from a news feed">
  <img src="docs/images/epaper-note-rhythm.png" width="400" alt="The Your note screen: a personal note in large type above a dithered band">
</p>
<p align="center"><sub>News and Your note, drawn from sample data</sub></p>

News comes from BBC World by default, or from a public RSS or Atom feed of your
choice, as long as it is served over HTTPS. The note is yours: a reminder, a
line from a friend, a few words to keep in view.

Headlines and notes in Simplified Chinese are drawn with Noto Sans CJK glyphs
(the whole of GB 2312, 6 763 characters, plus punctuation); lines break between
characters, so a Chinese feed such as a news site's RSS works as it is.
The screens themselves speak Simplified Chinese too: every label, footer
and sentence, the date as 9月15日, and air quality, UV and pollen by name.
Choose the language in the phone panel, or hold the lower side button for five
seconds on the device.

## Indoor Environmental Sensor

Unlike the original emini-home which relied solely on internet weather APIs, this port utilizes the RockBase NM-EPD-420's onboard AHT20 sensor to display local indoor temperature and humidity. The sensor runs on a dedicated FreeRTOS background task, querying the I2C bus every 10 minutes and feeding the data to the main rendering engine.

## Set it up from your phone

On first start the display shows a setup screen. Join the **emini.ink** Wi-Fi
network it shows, open `http://192.168.4.1`, type the pairing code from the
display, and move the device onto your home network. From then on the panel
lives at the device's own address on that network (Settings → Your device);
the setup network exists only for the 5-minute setup window. In Breath, the
default power mode, press the round OK button on the device first and give it a
few seconds: the panel then answers for five minutes. Then search for your town,
pick your screens and their compositions, and choose one screen, a day rhythm
or a rotation, with quiet hours for the night.

<p align="center">
  <img src="docs/images/epaper-setup.png" width="400" alt="The setup screen on the display: QR codes for the setup Wi-Fi and the panel address, and a pairing code">
  <img src="docs/images/panel-pair-crop.webp" width="195" alt="Pairing page in the phone browser: the six-digit code and the Connect this phone button">
  <img src="docs/images/panel-location-search-crop.webp" width="195" alt="Town search on the Weather page: Warsaw typed in, and matching places with their region and country">
</p>
<p align="center"><sub>Panel screenshots were taken in a browser on a computer, with sample data.</sub></p>

The [phone panel guide](docs/PANEL.md) walks through every step.

## Breath and Open

Since 0.6.0 nm-epd-weather has two power modes, chosen in the panel under Settings → Battery.

* **Breath**, the default, turns Wi-Fi off between downloads. About twice an hour the device
  switches the radio on, fetches the weather and the news, and lets it sleep again. While it sleeps the phone
  panel cannot reach the device. Press the round OK button, wait a few seconds, and the
  panel opens for five minutes. The five minutes start again whenever you ask for
  something in the panel, such as opening it, a preview or a save; what the panel checks by
  itself does not count.

* **Open** keeps Wi-Fi connected, so the panel answers at any time, as in earlier releases.
  The battery runs down roughly two to four times faster, by our estimate.

On a USB cable and while the setup window is open, the device keeps Wi-Fi on in either mode.

## Install

The full manual installation requires `esptool` and a terminal. The
[installation guide](docs/INSTALL.md) has eight steps, and most of the time
goes into two full backups. In short:

1. Back up the whole flash twice.

2. Run [`tools/preflight.py`](tools/preflight.py) on the backups. It compares
   them with the tested RockBase NM-EPD-420 and says `READY` or `STOP`.

3. Write two files from the release:
   the partition table at `0x8000` and the application at `0x20000`. The
   bootloader and factory data stay untouched.

4. Verify, start and continue on your phone.

## Status

Where nm-epd-weather 0.6.0 stands:

* **Tested on one RockBase NM-EPD-420 Devkit** (ESP32-S3, 16 MiB flash, 8 MiB octal PSRAM). The preflight check compares your backups with that unit's factory
  flash.

* **Phone setup** has been done end to end from an iPhone on that unit, from
  the setup Wi-Fi to the panel on the home network. Android phones have not
  been tried yet.

* **Starting over**, which erases only the settings area, has been done on
  that unit: the area read back empty and the device opened the setup screen.

* **Location** comes from a town you search for in the panel, or from an
  estimate based on your internet address, which can land on your provider's
  city.

* **Battery life depends on the power mode.** It has not been comprehensively tested for the NM-EPD-420 in this port. The panel shows voltage and a rough percentage.

* **Updates** are installed over USB. There is no over-the-air update mechanism.

## Privacy

The device talks to MET Norway for weather, to the news feed you choose, to FreeIPAPI
for an approximate location and to public time servers. While the Air screen is
switched on, it also sends the saved coordinates to the Open-Meteo Air Quality
API for air quality, UV and pollen. Weather, news and air quality are fetched only
while their screens are switched on. When you search for a town, your phone's
browser sends the search to Open-Meteo. Each of these services sees an ordinary
request from your internet address. Nothing goes to the original emini servers, and the firmware has
no analytics. The panel runs over HTTP on your local network and settings are
stored on the device without encryption, so keep the device on a network and in
a home you trust. Details: [privacy](docs/PRIVACY.md).

## Build it yourself

The firmware builds with ESP-IDF v6.0.2. The RockBase NM-EPD-420 requires external RAM for frame buffer allocations. Ensure that **Octal Mode PSRAM** is enabled in `idf.py menuconfig`. See
[building from source](docs/BUILD.md). Hardware details and the flash layout
are in [hardware](docs/HARDWARE.md).

## Security

Please report vulnerabilities privately through GitHub security advisories.
[SECURITY.md](SECURITY.md) lists the known limits of this release.

## Credits and licence

nm-epd-weather is released under the [MIT License](LICENSE).

* This port is forked from the original [emini-home by fiedoruk](https://github.com/fiedoruk/emini-home).

* Weather data from [MET Norway](https://api.met.no/), licensed CC BY 4.0.

* Air quality, UV and pollen from the [Open-Meteo Air Quality API](https://open-meteo.com/en/docs/air-quality-api), licensed CC BY 4.0.

* Place search by [Open-Meteo.com](https://open-meteo.com/), using location
  data from [GeoNames](https://www.geonames.org/), licensed CC BY 4.0.

* Approximate location from [FreeIPAPI](https://freeipapi.com/).

* Headlines belong to their publishers; the default feed is BBC World.

* Typeface: [Atkinson Hyperlegible Next](https://github.com/googlefonts/atkinson-hyperlegible-next) (SIL OFL 1.1).

* Chinese glyphs: [Noto Sans CJK SC](https://github.com/notofonts/noto-cjk) (SIL OFL 1.1), GB 2312 levels 1 and 2.

* Small text (12 and 16 px): [TRMNL12 and TRMNL16](https://github.com/usetrmnl/trmnl-framework) pixel fonts by Heavyweight Digital Type Foundry for TRMNL (SIL OFL 1.1).

* Built on [ESP-IDF](https://github.com/espressif/esp-idf); all components and
  licences are listed in [third-party notices](THIRD_PARTY_NOTICES.md).

RockBase and NM-EPD-420 may be trademarks of their owner. nm-epd-weather is an independent
project.