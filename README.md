# RealADSB for MSX

RealADSB is an app for airplane tracking that utilizing adsb_hub3 (version 3.7.4+) traffic feed. adsb_hub3 should have smartwatch output configured. More information you can find at http://www.realadsb.com 

Currently it's only running on Apple hardware: iPhone, iPad, AppleTV and Apple Watch.

As a big fan of MSX computers I always had an idea to port RealADSB on it but only recently I've purchased Sony MSX2 that can be used for that.

In order to access traffic feed GR8NET cartridge has to be used.

r1.bas - DEMO version of the application that can run on any MSX2 computer without GR8NET cartridge

r2.bas - real application that using GR8NET to get traffic feed and METAR from local adsb_hub3 server running on Rapberry Pi, PC or Mac

![RealADSB for MSX Basic](https://github.com/klimchuk/realadsb_msx/blob/main/screenshot.jpeg)

BASIC is super slow on MSX and update of 10 airplanes on screen takes more than 20 seconds. I will work on optimization later on.

r3.c - RealADSB client written on C language and using Fusion-C 1.2 MSX library

Biggest advantage of using C is speed: refresh of picture on screen with 10 airplanes is taking just 3-4 seconds and that's roughly 6 times faster than r2.bas able to do.

Cursor keys up/down can be used to change zoom, L key is showing list of aircrafts sorted by distance, M key is showing METAR for airport selected with r3 config tool, Q closes the application, 0-9 keys are used to select aircraft on radar.

r3config.c - configuration tool for RealADSB client, it allows to enter ICAO code of the reference airport, latitude, longitude and IP:port of adsb_hub3 server

# Credits

https://www.msx.org/wiki/Category:MSX-BASIC_Instructions 

http://msx.jannone.org/tinysprite/tinysprite.html 
