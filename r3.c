//
// RealADSB client for MSX-DOS
// Based on Fusion-C library
//

#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/gr8net-tcpip.h"
#include "fusion-c/header/vdp_sprites.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

char NumberOfAirplanes = 0;

struct DD {
    char  XA[7]; // ICAO24
    long XB; // Latitude
    long XC; // Longitude
    unsigned int XD; // Altitude, ft
    char XE[9]; // Callsign
    int XF; // Heading
    int XG; // Speed, kts
    int XH; // Vertical speed, ft/min
    char XI[5]; // Aircraft type
    char XJ[10]; // Tail number
    int XK; // Squawk
    char XL[10]; // Owner or operator
    char XM[10]; // Distance, mi
};

__at 0xa000 struct DD d[10];

__at 0xa400 char Selected[7]="";
int SelIndex=-1;
int Zoom = 20; // Zoom, mi
float coeffX, coeffY;
long ZX, ZY; // Number of 1/10000000degs in 1 screen pixel

char CfgName[20] = "REALADSB.CFG"; // Name of configuration file
char Airport[6] = "KEWR"; // ICAO code of the airport
float Latitude = 40.6924798; // Latitude of the airport
float Longitude = -74.1686868; // Longitude of the airport
long LatitudeL = 406924798L; // Latitude of the airport (1/10000000deg)
long LongitudeL = -741686868L; // Longitude of the airport (1/10000000deg)
char IPPort[80] = "192.168.1.153:5567"; // IP:port of adsb_hub3

char tmpString[200];
char latString[15];
char lonString[15];

TIME tm;

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
0xF8,0xE0,0x80,0x80,0x80,0x80,0x80,0x00};
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

// Sprite colors
static const unsigned char whiteSprite[] = { 15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 };
static const unsigned char yellowSprite[] = { 10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10 };
static const unsigned char redSprite[] = { 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 };
 
long atol7(char *str)
{
   long result = 0L;
   char i;
   char afterDot=255;
 
   for(i=0; i<13; i++)
   {
       if(str[i]==0)
           break;
 
       if(i==0 && str[i]=='-')
       {
       }
       else
       if(str[i]=='.')
       {
           afterDot=0;
       }
       else
       {
           char n = str[i]-'0';
           result = result*10L+(long)n;
           if(afterDot<255)
           {
               afterDot++;
               if(afterDot==7)
                   break;
           }
       }
   }
   if(afterDot==255)
       afterDot = 0;
   for(i=afterDot; i<7; i++)
       result = result*10;
 
   if(str[0]=='-')
       return -result;
   else
       return result;
}

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

void initConnection(tcpip_unapi_tcp_conn_parms *tcp_conn_parms)
{
    int i, index=0, pos=0;

    implementation_count=tcpip_enumerate();
	if(implementation_count==0)
    {
        PrintString("Requires GR8NET cartridge for network access\r\n");
		return;
    }

	/* set currently active implementation */
	active_implementation=0;

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
                tcp_conn_parms->dest_ip[0]=(char)atoi(tmpString);
            else
            if(index==1)
                tcp_conn_parms->dest_ip[1]=(char)atoi(tmpString);
            else
            if(index==2)
                tcp_conn_parms->dest_ip[2]=(char)atoi(tmpString);
            else
            if(index==3)
                tcp_conn_parms->dest_ip[3]=(char)atoi(tmpString);
            index++;
            pos=0;
        }
    }
    if(pos>0)
    {
        tmpString[pos]=0;
        tcp_conn_parms->dest_port=atoi(tmpString);
    }

	tcp_conn_parms->local_port=-1;
    // Timeout 5 seconds
	tcp_conn_parms->user_timeout=-1;
	tcp_conn_parms->flags=0;
}

