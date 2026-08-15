/*****************************************************************//**
 * @file main_chasing_led_mouse.cpp
 *
 * @brief Chasing LED controlled by PS2 mouse
 *
 * Mouse controls:
 *   Left  button (rising edge) - toggle pause / resume
 *                                (also confirms speed if in adj mode)
 *   Right button (1st rising edge) - enter speed-adjust mode
 *                                    x movement changes period
 *   Right button (2nd rising edge) - confirm new speed and resume
 *
 * 7-Segment layout  (digit 7 = leftmost, digit 0 = rightmost):
 *
 *   [7]  [6]  [5][4][3]  [2][1][0]
 *    P.   __   S  P  :    x  x  x
 *    ^paused        ^dp   ^period ms
 *
 * NOTE on SsegCore::set_dp():
 *   The driver stores  dp = ~pt  (active-low internally).
 *   Therefore set_dp() must receive an ACTIVE-HIGH bitmask:
 *   bit 7 = digit 7 decimal point ... bit 0 = digit 0 decimal point.
 *********************************************************************/

#include "chu_init.h"
#include "gpio_cores.h"
#include "sseg_core.h"
#include "ps2_core.h"

// ── Hardware instances ──────────────────────────────────────────────────────
GpoCore  led(get_slot_addr(BRIDGE_BASE, S2_LED));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
Ps2Core  ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));

// ── Constants ───────────────────────────────────────────────────────────────
#define NUM_LEDS        16
#define SPEED_MIN       10       // ms (fastest allowed period)
#define SPEED_MAX       999      // ms (slowest  allowed period)
#define SPEED_DEFAULT   300      // ms
#define XMOV_SCALE      5        // accumulated x-units per 10 ms period change

// Active-low 7-segment patterns (same encoding as SsegCore::h2s / PTN_TABLE)
// Bit mapping: bit7=unused(1), bit6=g, bit5=f, bit4=e, bit3=d, bit2=c, bit1=b, bit0=a
// A bit value of 0 turns the segment ON; 1 turns it OFF.
//
//  'P'  segments ON: a b f g e  → bits: a=0 b=0 c=1 d=1 e=0 f=0 g=0
//       binary (bit7..0): 1_0_0_0_1_1_0_0 = 0x8C
//
//  'S'  segments ON: a f g c d  → same as h2s(5) = 0x92
//
//  blank (all off)             = 0xFF
#define SEG_BLANK  0xFF
#define SEG_P      0x8C
#define SEG_S      0x92   // identical to h2s(5)

// ── State ───────────────────────────────────────────────────────────────────
static bool paused    = false;
static bool speed_adj = false;   // true while in right-button speed-adjust mode
static int  led_pos   = 0;       // currently lit LED index (0 = rightmost)
static int  period_ms = SPEED_DEFAULT;
static int  x_accum   = 0;       // accumulated x movement during adj mode

// Track decimal-point state locally (SsegCore has no get_dp())
// Active-high bitmask passed to set_dp(): bit N controls digit N decimal point.
static uint8_t dp_state = 0x00;

// ── Helpers ─────────────────────────────────────────────────────────────────
static void dp_set_bit(int digit, bool on) {
   if (on) dp_state |=  (1 << digit);
   else    dp_state &= ~(1 << digit);
}

// ── Display update ──────────────────────────────────────────────────────────
void update_display() {
   // ── digit 7: 'P.' when paused or in speed-adjust mode, blank otherwise
   if (paused || speed_adj) {
      sseg.write_1ptn(SEG_P, 7);
      dp_set_bit(7, true);          // decimal point after 'P'
   } else {
      sseg.write_1ptn(SEG_BLANK, 7);
      dp_set_bit(7, false);
   }
   // ── digit 6: always blank
   sseg.write_1ptn(SEG_BLANK, 6);
   dp_set_bit(6, false);

   // ── digits 5, 4: 'S', 'P'
   sseg.write_1ptn(SEG_S, 5);
   dp_set_bit(5, false);

   sseg.write_1ptn(SEG_P, 4);
   dp_set_bit(4, false);

   // ── digit 3: blank segment body, decimal point only  →  looks like ':'
   sseg.write_1ptn(SEG_BLANK, 3);
   dp_set_bit(3, false);

   // ── digits 2, 1, 0: period value 000-999
   int spd = period_ms;
   if (spd < 0)   spd = 0;
   if (spd > 999) spd = 999;

   sseg.write_1ptn(sseg.h2s(spd / 100),       2);
   sseg.write_1ptn(sseg.h2s((spd / 10) % 10), 1);
   sseg.write_1ptn(sseg.h2s(spd % 10),        0);
   dp_set_bit(2, false);
   dp_set_bit(1, false);
   dp_set_bit(0, false);

   // Commit all decimal-point changes in one call
   sseg.set_dp(dp_state);
}

