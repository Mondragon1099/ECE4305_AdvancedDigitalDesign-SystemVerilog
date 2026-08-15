/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief Lab8: Potentiometer-controlled chasing LEDs
 *
 * @author Daniel/Sean
 * @version v1.0: initial release
 *********************************************************************/
#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"

// instantiate led, adc, sseg
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

long pot_check(XadcCore *adc_p) {
   double reading = adc_p->read_adc_in(0);
   double scaled = reading / (1.0 / 3.3);
   return (long)(scaled * 90.0 + 10.0);
}

void led_check(GpoCore *led_p, XadcCore *adc_p, int n) {
   int i;
   long delay;
   for (i = 0; i < n; i++) {
      delay = pot_check(adc_p);
      led_p->write(1, i);
      sleep_ms(delay);
      led_p->write(0, i);
      sleep_ms(delay);
   }
   for (i = n; i > 0; i--) {
      delay = pot_check(adc_p);
      led_p->write(1, i);
      sleep_ms(delay);
      led_p->write(0, i);
      sleep_ms(delay);
   }
}

int main() {

   while (1) {
   double reading = adc.read_adc_in(0);  // 0.0 to 1.0      
   uart.disp("pot voltage: ");
   uart.disp(reading, 2);
   uart.disp("\n\r");
   led_check(&led, &adc, 16);
   } 
   
} // main
