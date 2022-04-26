//
// RealADSB client for MSX-DOS
// Based on Fusion-C library
//

#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/vdp_circle.h"
#include "fusion-c/header/gr8net-tcpip.h"
#include "fusion-c/header/vdp_sprites.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

char NumberOfAirplanes;
char XA[10][10]; // ICAO24
float XB[10]; // Latitude
float XC[10]; // Longitude
int XD[10]; // Altitude, ft
char XE[10][12]; // Callsign
int XF[10]; // Heading
int XG[10]; // Speed, kts
int XH[10]; // Vertical speed, ft/min
char XI[10][6]; // Aircraft type
char XJ[10][20]; // Tail number
int XK[10]; // Squawk
char XL[10][20]; // Owner or operator
float XM[10]; // Distance, mi

char Selected[6]="";
int SelIndex;
int Zoom = 20; // Zoom, mi
float ZX, ZY;

char CfgName[20] = "REALADSB.CFG"; // Name of configuration file
char Airport[6] = "KEWR"; // ICAO code of the airport
float Latitude = 40.6924798; // Latitude of the airport
float Longitude = -74.1686868; // Longitude of the airport
char IPPort[80] = "192.168.1.153:5567"; // IP:port of adsb_hub3

char tmpString[256];
char latString[15];
char lonString[15];

FCB file;
tcpip_unapi_tcp_conn_parms tcp_conn_parms;
	
