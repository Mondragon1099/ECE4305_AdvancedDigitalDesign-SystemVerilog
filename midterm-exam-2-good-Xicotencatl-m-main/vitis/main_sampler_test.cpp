#include "chu_init.h"
#include "gpio_cores.h"
#include "i2c_core.h"
#include "cmps2_core.h"
#include "sseg_core.h"
#include <cstdlib>

I2cCore i2c_bus(get_slot_addr(BRIDGE_BASE, S14_CMPS2));
Cmps2Core compass(&i2c_bus);
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));

int get_uart_int() {
    char buf[16];
    int i = 0;
    while (true) {
        if (!uart.rx_fifo_empty()) {
            char c = uart.rx_byte();
            if (c == '\r' || c == '\n') {
                buf[i] = '\0';
                return atoi(buf);
            }
            if (i < 15) buf[i++] = c;
        }
    }
}
// show the live reading on the 7-segDisplay on the FPGA board
void display_degrees(int degrees) {
    // TODO Make sure the led pointers off at start
    for (int i = 0; i < 8; i++)
        sseg.write_1ptn(0xff, i);
    sseg.write_1ptn(sseg.h2s((degrees / 100) % 10), 2);
    sseg.write_1ptn(sseg.h2s((degrees /  10) % 10), 1);
    sseg.write_1ptn(sseg.h2s( degrees        % 10), 0);
}

// Flash a specific led depending on the direciton
void flash_cardinal_led(int degrees, bool flash_on) {
    int led_bit = -1;
    if (degrees >= 315 || degrees < 45)
        led_bit = 15;  // N
    else if (degrees < 135)
        led_bit = 3;   // E
    else if (degrees < 225)
        led_bit = 7;   // S
    else
        led_bit = 11;  // W
    uint32_t val = flash_on ? (1 << led_bit) : 0;
    led.write(val);
}

/**
 * Sets the tricolor LED based on cardinal direction:
 *   E  = green
 *   W  = red
 *   N/S and intercardinals = off
 */
void set_cardinal_rgb(int degrees) {
    double r = 0.0, g = 0.0, b = 0.0;

    if (degrees >= 45 && degrees < 135)       // E
        g = 1.0;
    else if (degrees >= 225 && degrees < 315) // W
        r = 1.0;

    pwm.set_duty(r, 2);  // Red   - channel 2
    pwm.set_duty(g, 1);  // Green - channel 1
    pwm.set_duty(b, 0);  // Blue  - channel 0
}

int main() {
    uart.disp("--- MANUAL OFFSET COMPASS ---\n\r");
    compass.init();
    pwm.set_freq(50);

    // 1. Magnetic mapping
    uart.disp("Rotate 360... Press any key to lock.\n\r");
    while (uart.rx_fifo_empty()) {
        compass.get_heading(11.202f, nullptr);
        sleep_ms(50);
    }
    uart.rx_byte();
    compass.lock_calibration();

    // 2. Manual Adjustment
    while (1) {
        bool err;
        int current = compass.get_heading(11.202f, &err);
        uart.disp("\rCurrent Heading: "); uart.disp(current);
        uart.disp(" deg. Press 's' to set offset or 'k' to keep.  ");
        display_degrees(current);
        if (!uart.rx_fifo_empty()) {
            char cmd = uart.rx_byte();
            if (cmd == 's') {
                uart.disp("\n\rEnter value to SUBTRACT: ");
                int val = get_uart_int();
                compass.set_manual_offset((float)val);
                uart.disp("\n\rOffset applied.\n\r");
                break;
            }
            if (cmd == 'k') break;
        }
        sleep_ms(200);
    }

    // 3. Live mode
    bool flash_state = false;
    int  flash_counter = 0;

    while (1) {
        bool err;
        int head          = compass.get_heading(11.202f, &err);
        const char* cardinal = compass.get_cardinal(head);

        display_degrees(head);
        set_cardinal_rgb(head);  // green=E, red=W, off otherwise

        uart.disp("\rHeading: "); uart.disp(head);
        uart.disp(" deg | ");     uart.disp(cardinal);
        uart.disp("          ");

        flash_counter++;
        if (flash_counter >= 2) {
            flash_state   = !flash_state;
            flash_counter = 0;
        }
        flash_cardinal_led(head, flash_state);

        sleep_ms(250);
    }
}
