/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief Rainbow spectrum control using potentiometer on tricolor LED
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/
#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"

/**
 * Displays hue value (0.0 - 1.0) on rightmost 3 digits of 7-seg display
 * Format: "0.XX" using positions 2-0 (rightmost side)
 * Example: hue=0.75 shows "0.75" on right 3 digits
 * @param sseg_p  pointer to SsegCore instance
 * @param hue     value between 0.0 and 1.0
 */
void display_hue(SsegCore *sseg_p, double hue) {

   // turn off all segments
   for (int i = 0; i < 8; i++) {
      sseg_p->write_1ptn(0xff, i);
   }
   // turn off all decimal points
   sseg_p->set_dp(0x00);

   if (hue < 0.0) hue = 0.0;
   if (hue > 1.0) hue = 1.0;

   // Extract two fractional digits: e.g. 0.75 -> tens=7, ones=5
   int frac = (int)(hue * 100.0 + 0.5);  // round to nearest
   if (frac > 99) frac = 99;             // clamp
   int tens = frac / 10;
   int ones = frac % 10;

   // pos 2 = '0' with decimal point
   // pos 1 = tens fractional digit
   // pos 0 = ones fractional digit
   sseg_p->write_1ptn(sseg_p->h2s(0),    2);  // '0'
   sseg_p->write_1ptn(sseg_p->h2s(tens), 1);  // first fractional digit
   sseg_p->write_1ptn(sseg_p->h2s(ones), 0);  // second fractional digit

   // Decimal point on position 2 (the '0' digit)
   sseg_p->set_dp(0x04);  // bit 2 = position 2 decimal point
}

/**
 * Controls tricolor LED color based on potentiometer input
 * Pot range: 0-1V (ADC range 0-3.3V, so normalized accordingly)
 * Spectrum: Red -> Yellow -> Green -> Cyan -> Blue -> Magenta -> Red
 * @param pwm_p  pointer to pwm instance
 * @param adc_p  pointer to xadc instance
 * @param sseg_p pointer to sseg instance
 */
void rainbow_led_check(PwmCore *pwm_p, XadcCore *adc_p, SsegCore *sseg_p) {
   double pot, hue, r, g, b;

   // Read pot: 0.0 to ~0.303 (0-1V out of 3.3V range)
   // Normalize to 0.0 - 1.0 for full spectrum
   pot = adc_p->read_adc_in(0);
   hue = pot / (1.0);   // scale 0-1V range to 0.0-1.0
   if (hue > 1.0) hue = 1.0;  // clamp

   // Map hue (0.0-1.0) to RGB spectrum in 6 segments:
   // 0.000 - 0.166: Red -> Yellow  (green fades in,  red stays)
   // 0.166 - 0.332: Yellow -> Green (red fades out,  green stays)
   // 0.332 - 0.500: Green -> Cyan  (blue fades in,   green stays)
   // 0.500 - 0.664: Cyan -> Blue   (green fades out, blue stays)
   // 0.664 - 0.830: Blue -> Magenta (red fades in,   blue stays)
   // 0.830 - 1.000: Magenta -> Red  (blue fades out, red stays)
   if (hue < 0.166) {
      double t = hue / 0.166;
      r = 1.0;
      g = 0.0 + t;
      b = 0.0;
   } else if (hue < 0.332) {
      double t = (hue - 0.166) / 0.166;
      r = 1.0 - t;
      g = 1.0;
      b = 0.0;
   } else if (hue < 0.5) {
      double t = (hue - 0.332) / 0.166;
      r = 0.0;
      g = 1.0;
      b = 0.0 + t;
      if (b > 1.0) b = 1.0;
   } else if (hue < 0.664) {
      double t = (hue - 0.5) / 0.166;
      r = 0.0;
      g = 1.0 - t;
      b = 1.0;
   } else if (hue < 0.83) {
      double t = (hue - 0.664) / 0.166;
      r = 0.0 + t;
      g = 0.0;
      b = 1.0;
   } else {
      double t = (hue - 0.83) / 0.166;
      r = 1.0;
      g = 0.0;
      b = 1.0 - t;
      if (b < 0.0) b = 0.0;
   }

   pwm_p->set_duty(r, 2);   // Red   - channel 2
   pwm_p->set_duty(g, 1);   // Green - channel 1
   pwm_p->set_duty(b, 0);   // Blue  - channel 0

   // Display hue on 7-segment display
   display_hue(sseg_p, hue);

   // debug output
   uart.disp("hue/R/G/B: ");
   uart.disp(hue, 3); uart.disp(" / ");
   uart.disp(r, 3);   uart.disp(" / ");
   uart.disp(g, 3);   uart.disp(" / ");
   uart.disp(b, 3);   uart.disp("\n\r");

   sleep_ms(50);  // small delay to avoid flickering
}

// instantiate cores
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
DdfsCore ddfs(get_slot_addr(BRIDGE_BASE, S12_DDFS));
AdsrCore adsr(get_slot_addr(BRIDGE_BASE, S13_ADSR), &ddfs);

int main() {
   // set PWM frequency once before loop
   pwm.set_freq(50);
   while (1) {
      rainbow_led_check(&pwm, &adc, &sseg);
   } // while
} // main