void loadSprites(void)
{
    int i;

    SpriteReset();

    Sprite16();
    SpriteSmall();

    // Sprites separated by 4 for 16x16 
    // which is combination of 4 8x8 sprites 
    SetSpritePattern(0,spriteUp,32);
    SetSpritePattern(4,spriteRight,32);
    SetSpritePattern(8,spriteDown,32);
    SetSpritePattern(12,spriteLeft,32);
    SetSpritePattern(16,sprite0,32);
    SetSpritePattern(20,sprite1,32);
    SetSpritePattern(24,sprite2,32);
    SetSpritePattern(28,sprite3,32);
    SetSpritePattern(32,sprite4,32);
    SetSpritePattern(36,sprite5,32);
    SetSpritePattern(40,sprite6,32);
    SetSpritePattern(44,sprite7,32);
    SetSpritePattern(48,sprite8,32);
    SetSpritePattern(52,sprite9,32);

    // Digits will be always yellow
    for(i=10;i<20;i++)
        SC8SpriteColors(i, yellowSprite);

    SpriteOn();
}

void showList(void)
{
    char i;
    // Hide all sprites
    for(i=0; i<23; i++)
        PutSprite(i,0,-32,-32,15);

    SetColors(10,1,4);
    BoxFill(20,20,492,192,1,0);

    PutText(30, 25, "Nearest airplanes", 0);
    PutText(30, 43, "Callsign  Type  Tail      Alt,ft     Speed,kts  Dist,mi", 0);

    for(i=0; i<NumberOfAirplanes; i++)
    {
        PutText(30,55+i*12,d[i].XE,0);
        PutText(110,55+i*12,d[i].XI,0);
        PutText(158,55+i*12,d[i].XJ,0);
        sprintf(tmpString,"%u/%d",d[i].XD,d[i].XH);
        PutText(230,55+i*12,tmpString,0);
        sprintf(tmpString,"%d",d[i].XG);
        PutText(342,55+i*12,tmpString,0);
        PutText(422,55+i*12,d[i].XM,0);
    }
    
    while(1)
    {
        unsigned char key = WaitKey();
        if(key==27)
            break;
    }
}

/*
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
}*/

void showNetworkMetar(tcpip_unapi_tcp_conn_parms *tcp_conn_parms)
{
    int a, i, j, k;
    char tcp_data[80];
    char response[1024];
 
    tcpip_unapi_tcp_conn_parms state_tcp_conn_parms;
    int conn_number;

    // 1 when HTTP content found after \r\n\r\n
    char content=0;
    char offset=0;
    char line=0;
    int pos;

    sprintf(tcp_data,"GET /metar?icao=%s HTTP/1.0\r\nAccept: \r\nConnection: close\r\n\r\n", Airport);
    //const char tcp_data[]={"GET /metar?icao=KEWR HTTP/1.0\r\nAccept: \r\nConnection: close\r\n\r\n"};
    //const	char	tcp_data[]={ "GET / HTTP/1.0\r\nUser-Agent: TCP/IP UNAPI test program\r\nAccept: ;q=0.8\r\nAccept-Language: en-us,en;q=0.5\r\nConnection: close\r\n\r\n" };
    a=tcpip_tcp_open(tcp_conn_parms, &conn_number);
	if(a==ERR_OK || a==ERR_CONN_EXISTS)
	{
        a = tcpip_tcp_state(conn_number, &state_tcp_conn_parms);
        if(state_tcp_conn_parms.send_free_bytes>=sizeof(tcp_data))
        {
            a=tcpip_tcp_send(conn_number,tcp_data,sizeof(tcp_data),0);
            for(k=0;k<100;k++)
            {
                j = tcpip_tcp_state(conn_number,&state_tcp_conn_parms);
                if(state_tcp_conn_parms.conn_state!=4 && state_tcp_conn_parms.incoming_bytes==0)
                {
                    //PrintString("\r\nTCP session finished");
                    break;
                }
                if(state_tcp_conn_parms.incoming_bytes!=0)
                {
                    a=tcpip_tcp_rcv(conn_number, &response[0], 1024, tcp_conn_parms);
                    if(a!=ERR_OK)
                    { 
                        sprintf(tmpString,"ERROR %d",a);
                        PutText(30,52,tmpString,0);
                        break;
                    }
                    for(pos=0;pos<tcp_conn_parms->incoming_bytes;pos++)
                    {
                        char ch=response[pos];
                        if(ch=='\r' || ch=='\n')
                        {
                            if(content==1 && offset>0)
                            {
                                tmpString[offset] = 0;
                                PutText(30,40+line*12,tmpString,0);
                                line++;
                                offset = 0;
                            }
                            if(pos>2 && content==0 && ch=='\n' && response[pos-2]=='\n')
                                content = 1;
                        }
                        else
                        if(content==1)
                        {
                            tmpString[offset] = ch;
                            offset++;
                            // Soft length 45, hard length 55
                            if((ch==' ' && offset>44) || offset==55)
                            {
                                tmpString[offset] = 0;
                                PutText(30,40+line*12,tmpString,0);
                                line++;
                                offset = 0;
                            }
                        }
                    }
                }
                for(i=0;i<500;i++);		// delay
            }
            if(offset>0)
            {
                tmpString[offset] = 0;
                PutText(30,40+line*12,tmpString,0);
            }
        }

		a=tcpip_tcp_close(conn_number);
	}
    else
    {
        sprintf(tmpString,"Can't connect: %d", a);
        PutText(30,40,tmpString,0);
    }
}