// ── LED update ──────────────────────────────────────────────────────────────
void update_leds() {
   led.write(1 << led_pos);
}

// ── Main ────────────────────────────────────────────────────────────────────
int main() {
   int lbtn, rbtn, xmov, ymov;
   int prev_lbtn = 0, prev_rbtn = 0;
   long last_step_ms;

   // ── Init PS2 mouse ────────────────────────────────────────────────────
   uart.disp("Initialising PS2 mouse...\n\r");
   int ps2_id = ps2.init();
   if (ps2_id != 2) {
      uart.disp("ERROR: PS2 mouse not detected (id=");
      uart.disp(ps2_id);
      uart.disp("). Halting.\n\r");
      while (1) {}
   }
   uart.disp("PS2 mouse ready.\n\r");

   // ── Init display ──────────────────────────────────────────────────────
   for (int i = 0; i < 8; i++) sseg.write_1ptn(SEG_BLANK, i);
   dp_state = 0x00;
   sseg.set_dp(dp_state);
   update_display();

   // ── Init LEDs ─────────────────────────────────────────────────────────
   update_leds();
   last_step_ms = now_ms();

   // ── Main loop ─────────────────────────────────────────────────────────
   while (1) {

      // ── Poll mouse ────────────────────────────────────────────────────
      if (ps2.get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov)) {

         bool lbtn_edge = ( lbtn && !prev_lbtn);
         bool rbtn_edge = ( rbtn && !prev_rbtn);

         // ── Left button (rising edge): pause / resume ──────────────
         if (lbtn_edge) {
            if (speed_adj) {
               // Pressing left while adjusting speed confirms and resumes
               speed_adj = false;
               paused    = false;
               x_accum   = 0;
               last_step_ms = now_ms();
               uart.disp("Speed confirmed (via left btn): ");
               uart.disp(period_ms);
               uart.disp(" ms — RUNNING\n\r");
            } else {
               paused = !paused;
               if (!paused) last_step_ms = now_ms();
               uart.disp(paused ? "PAUSED\n\r" : "RUNNING\n\r");
            }
            update_display();
         }

         // ── Right button (rising edge): enter / confirm speed-adj ──
         if (rbtn_edge) {
            if (!speed_adj) {
               // First press → enter speed-adjust mode
               speed_adj = true;
               paused    = true;
               x_accum   = 0;
               update_display();
               uart.disp("SPEED ADJUST MODE — move mouse left/right\n\r");
            } else {
               // Second press → confirm and resume
               speed_adj = false;
               paused    = false;
               x_accum   = 0;
               last_step_ms = now_ms();
               update_display();
               uart.disp("Speed confirmed: ");
               uart.disp(period_ms);
               uart.disp(" ms — RUNNING\n\r");
            }
         }

         // ── X movement during speed-adjust mode ───────────────────
         if (speed_adj && xmov != 0) {
            x_accum += xmov;

            // Every XMOV_SCALE accumulated units → ±10 ms change.
            // Moving right (xmov > 0) → faster → lower period.
            // Moving left  (xmov < 0) → slower → higher period.
            int steps = x_accum / XMOV_SCALE;
            if (steps != 0) {
               period_ms -= steps * 10;
               x_accum    = x_accum % XMOV_SCALE;

               if (period_ms < SPEED_MIN) period_ms = SPEED_MIN;
               if (period_ms > SPEED_MAX) period_ms = SPEED_MAX;

               update_display();
               uart.disp("period: ");
               uart.disp(period_ms);
               uart.disp(" ms\n\r");
            }
         }

         prev_lbtn = lbtn;
         prev_rbtn = rbtn;
      }

      // ── Advance chasing LED ───────────────────────────────────────────
      if (!paused && !speed_adj) {
         long now = now_ms();
         if ((now - last_step_ms) >= (long)period_ms) {
            led_pos = (led_pos + 1) % NUM_LEDS;
            update_leds();
            last_step_ms = now;
         }
      }

   } // while(1)

   return 0;
} // main