// Sprites
// Airplane up
static const unsigned char spriteUp[] = {0x00,0x01,0x01,0x01,0x01,0x01,0x07,0x1F,
0x7F,0x79,0x01,0x01,0x01,0x03,0x07,0x00,
0x00,0x80,0x80,0x80,0x80,0x80,0xE0,0xF8,
0xFE,0x9E,0x80,0x80,0x80,0xC0,0xE0,0x00};
// Airplane right
static const unsigned char spriteRight[] = {0x00,0x03,0x03,0x03,0x03,0x41,0x61,0x7F,
0x7F,0x61,0x41,0x03,0x03,0x03,0x03,0x00,
0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0xFE,
0xFE,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00};
// Airplane down
static const unsigned char spriteDown[] = {0x00,0x07,0x03,0x01,0x01,0x01,0x79,0x7F,
0x1F,0x07,0x01,0x01,0x01,0x01,0x01,0x00,
0x00,0xE0,0xC0,0x80,0x80,0x80,0x9E,0xFE,
0xF8,0xE0,80,0x80,0x80,0x80,0x80,0x00};
// Airplane left
static const unsigned char spriteLeft[] = {0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x7F,
0x7F,0x03,0x03,0x01,0x01,0x00,0x00,0x00,
0x00,0xC0,0xC0,0xC0,0xC0,0x82,0x86,0xFE,
0xFE,0x86,0x82,0xC0,0xC0,0xC0,0xC0,0x00};
// Digit 0
static const unsigned char sprite0[] = {0xE0,0xA0,0xA0,0xA0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 1
static const unsigned char sprite1[] = {0x40,0xC0,0x40,0x40,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 2
static const unsigned char sprite2[] = {0xE0,0x20,0xE0,0x80,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 3
static const unsigned char sprite3[] = {0xE0,0x20,0xE0,0x20,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 4
static const unsigned char sprite4[] = {0xA0,0xA0,0xE0,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 5
static const unsigned char sprite5[] = {0xE0,0x80,0xE0,0x20,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 6
static const unsigned char sprite6[] = {0xE0,0x80,0xE0,0xA0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 7
static const unsigned char sprite7[] = {0xE0,0x20,0x20,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 8
static const unsigned char sprite8[] = {0xE0,0xA0,0xE0,0xA0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// Digit 9
static const unsigned char sprite9[] = {0xE0,0xA0,0xE0,0x20,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

// Big thanks to https://github.com/aralbrec (z88dk) for giving me an idea
// x - the number to be converted
// f - number of digits to follow decimal point
// *str - output string
void ftoa(float x, int f,char *str)
{
        float scale;           /* scale factor */
        int i,                          /* copy of f, and # digits before decimal point */
            d;                              /* a digit */

        if( x < 0.0 ) {
                *str++ = '-' ;
                x = -x ;
        }
        i = f ;
        scale = 2.0 ;
        while ( i-- )
                scale *= 10.0 ;
        x += 1.0 / scale ;
        /* count places before decimal & scale the number */
        i = 0 ;
        scale = 1.0 ;
        while ( x >= scale ) {
                scale *= 10.0 ;
                i++ ;
        }
	if ( i == 0 )
	    *str++ = '0';

        while ( i-- ) {
                /* output digits before decimal */
                scale = floorf(0.5 + scale * 0.1 ) ;
                d = ( x / scale ) ;
                *str++ = d + '0' ;
                x -= (float)d * scale ;
        }
        if ( f <= 0 ) {
                *str = 0;
                return ;
        }
        *str++ = '.' ;
        while ( f-- ) {
                /* output digits after decimal */
                x *= 10.0 ;
                d = x;
                *str++ = d + '0' ;
                x -= d ;
        }
        *str = 0;
}

void initConnection(void)
{
    int i, index=0, pos=0;

    for(i=0; i<strlen(IPPort); i++)
    {
        char ch = IPPort[i];
        if(IsDigit(ch))
        {
            tmpString[pos]=ch;
            pos++;
        }
        else
        {
            tmpString[pos]=0;
            if(index==0)
                tcp_conn_parms.dest_ip[0]=atoi(tmpString);
            else
            if(index==1)
                tcp_conn_parms.dest_ip[1]=atoi(tmpString);
            else
            if(index==2)
                tcp_conn_parms.dest_ip[2]=atoi(tmpString);
            else
            if(index==3)
                tcp_conn_parms.dest_ip[3]=atoi(tmpString);
            index++;
            pos=0;
        }
    }
    if(pos>0)
        tcp_conn_parms.dest_port=atoi(tmpString);

	tcp_conn_parms.local_port=-1;
	tcp_conn_parms.user_timeout=-1;
	tcp_conn_parms.flags=0;
}

void loadSprites(void)
{
    SpriteReset();

    // Sprites separated by 4 for 16x16 
    // which is combination of 4 8x8 sprites 
    SetSpritePattern(0,spriteUp,32);
    SetSpritePattern(4,spriteRight,32);
    SetSpritePattern(8,spriteDown,32);
    SetSpritePattern(12,spriteLeft,32);
    SetSpritePattern(16,sprite0,32);
    SetSpritePattern(17,sprite1,32);
    SetSpritePattern(18,sprite2,32);
    SetSpritePattern(19,sprite3,32);
    SetSpritePattern(20,sprite4,32);
    SetSpritePattern(21,sprite5,32);
    SetSpritePattern(22,sprite6,32);
    SetSpritePattern(23,sprite7,32);
    SetSpritePattern(24,sprite8,32);
    SetSpritePattern(25,sprite9,32);

    SpriteOn();
    Sprite16();
    SpriteSmall();

    return;
}

void showList(void)
{
    char i;
    // Hide all sprites
    for(i=0; i<20; i++)
        PutSprite(i,0,-32,-32,15);

    SetColors(10,1,4);
    BoxFill(20,20,492,192,1,0);

    PutText(30, 25, "Nearest airplanes", 0);
    PutText(30, 43, "Callsign  Type  Tail      Alt,ft     Speed,kts  Dist,mi", 0);

    for(i=0; i<NumberOfAirplanes; i++)
    {
        PutText(30,55+i*12,XE[i],0);
        PutText(110,55+i*12,XI[i],0);
        PutText(158,55+i*12,XJ[i],0);
        sprintf(tmpString,"%d/%d",XD[i],XH[i]);
        PutText(230,55+i*12,tmpString,0);
        sprintf(tmpString,"%d",XG[i]);
        PutText(342,55+i*12,tmpString,0);
        ftoa(XM[i],2,tmpString);
        PutText(422,55+i*12,tmpString,0);
    }
    
    while(1)
    {
        unsigned char key = WaitKey();
        if(key==27)
            break;
    }
}

void showStaticMetar(void)
{
    char offset=0;
    char line=0;
    int pos;
    char *metar = "2022/04/09 00:51\r\nKEWR 090051Z 17006KT 10SM FEW060 14/M01 A2968 RMK AO2 PK WND 30028/1340 SLP088 T00891044";

    for(pos=0;pos<strlen(metar);pos++)
    {
        char ch=metar[pos];
        if(ch=='\r' || ch=='\n')
        {
            if(offset>0)
            {
                tmpString[offset] = 0;
                PutText(30,40+line*12,tmpString,0);
                line++;
                offset = 0;
            }
        }
        else
        {
            tmpString[offset] = ch;
            offset++;
            if(offset==50)
            {
                tmpString[offset] = 0;
                PutText(30,40+line*12,tmpString,0);
                line++;
                offset = 0;
            }
        }
    }
    if(offset>0)
    {
        tmpString[offset] = 0;
        PutText(30,40+line*12,tmpString,0);
    }
}

void showNetworkMetar(void)
{
    tcpip_unapi_tcp_conn_parms state_tcp_conn_parms;
    int a, i;
    int conn_number;
    char response[1024];

    char offset=0;
    char line=0;
    int pos;

    sprintf(tmpString,"GET /metar?icao=%s HTTP/1.0\r\nUser-Agent: RealADSB\r\nAccept: */*\r\nConnection: close\r\n\r\n", Airport);

    a=tcpip_tcp_open(&tcp_conn_parms, &conn_number);
	if(a==ERR_OK)
	{
		a=tcpip_tcp_state(conn_number, &state_tcp_conn_parms);
		if(a==ERR_OK)
		{
			if(state_tcp_conn_parms.send_free_bytes>=sizeof(tmpString))
			{
				a=tcpip_tcp_send(conn_number,tmpString,sizeof(tmpString),0);
				while((i=tcpip_tcp_state(conn_number,&state_tcp_conn_parms))==ERR_OK)
				{
					if((state_tcp_conn_parms.conn_state!=4) && state_tcp_conn_parms.incoming_bytes==0)
					{
						PutText(30,40,"TCP session finished",0);
						break;
					}
					if(state_tcp_conn_parms.incoming_bytes!=0)
					{
						a=tcpip_tcp_rcv(conn_number, &response[0], 1024, &tcp_conn_parms);
						if(a!=ERR_OK) 
                            break;
                        // Output
                        for(pos=0;pos<state_tcp_conn_parms.incoming_bytes;pos++)
                        {
                            char ch=response[pos];
                            if(ch=='\r' || ch=='\n')
                            {
                                if(offset>0)
                                {
                                    tmpString[offset] = 0;
                                    PutText(30,40+line*12,tmpString,0);
                                    line++;
                                    offset = 0;
                                }
                            }
                            else
                            {
                                tmpString[offset] = ch;
                                offset++;
                                if(offset==50)
                                {
                                    tmpString[offset] = 0;
                                    PutText(30,40+line*12,tmpString,0);
                                    line++;
                                    offset = 0;
                                }
                            }
                        }
					}
				}
			}
			else PutText(30,40,"No enough space in TX buffer",0);
		}
        if(offset>0)
        {
            tmpString[offset] = 0;
            PutText(30,40+line*12,tmpString,0);
        }

		a=tcpip_tcp_close(conn_number);
	}
    else
    {
        PutText(30,40,"Can't connect",0);
    }
}

void showMetar(void)
{
    char i;

    // Hide all sprites
    for(i=0; i<20; i++)
        PutSprite(i,0,-32,-32,15);

    SetColors(10,1,4);
    BoxFill(20,20,492,192,1,0);
    sprintf(tmpString,"METAR for %s", Airport);
    PutText(30,25,tmpString,0);
    Line(30,35,482,35,10,0);

    //showStaticMetar();
    showNetworkMetar();

    while(1)
    {
        unsigned char key = WaitKey();
        if(key==27)
            break;
    }
}

void changeZoom(void)
{
    sprintf(tmpString,"R:  %dmi  ",Zoom);
    PutText(435,172,tmpString,0);
    ZX=69.172*cosf(Latitude*3.14159/180)*1.05/(float)Zoom;
    ZY=69*1.05/(float)Zoom;
    return;
}

void loadTraffic(void)
{
    

}

void showSelected()
{
    // Copy ICAO code of aircraft
    StrCopy(Selected, XA[SelIndex]);
    PutText(2,162,XE[SelIndex],0);
    PutText(2,172,XI[SelIndex],0);
    PutText(2,182,XJ[SelIndex],0);
    sprintf(tmpString,"%dft %d", XD[SelIndex], XH[SelIndex]);
    PutText(2,192,tmpString,0);
    sprintf(tmpString,"%dkts", XG[SelIndex]);
    PutText(2,202,tmpString,0);
}

void FT_SetName( FCB *p_fcb, const char *p_name ) 
{
  char i, j;
  
  memset( p_fcb, 0, sizeof(FCB) );
  
  p_fcb->drive_no = 0;
  
  for( i = 0; i < 8; i++ ) {
    p_fcb->name[i] = ' ';
  }
  for( i = 0; (i < 8) && (p_name[i] != 0) && (p_name[i] != '.'); i++ ) {
    p_fcb->name[i] =  p_name[i];
  }
  if( p_name[i] == '.' ) {
    i++;
    for( j = 0; (j < 3) && (p_name[i + j] != 0) && (p_name[i + j] != '.'); j++ ) {
      p_fcb->ext[j] =  p_name[i + j] ;
    }
  }
}

void loadConfiguration()
{
    unsigned int n, pos, index=0, dstpos=0;
    char ch;
    char newline=0;

    FT_SetName(&file, CfgName);
    // Trying to open
    if(fcb_open(&file) == FCB_SUCCESS)
    {
        n = fcb_read(&file, tmpString, sizeof(tmpString));
        if(n>0)
        {
            for(pos=0; pos<n; pos++)
            {
                ch = tmpString[pos];
                if(IsAlpha(ch)>0 || IsDigit(ch)>0 || ch=='-' || ch=='.' || ch==':')
                {
                    // Add character
                    if(index==0)
                        Airport[dstpos] = ch;
                    else
                    if(index==1)
                        latString[dstpos] = ch;
                    else
                    if(index==2)
                        lonString[dstpos] = ch;
                    else
                    if(index==3)
                        IPPort[dstpos] = ch;
                    dstpos++;
                    newline = 1;
                }
                else
                {
                    if(newline==1)
                    {
                        if(index==0)
                        {
                            Airport[dstpos] = 0;
                            //printf("%s ", Airport);
                        }
                        else
                        if(index==1)
                        {
                            latString[dstpos] = 0;
                            //printf("%s ", latString);
                        }
                        else
                        if(index==2)
                        {
                            lonString[dstpos] = 0;
                            //printf("%s ", lonString);
                        }
                        else
                        if(index==3)
                        {
                            IPPort[dstpos] = 0;
                            //printf("%s ", IPPort);
                        }
                        newline=0;
                        dstpos=0;
                        index++;
                    }
                }
            }
        }
        fcb_close(&file);
    }
}

void saveConfiguration()
{
    FT_SetName(&file, CfgName);
    // Trying to open
    if(fcb_open(&file) != FCB_SUCCESS)
    {
        // Can't open then create
        if(fcb_create(&file) != FCB_SUCCESS)
        {
            printf("ERROR");
            return;
        }
    }
    fcb_write(&file, tmpString, strlen(tmpString));
    fcb_close(&file);
    printf("OK\r\nPress any key...");
}

int main(void) 
{
    Screen(0);
    Width(80);
    PrintString("RealADSB 0.3 for MSX-DOS\r\n");
    PrintString("--------------------\r\n");
    PrintString("Requires GR8NET cartridge for network access\r\n");

    // Load configuration
    ftoa(Latitude,7,latString);
    ftoa(Longitude,7,lonString);
    printf("Loading configuration from %s...\r\n", CfgName);
    loadConfiguration();
    //
    printf("Airport ICAO code [%s]:", Airport);
    if(InputString(tmpString, 6)==4)
        StrCopy(Airport, tmpString);
    printf("Your latitude [%s]:", latString);
    if(InputString(tmpString, 20)>0)
    {
        StrCopy(latString, tmpString);
        Latitude = atof(tmpString);
    }
    printf("Your longitude [%s]:", lonString);
    if(InputString(tmpString, 20)>0)
    {
        StrCopy(lonString, tmpString);
        Longitude = atof(tmpString);
    }
    printf("adsb_hub3 IP:port [%s]:", IPPort);
    if(InputString(tmpString, 80)>0)
        StrCopy(IPPort, tmpString);

    printf("Writing configuration to %s...", CfgName);

    // Save configuration
    sprintf(tmpString,"%s\r\n%s\r\n%s\r\n%s\r\n", 
        Airport, latString, lonString, IPPort);
    saveConfiguration();
    InputString(tmpString, 80);

    // Setting 512x212 16 colors
    Screen(7);

    // Initialize connection parameters
    initConnection();
    
    loadSprites();
    
    while(1)
    {
        SetColors(15,4,4);
        Cls();

        // TEST
        PutSprite(0,12,10,10,15);

        // Draw radar
        Line(256-210,106,256+210,106,15,0);
        Line(256,2,256,210,15,0);
        Circle(256,106,105,15,0);
        Circle(256,106,210,15,0);

        // Left top corner
        sprintf(tmpString,"Airport: %s", Airport);
        PutText(2,2,tmpString,0);
        PutText(2,12,latString,0);
        PutText(2,22,lonString,0);

        changeZoom();

        // Right bottom corner
        PutText(435,182,"L - List",0);
        PutText(435,192,"M - METAR",0);
        PutText(435,202,"Q - Quit",0);

        // Right top corner
        sprintf(tmpString,"%d.%d.%d.%d:%d", tcp_conn_parms.dest_ip[0], tcp_conn_parms.dest_ip[1], tcp_conn_parms.dest_ip[2], tcp_conn_parms.dest_ip[3], tcp_conn_parms.dest_port);
        PutText(365,2,tmpString,0);

        while(1)
        {
            unsigned char key = WaitKey();
        
            if(key=='l' || key=='L')
            {
                showList();
                break;
            }
            else
            if(key=='m' || key=='M')
            {
                showMetar();
                break;
            }
            else
            if(key>47 && key<58)
            {
                SelIndex = key-48;
                showSelected();
            }
            else
            if(key==30 && Zoom>5)
            {
                Zoom = Zoom / 2;
                changeZoom();
            }
            else
            if(key==31 && Zoom<160)
            {
                Zoom = Zoom * 2;
                changeZoom();
            }
            else
            if(key=='q' || key=='Q')
            {
                Screen(0);
                return 0;
            }
        }
    }
}
 