//
// RealADSB client for MSX-DOS
// Based on Fusion-C library
//

#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/vdp_circle.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

char XA[10][80]; // ICAO24
float XB[10]; // Latitude
float XC[10]; // Longitude
int XD[10]; // Altitude, ft
char XE[10][80]; // Callsign
int XF[10]; // Heading
int XG[10]; // Speed, kts
int XH[10]; // Vertical speed, ft/min
char XI[10][80]; // Aircraft type
char XJ[10][80]; // Tail number
int XK[10]; // Squawk
char XL[10][80]; // Owner or operator
float XM[10]; // Distance, mi

char Selected[6]="";
int Zoom = 20; // Zoom, mi

char CfgName[80] = "B:REALADSB.CFG"; // Name of configuration file
char Airport[6] = "KEWR"; // ICAO code of the airport
float Latitude = 40.6924798; // Latitude of the airport
float Longitude = -74.1686868; // Longitude of the airport
char IPPort[80] = "192.168.1.153:5567"; // IP:port of adsb_hub3

void loadSprites()
{
    return;
}

void showList(void)
{
    return;
}

void showMetar(void)
{
    return;
}

void main(void) 
{
    char tmpString[80];
    
    Screen(0);
    Width(80);
    PrintString("RealADSB 0.3 for MSX\r\n");
    PrintString("--------------------\r\n");
    PrintString("Requires GR8NET cartridge for network access\r\n");
    printf("Loading configuration from %s...\r\n", CfgName);
    // TODO: Load configuration from file
    printf("Airport ICAO code [%s]:", Airport);
    if(InputString(tmpString, 6)==4)
        StrCopy(Airport, tmpString);
    printf("Your latitude [%f]:", Latitude);
    if(InputString(tmpString, 20)>0)
        Latitude = atof(tmpString);
    printf("Your longitude [%f]:", Longitude);
    if(InputString(tmpString, 20)>0)
        Longitude = atof(tmpString);
    printf("adsb_hub3 IP:port [%s]:", IPPort);
    if(InputString(tmpString, 80)>0)
        StrCopy(IPPort, tmpString);
    // Setting 512x212 16 colors
    Screen(7);

    loadSprites();
    
    SetColors(15,4,4);
    Cls();

    // Draw radar
    Line(256-210,106,256+210,106,15,0);
    Line(256,2,256,210,15,0);
    Circle(256,106,105,15,0);
    Circle(256,106,210,15,0);

    // Left top corner
    Locate(2,2);
    PrintString(Airport);

    while(1)
    {
        unsigned char key = WaitKey();
    
        if(key=='l' || key=='L')
            showList();
        else
        if(key=='m' || key=='M')
            showMetar();
        else
        if(key=='q' || key=='Q')
        {
            Screen(0);
            Exit(0);
        }
    }
}
 