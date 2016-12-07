#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>   
#include <limits.h> 

#define MAX_TIMINGS 85

int DHT_PIN;

int data[5] = { 0, 0, 0, 0, 0 };

void read_dht_data( )
{
    printf ( "Read GPIO.%i ", DHT_PIN );

    uint8_t laststate    = HIGH;
    uint8_t counter        = 0;
    uint8_t j            = 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode( DHT_PIN, OUTPUT );
    digitalWrite( DHT_PIN, LOW );
    delay( 18 );

    /* prepare to read the pin */
    pinMode( DHT_PIN, INPUT );

    /* detect change and read data */
    for ( i = 0; i < MAX_TIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHT_PIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHT_PIN );

        if ( counter == 255 )
            break;

        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            data[j / 8] <<= 1;
            if ( counter > 16 )
                data[j / 8] |= 1;
            j++;
        }
    }

    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */

    if ( (j >= 40) &&
         (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
    {
        float h = (float)((data[0] << 8) + data[1]) / 10;
        if ( h > 100 )
        {
            h = data[0];    // for DHT11
        }
        float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if ( c > 125 )
        {
            c = data[2];    // for DHT11
        }
        if ( data[2] & 0x80 )
        {
            c = -c;
        }
        printf( "Humidity: %.1f Temperature: %.1f\n", h, c);
    }
    else  {
        printf ( "Wrong data from GPIO.%i\n", DHT_PIN );
    }
}

int main( int argc, char *argv[] )
{

    char *p;

    errno = 0;
    long conv = strtol(argv[1], &p, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than int
    if (errno != 0 || *p != '\0' || conv > INT_MAX) {
	printf ( "Invalid GPIO pin '%s'! \nAvailable pins: 7 12 16 18 21 22 23 24 25 26\n", argv[1] );
	exit( 1 );
    } else {
	// No error
	DHT_PIN = conv;
    }

    if ( wiringPiSetup() == -1 )
        exit( 1 );

    for (int i = 0; i < 3; i++)
    {
        read_dht_data( DHT_PIN );
        delay( 2500 ); /* wait 2.5 seconds before next read */
    }
    return(0);
}