void showMetar(tcpip_unapi_tcp_conn_parms *tcp_conn_parms)
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
    showNetworkMetar(tcp_conn_parms);

    while(1)
    {
        unsigned char key = WaitKey();
        if(key==27)
            break;
    }
}

void showSelected()
{
    char a=0;
    
    if(SelIndex<0 || SelIndex>=NumberOfAirplanes)
        return;

    if(d[SelIndex].XF>44 && d[SelIndex].XF<136)
        a=4;
    if(d[SelIndex].XF>135 && d[SelIndex].XF<226)
        a=8;
    if(d[SelIndex].XF>225 && d[SelIndex].XF<316)
        a=12;
    PutSprite(SelIndex,a,120+(char)(d[SelIndex].XC/ZX),98-(char)(d[SelIndex].XB/ZY),10);
    SC8SpriteColors(SelIndex, redSprite);
    // Copy ICAO code of aircraft
    StrCopy(Selected, d[SelIndex].XA);
    //PutText(2,152,Selected,0);
    PutText(2,162,d[SelIndex].XE,0);
    PutText(2,172,d[SelIndex].XI,0);
    PutText(2,182,d[SelIndex].XJ,0);
    sprintf(tmpString,"%uft %dft/min    ", d[SelIndex].XD, d[SelIndex].XH);
    PutText(2,192,tmpString,0);
    sprintf(tmpString,"%dkts   ", d[SelIndex].XG);
    PutText(2,202,tmpString,0);
}

