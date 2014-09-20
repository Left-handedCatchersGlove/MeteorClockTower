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
  int usec;
  time_t jt;
  struct tm *tmt;
  
  gettimeofday( &tv, NULL );       // 現在のマイクロ秒を得る
  usleep( tv.tv_usec % 1000000 );  // 次の秒まで待つ
 
  jt = time( NULL );
  tmt = localtime( &jt );          // 現在時刻を得る
  
  usleep( (60 - tmt->tm_sec) * 1000 * 1000 ); // 次の分まで待つ
}

int main( int argc, char **argv )
{
  bcm2835_init();
  bcm2835_gpio_fsel( PIN, BCM2835_GPIO_FSEL_OUTP );

  while(1) {
    wait_for_next_minute();
    time_t jt;
    struct tm *tmt;
    int t, i;
    int send_data[60];
    
    jt = time( NULL );
    tmt = localtime( &jt );
    
    // マーカー
    send_data[0] = -1;
    
    // 分(10の位)
    t = tmt->tm_min / 10;
    // ビット演算
    send_data[1] = t & 4 ? 1 : 0;
    send_data[2] = t & 2 ? 1 : 0;
    send_data[3] = t & 1 ? 1 : 0;
    send_data[4] = 0; // ここはいらないデータ？

    // 分(1の位)
    t = tmt->tm_min - t * 10;
    send_data[5] = t & 8 ? 1 : 0;
    send_data[6] = t & 4 ? 1 : 0;
    send_data[7] = t & 2 ? 1 : 0;
    send_data[8] = t & 1 ? 1 : 0;
    
    // マーカー
    send_data[9] = -1;
    send_data[10] = 0;
    send_data[11] = 0;

    // 時(10の位)
    t = tmt->tm_hour / 10;
    send_data[12] = t & 2 ? 1 : 0;
    send_data[13] = t & 1 ? 1 : 0;

    send_data[14] = 0;

    // 時(１の位)
    t = tmt->tm_hour - t * 10;
    send_data[15] = t & 8 ? 1 : 0;
    send_data[16] = t & 4 ? 1 : 0;
    send_data[17] = t & 2 ? 1 : 0;
    send_data[18] = t & 1 ? 1 : 0;
    
    // マーカー
    send_data[19] = -1;
    
    send_data[20] = 0;
    send_data[21] = 0;

    // 1月1日からの経過日(100の位)
    t = ( tmt->tm_yday + 1 ) / 100;
    send_data[22] = t & 2 ? 1 : 0;
    send_data[23] = t & 1 ? 1 : 0;

    send_data[24] = 0;

    // 1月1日からの経過日(10の位)
    t = ( tmt->tm_yday + 1 ) / 10 - t * 10;
    send_data[25] = t & 8 ? 1 : 0;
    send_data[26] = t & 4 ? 1 : 0;
    send_data[27] = t & 2 ? 1 : 0;
    send_data[28] = t & 1 ? 1 : 0;

    // マーカー
    send_data[29] = -1;

    // 1月1日からの経過日(1の位)
    t = ( tmt->tm_yday + 1 ) % 100 - t * 10;
    send_data[30] = t & 8 ? 1 : 0;
    send_data[31] = t & 4 ? 1 : 0;
    send_data[32] = t & 2 ? 1 : 0;
    send_data[33] = t & 1 ? 1 : 0;

    send_data[34] = 0;
    send_data[35] = 0;

    // 時刻のパリティー
    t = 0;
    for( i = 12; i < 19; i++ ) {
      t += send_data[i] == 1 ? 1 : 0;
    }
    send_data[36] = t & 1;
    
    // 分のパリティー
    t = 0;
    for( i = 1; i < 9; i++ ) {
      t += send_data[i] == 1 ? 1 : 0;
    }
    send_data[37] = t & 1 ? 1 : 0;
    send_data[38] = 0;
    
    // マーカー
    send_data[39] = -1;

    send_data[40] = 0;

    // 年の下2桁の10の位
    t = ( tmt->tm_year % 100 ) / 10;
    send_data[41] = t & 8 ? 1 : 0;
    send_data[42] = t & 4 ? 1 : 0;
    send_data[43] = t & 2 ? 1 : 0;
    send_data[44] = t & 1 ? 1 : 0;

    // 年の下2桁の1の位
    t = ( tmt->tm_year % 100 ) - t * 10;
    send_data[45] = t & 8 ? 1 : 0;
    send_data[46] = t & 4 ? 1 : 0;
    send_data[47] = t & 2 ? 1 : 0;
    send_data[48] = t & 1 ? 1 : 0;

    // マーカー
    send_data[49] = -1;

    // 曜日
    t = tmt->tm_wday;
    send_data[50] = t & 4 ? 1 : 0;
    send_data[51] = t & 2 ? 1 : 0;
    send_data[52] = t & 1 ? 1 : 0;

    send_data[53] = 0;
    send_data[54] = 0;

    send_data[55] = 0;
    send_data[56] = 0;
    send_data[57] = 0;
    send_data[58] = 0;

    // マーカー
    send_data[59] = -1;

    transmit_timecode( send_data );
  }
}
