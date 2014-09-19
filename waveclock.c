/**
 * @author : Katsuya Yamaguchi
 */
#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <signal.h>
#include <time.h>

#define PIN RPI_V2_GPIO_P1_07

/**
 * Generate pulse
 * @param ms : move clock time(ms)
 * @brief move clock
 */
void gen_pulse( int ms )
{
  int len = ms * 40;
  while( len-- ) {
    bcm2835_gpio_write( PIN, HIGH );
    bcm2835_delayMicroseconds( 12 ); // 12 micro sec
    bcm2835_gpio_write( PIN, LOW );
    bcm2835_delayMicroseconds( 12 ); // 12 micro sec
  }
}

void transmit_timecode( int *send_data )
{
  int i;
  for( i = 0; i < 60; i++ ) {
    switch( send_data[i] ) {
    case -1: // Marker
      gen_pulse( 200 );
      bcm2835_delayMicroseconds( 800 * 1000 ); // 800 msec
      break;

    case 1:
      gen_pulse( 500 );
      bcm2835_delayMicroseconds( 500 * 1000 ); // 500 msec
      break;

    case 0:
      gen_pulse( 800 );
      bcm2835_delayMicroseconds( 200 * 1000 ); // 200 msec
      break;
    }
  }
}

void wait_for_next_minute()
{
  struct timeval tv;

}
