/*****************************************************************//**
 * @file main_vanilla_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"


/**
 * blink once per second for 5 times.
 * provide a sanity check for timer (based on SYS_CLK_FREQ)
 * @param led_p pointer to led instance
 */
   void timer_check(GpoCore *led_p) {
   int i;

   for (i = 0; i < 5; i++) {
      led_p->write(0xffff);
      sleep_ms(500);
      led_p->write(0x0000);
      sleep_ms(500);
      debug("timer check - (loop #)/now: ", i, now_ms());
   }
}


int map(long x, long in_min, long in_max, long out_min, long out_max) {
  if ((x & 0x1FFC0) != 0) return -1; //bit mask that returns -1 if any switches larger than sw5 turn on
  else {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
}

/**
 * check individual led
 * @param led_p pointer to led instance
 * @param n number of led
 */
void led_check(GpiCore *sw_p, GpoCore *led_p, int n, long delay) {
   int i, s;
   long d1 = delay;
   s = sw_p->read() % 2; 
   
   //debug("Test", s, sw_p->read());
   
   for (i = 0; i < n; i++) {
      s = sw_p->read() % 2;
      if (s == 0) break;
      led_p->write(1, i);
      sleep_ms(d1);
      led_p->write(0, i);
      sleep_ms(d1);
      if (map(sw_p->read(),2,63,10,100) != d1) debug("Delay (ms) : %d", map(sw_p->read() , 2, 63, 10, 100), sw_p->read());
      d1 = map(sw_p->read(), 2, 63, 10, 100);
   }

   d1 = map(sw_p->read(), 2, 63, 10, 100);

   for (i = n; i > 0; i--) {
      s = sw_p->read() % 2;
      if (s == 0) break;
      led_p->write(1, i);
      sleep_ms(d1);
      led_p->write(0, i);
      sleep_ms(d1);
      if (map(sw_p->read(),2,63,10,100) != d1) debug("Delay (ms) : %d", map(sw_p->read() , 2, 63, 10, 100), sw_p->read());
      d1 = map(sw_p->read(), 2, 63, 10, 100);
   }
}

/**
 * leds flash according to switch positions.
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */
void sw_check(GpoCore *led_p, GpiCore *sw_p) {
   int i, s;

   s = sw_p->read();
   for (i = 0; i < 30; i++) {
      led_p->write(s);
      sleep_ms(50);
      led_p->write(0);
      sleep_ms(50);
   }
}

/**
 * uart transmits test line.
 * @note uart instance is declared as global variable in chu_io_basic.h
 */
void uart_check() {
   static int loop = 0;

   uart.disp("uart test #");
   uart.disp(loop);
   uart.disp("\n\r");
   loop++;
}

// instantiate switch, led
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));

int main() {

   while (1) {
      long delay = map(sw.read(), 2, 63, 10, 100);
      if (sw.read() % 2 != 0 && delay != -1) {
         led_check(&sw, &led, 16, delay);
         if (map(sw.read() , 2, 63, 10, 100) != delay) debug("Delay (ms) : %d", map(sw.read() , 2, 63, 10, 100), sw.read());
      }
   } //while
} //main