void loadTraffic(tcpip_unapi_tcp_conn_parms *tcp_conn_parms)
{
    int a, j, k;
    char tcp_data[120];
    char response[1024];

    tcpip_unapi_tcp_conn_parms state_tcp_conn_parms;
    int conn_number;

    char updateSelected = 0;

    // 1 when HTTP content found after \r\n\r\n
    char content=0;
    char offset=0;
    char item=0;
    int pos;
    NumberOfAirplanes=0;

    // Drop selection
    SelIndex=-1;

    GetTime(&tm);
    sprintf(tmpString,"%d:%s%d:%s%d*",tm.hour,tm.min<10?"0":"",tm.min,tm.sec<10?"0":"",tm.sec);
    PutText(405,12,tmpString,0);

    sprintf(tcp_data,"GET /a?lat=%s&lon=%s&num=10 HTTP/1.0\r\nAccept: */*\r\nConnection: close\r\n\r\n", latString, lonString);
    a=tcpip_tcp_open(tcp_conn_parms, &conn_number);
	if(a==ERR_OK || a==ERR_CONN_EXISTS)
	{
        a = tcpip_tcp_state(conn_number, &state_tcp_conn_parms);
        if(state_tcp_conn_parms.send_free_bytes>=sizeof(tcp_data))
        {
            a=tcpip_tcp_send(conn_number,tcp_data,sizeof(tcp_data),0);
            for(k=0;k<100;k++)
            {
                j = tcpip_tcp_state(conn_number,&state_tcp_conn_parms);
                if(state_tcp_conn_parms.conn_state!=4 && state_tcp_conn_parms.incoming_bytes==0)
                {
                    //PrintString("\r\nTCP session finished");
                    break;
                }
                if(state_tcp_conn_parms.incoming_bytes!=0)
                {
                    a=tcpip_tcp_rcv(conn_number, &response[0], 1024, tcp_conn_parms);
                    if(a!=ERR_OK)
                    { 
                        //sprintf(tmpString,"ERROR %d",a);
                        //PutText(30,52,tmpString,0);
                        break;
                    }
                    for(pos=0;pos<tcp_conn_parms->incoming_bytes;pos++)
                    {
                        char ch=response[pos];
                        if(ch=='\r' || ch=='\n')
                        {
                            if(content==1 && offset>0)
                            {
                                tmpString[offset]=0;
                                // Last parameter before end of line is distance
                                if(item==12)
                                    StrCopy(d[NumberOfAirplanes].XM, tmpString);
                                item=0;
                                offset = 0;
                                NumberOfAirplanes++;
                                // 10 is all we need
                                if(NumberOfAirplanes==10)
                                    break;
                            }
                            if(pos>2 && content==0 && ch=='\n' && response[pos-2]=='\n')
                                content = 1;
                        }
                        else
                        if(content==1)
                        {
                            if(ch=='|')
                            {
                                if(offset>0)
                                {
                                    tmpString[offset]=0;
                                    switch(item)
                                    {
                                    case 0:
                                        // Cleanup data structure before filling up
                                        // 69 is sizeof(DD)
                                        memset(&d[NumberOfAirplanes], 0, 69);
                                        StrCopy(d[NumberOfAirplanes].XA,tmpString);
                                        break;
                                    case 1:
                                        d[NumberOfAirplanes].XB=atol7(tmpString)-LatitudeL;
                                        break;
                                    case 2:
                                        d[NumberOfAirplanes].XC=atol7(tmpString)-LongitudeL;
                                        break;
                                    case 3:
                                        d[NumberOfAirplanes].XD=atoi(tmpString);
                                        break;
                                    case 4:
                                        StrCopy(d[NumberOfAirplanes].XE,tmpString);
                                        break;
                                    case 5:
                                        d[NumberOfAirplanes].XF=atoi(tmpString);
                                        break;
                                    case 6:
                                        d[NumberOfAirplanes].XG=atoi(tmpString);
                                        break;
                                    case 7:
                                        d[NumberOfAirplanes].XH=atoi(tmpString);
                                        break;
                                    case 8:
                                        StrCopy(d[NumberOfAirplanes].XI,tmpString);
                                        break;
                                    case 9:
                                        StrCopy(d[NumberOfAirplanes].XJ,tmpString);
                                        break;
                                    case 10:
                                        d[NumberOfAirplanes].XK=atoi(tmpString);
                                        break;
                                    case 11:
                                        if(strlen(tmpString)>8)
                                            strncpy(d[NumberOfAirplanes].XL,tmpString, 8);
                                        else
                                            StrCopy(d[NumberOfAirplanes].XL,tmpString);
                                        break;
                                    }
                                    offset=0;
                                }
                                item++;
                            }
                            else
                            {
                                tmpString[offset] = ch;
                                offset++;
                            }
                        }
                    }
                }
                //for(i=0;i<1000;i++);		// delay
            }
        }

		a=tcpip_tcp_close(conn_number);
	}
    else
    {
        sprintf(tmpString,"Can't connect: %d", a);
        PutText(30,40,tmpString,0);
    }

    for(pos=0;pos<NumberOfAirplanes;pos++)
    {
        char x,y;
        a=0;
        if(d[pos].XF>44 && d[pos].XF<136)
            a=4;
        else
        if(d[pos].XF>135 && d[pos].XF<226)
            a=8;
        else
        if(d[pos].XF>225 && d[pos].XF<316)
            a=12;
        x = 120+(char)(d[pos].XC/ZX);
        y = 98-(char)(d[pos].XB/ZY);
        // Digit
        PutSprite(10+pos,16+pos*4,x,y,10);
        // Airplane
        k = StrCompare(d[pos].XA,Selected);
        PutSprite(pos,a,x,y,k==0?10:15);
        SC8SpriteColors(pos,k==0?redSprite:whiteSprite);
        if(a==0)
            SelIndex = pos;
    }

    GetTime(&tm);
    sprintf(tmpString,"%d:%s%d:%s%d ",tm.hour,tm.min<10?"0":"",tm.min,tm.sec<10?"0":"",tm.sec);
    PutText(405,12,tmpString,0);

    showSelected();
}

