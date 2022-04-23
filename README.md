# RealADSB for MSX

RealADSB is an app for airplane tracking that utilizing adsb_hub3 traffic feed. adsb_hub3 should have smartwatch output configured. More information you can find at http://www.realadsb.com 

Currently it's only running on Apple hardware: iPhone, iPad, AppleTV and Apple Watch.

As a big fan of MSX computers I always had an idea to port RealADSB on it but only recently I've purchased Sony MSX2 that can be used for that.

In order to access traffic feed GR8NET cartridge has to be used.

r1.bas - DEMO version of the application that can run on any MSX2 computer without GR8NET cartridge

r2.bas - real application that using GR8NET to get traffic feed from local adsb_hub3 server and METAR

BASIC is super slow on MSX and update of 10 airplanes on screen takes more than 20 seconds. I will work on optimization later on.

r3.c - RealADSB client written on C language and using Fusion-C MSX library

# Credits

https://www.msx.org/wiki/Category:MSX-BASIC_Instructions 

http://msx.jannone.org/tinysprite/tinysprite.html 
