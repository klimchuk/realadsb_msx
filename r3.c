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
#include <stdlib.h>
#include <float.h>
#include <math.h>

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
int SelIndex;
int Zoom = 20; // Zoom, mi
float ZX, ZY;

char CfgName[80] = "B:REALADSB.CFG"; // Name of configuration file
char Airport[6] = "KEWR"; // ICAO code of the airport
float Latitude = 40.6924798; // Latitude of the airport
float Longitude = -74.1686868; // Longitude of the airport
char IPPort[80] = "192.168.1.153:5567"; // IP:port of adsb_hub3

char tmpString[80];


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

void loadSprites()
{
    Sprite16();

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

void changeZoom(void)
{
    sprintf(tmpString,"R: %dmi  ",Zoom);
    PutText(435,172,tmpString,0);
    ZX=69.172*cosf(Latitude*3.14159/180)*1.05/(float)Zoom;
    ZY=69*1.05/(float)Zoom;
    return;
}

void loadMetar(void)
{
    tcpip_unapi_tcp_conn_parms tcp_conn_parms;
    tcpip_unapi_tcp_conn_parms state_tcp_conn_parms;
    int a, i;
    int conn_number;
    char response[1024];
    const	char	tcp_data[]={ "GET / HTTP/1.0\r\nUser-Agent: TCP/IP UNAPI test program\r\nAccept: */*;q=0.8\r\nAccept-Language: en-us,en;q=0.5\r\nConnection: close\r\n\r\n" };

    tcp_conn_parms.dest_ip[0]=192;
	tcp_conn_parms.dest_ip[1]=168;
	tcp_conn_parms.dest_ip[2]=1;
	tcp_conn_parms.dest_ip[3]=53;
	tcp_conn_parms.dest_port=5567;
	tcp_conn_parms.local_port=-1;
	tcp_conn_parms.user_timeout=-1;
	tcp_conn_parms.flags=0;
	a=tcpip_tcp_open(&tcp_conn_parms, &conn_number);
	if(a==ERR_OK)
	{
		a=tcpip_tcp_state(conn_number, &state_tcp_conn_parms);
		if(a==ERR_OK)
		{
			if(state_tcp_conn_parms.send_free_bytes>=sizeof(tcp_data))
			{
				a=tcpip_tcp_send(conn_number,tcp_data,sizeof(tcp_data),0);
				while((i=tcpip_tcp_state(conn_number,&state_tcp_conn_parms))==ERR_OK)
				{
					if((state_tcp_conn_parms.conn_state!=4) && state_tcp_conn_parms.incoming_bytes==0)
					{
						PrintString("\r\nTCP session finished");
						break;
					}
					if(state_tcp_conn_parms.incoming_bytes!=0)
					{
						a=tcpip_tcp_rcv(conn_number, &response[0], 1024, &tcp_conn_parms);
						if(a!=ERR_OK) 
                            break;
					}
				}
			}
			else PrintString("No enough space in TX buffer");
		}
		a=tcpip_tcp_close(conn_number);
	}
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

int main(void) 
{
    char latString[15];
    char lonString[15];
    
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
    ftoa(Latitude,7,latString);
    printf("Your latitude [%s]:", latString);
    if(InputString(tmpString, 20)>0)
    {
        StrCopy(latString, tmpString);
        Latitude = atof(tmpString);
    }
    ftoa(Longitude,7,lonString);
    printf("Your longitude [%s]:", lonString);
    if(InputString(tmpString, 20)>0)
    {
        StrCopy(lonString, tmpString);
        Longitude = atof(tmpString);
    }
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
    PutText(2,2,Airport,0);
    PutText(2,12,latString,0);
    PutText(2,22,lonString,0);

    changeZoom();

    // Right bottom corner
    PutText(435,182,"L - List",0);
    PutText(435,192,"M - METAR",0);
    PutText(435,202,"Q - Quit",0);

    // Right top corner
    PutText(365,2,IPPort,0);

    while(1)
    {
        unsigned char key = WaitKey();
    
        if(key=='l' || key=='L')
            showList();
        else
        if(key=='m' || key=='M')
            showMetar();
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
 