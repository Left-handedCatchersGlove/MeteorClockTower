/**
 * @author : Katsuya Yamaguchi
 */
#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <signal.h>

#define PIN RPI_V2_GPIO_P1_07

int main( int argc, char **argv )
{
  if( !bcm2835_init() )
    return 1;

  bcm2835_gpio_fsel( PIN, BCM2835_GPIO_FSEL_OUTP );

  while( 1 ) {
    bcm2835_gpio_write( PIN, HIGH );
    bcm2835_delayMicroseconds(12);
    bcm2835_gpio_write( PIN, LOW );
    bcm2835_delayMicroseconds(12);
  }
}
