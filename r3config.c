#include "fusion-c/header/msx_fusion.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

char CfgName[20] = "REALADSB.CFG"; // Name of configuration file
char Airport[6] = "KEWR"; // ICAO code of the airport
float Latitude = 40.6924798; // Latitude of the airport
float Longitude = -74.1686868; // Longitude of the airport
char IPPort[80] = "192.168.1.153:5567"; // IP:port of adsb_hub3

char tmpString[256];
char latString[15];
char lonString[15];

FCB file;

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

void FT_SetName( FCB *p_fcb, const char *p_name ) 
{
  char i, j;
  
  memset( p_fcb, 0, sizeof(FCB) );
  
  // TODO: Change number to use another drive
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
    printf("OK\r\n");
}

void main(void) 
{
    Screen(0);
    Width(80);
    PrintString("RealADSB 0.3 Config for MSX-DOS\r\n");
    PrintString("-------------------------------\r\n");
    
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

    printf("\r\nPress any key...");

    InputString(tmpString, 80);
}