void changeZoom()
{
    sprintf(tmpString,"%dmi ",Zoom);
    PutText(476,102,tmpString,0);
    ZX=(long)(coeffX*(float)Zoom);
    ZY=(long)(coeffY*(float)Zoom);
    return;
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
    FCB file;
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
 
       if(strlen(latString)>0)
       {
           Latitude = atof(latString);
           LatitudeL = atol7(latString);
       }
       if(strlen(lonString)>0)
       {
           Longitude = atof(lonString);
           LongitudeL = atol7(lonString);
       }
    }
}

void main(void) 
{
    tcpip_unapi_tcp_conn_parms tcp_conn_parms;

    // Static part of calculations: 1/10000000degs on 1 pixel with Zoom 1 mile
    coeffX=1e5/(69.172*cosf(Latitude*3.14159/180)*1.05); // 1815
    coeffY=1e5/(69*1.05); // 1380
 
    Screen(0);
    Width(80);
    PrintString("RealADSB 0.3 for MSX-DOS\r\n");
    PrintString("------------------------\r\n");
    
    // Load configuration
    ftoa(Latitude,7,latString);
    ftoa(Longitude,7,lonString);
    printf("Loading configuration from %s...\r\n", CfgName);
    loadConfiguration();

    // Initialize connection parameters
    initConnection(&tcp_conn_parms);

    // Setting 512x212 16 colors
    Screen(7);

    loadSprites();

    while(1)
    {
        SetColors(15,4,4);
        Cls();

        // Draw radar
        Line(256-210,106,256+210,106,15,0);
        Line(256,2,256,210,15,0);
        //Circle(256,106,105,15,0);
        //Circle(256,106,210,15,0);

        // Left top corner
        sprintf(tmpString,"Airport: %s", Airport);
        PutText(2,2,tmpString,0);
        PutText(2,12,latString,0);
        PutText(2,22,lonString,0);

        // Right bottom corner
        PutText(435,182,"L - List",0);
        PutText(435,192,"M - METAR",0);
        PutText(435,202,"Q - Quit",0);

        changeZoom();

        // Right top corner
        sprintf(tmpString,"%d.%d.%d.%d:%d", tcp_conn_parms.dest_ip[0], tcp_conn_parms.dest_ip[1], tcp_conn_parms.dest_ip[2], tcp_conn_parms.dest_ip[3], tcp_conn_parms.dest_port);
        PutText(365,2,tmpString,0);

        loadTraffic(&tcp_conn_parms);
        SetRealTimer(0);

        while(1)
        {
            unsigned char key = Inkey();
        
            if(key=='l' || key=='L')
            {
                showList();
                break;
            }
            else
            if(key=='m' || key=='M')
            {
                showMetar(&tcp_conn_parms);
                break;
            }
            else
            if(key>47 && key<58)
            {
                if(SelIndex>=0)
                    SC8SpriteColors(SelIndex, whiteSprite);
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
                Exit(0);
            }

            if(RealTimer()>1200)
            {
                SetRealTimer(0);
                loadTraffic(&tcp_conn_parms);
            }
        }
    }
}
 