/*****************************************************************//**
 * @file main_video_test.cpp
 *********************************************************************/

#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "i2c_core.h"
#include "cmps2_core.h"
#include "ps2_core.h"
#include <cstdlib>
#include <cmath>

// ── Color palette (9-bit RGB) ───────────────────────────────────────
#define COL_BLACK       0x000
#define COL_WHITE       0x1ff
#define COL_DARK_GRAY   0x049
#define COL_LIGHT_GRAY  0x0db
#define COL_RED         0x1c0
#define COL_YELLOW      0x1f8
#define COL_GREEN       0x007
#define COL_BLUE        0x038
#define COL_CYAN        0x03f
#define COL_BG          0x001

// ── Compass geometry ────────────────────────────────────────────────
#define CX          320
#define CY          210
#define R_OUTER     160
#define R_INNER     148
#define R_NEEDLE    135
#define R_HUB         8
#define R_LABEL     175

#ifndef M_PI
#define M_PI 3.14159265f
#endif

// ── Hardware ────────────────────────────────────────────────────────
I2cCore   i2c_bus(get_slot_addr(BRIDGE_BASE, S14_CMPS2));
Cmps2Core compass(&i2c_bus);
SsegCore  sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
GpoCore   led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore   sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
OsdCore   osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
Ps2Core   ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));

// ────────────────────────────────────────────────────────────────────
// Math helpers
// ────────────────────────────────────────────────────────────────────
static inline int   iround(float f)  { return (int)(f + 0.5f); }
static inline float deg2rad(float d) { return d * M_PI / 180.0f; }

// ────────────────────────────────────────────────────────────────────
// PS2 mouse helpers
// button_id: 1 = left click, 2 = right click
// ────────────────────────────────────────────────────────────────────
static int prev_lbtn = 0;
static int prev_rbtn = 0;

bool mouse_get_click_edge(int *button_id) {
    int lbtn, rbtn, xmov, ymov;

    if (ps2.get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov)) {
        bool left_edge  = ( lbtn && !prev_lbtn);
        bool right_edge = ( rbtn && !prev_rbtn);

        prev_lbtn = lbtn;
        prev_rbtn = rbtn;

        if (left_edge) {
            *button_id = 1;
            return true;
        }

        if (right_edge) {
            *button_id = 2;
            return true;
        }
    }

    return false;
}

// ────────────────────────────────────────────────────────────────────
// Frame text helpers, 5x7 block font
// Draws text directly into the frame buffer
// ────────────────────────────────────────────────────────────────────
void frame_fill_rect(FrameCore *f, int x, int y, int w, int h, int color) {
    for (int yy = 0; yy < h; yy++) {
        f->plot_line(x, y + yy, x + w - 1, y + yy, color);
    }
}

void frame_draw_char(FrameCore *f, int x, int y, char ch, int color, int scale) {
    if (ch >= 'a' && ch <= 'z') {
        ch = ch - 'a' + 'A';
    }

    const char *rows = nullptr;

    switch (ch) {
        case 'A': rows = "01110""10001""10001""11111""10001""10001""10001"; break;
        case 'B': rows = "11110""10001""10001""11110""10001""10001""11110"; break;
        case 'C': rows = "01111""10000""10000""10000""10000""10000""01111"; break;
        case 'D': rows = "11110""10001""10001""10001""10001""10001""11110"; break;
        case 'E': rows = "11111""10000""10000""11110""10000""10000""11111"; break;
        case 'F': rows = "11111""10000""10000""11110""10000""10000""10000"; break;
        case 'G': rows = "01111""10000""10000""10111""10001""10001""01111"; break;
        case 'H': rows = "10001""10001""10001""11111""10001""10001""10001"; break;
        case 'I': rows = "11111""00100""00100""00100""00100""00100""11111"; break;
        case 'J': rows = "00111""00010""00010""00010""00010""10010""01100"; break;
        case 'K': rows = "10001""10010""10100""11000""10100""10010""10001"; break;
        case 'L': rows = "10000""10000""10000""10000""10000""10000""11111"; break;
        case 'M': rows = "10001""11011""10101""10101""10001""10001""10001"; break;
        case 'N': rows = "10001""11001""10101""10011""10001""10001""10001"; break;
        case 'O': rows = "01110""10001""10001""10001""10001""10001""01110"; break;
        case 'P': rows = "11110""10001""10001""11110""10000""10000""10000"; break;
        case 'Q': rows = "01110""10001""10001""10001""10101""10010""01101"; break;
        case 'R': rows = "11110""10001""10001""11110""10100""10010""10001"; break;
        case 'S': rows = "01111""10000""10000""01110""00001""00001""11110"; break;
        case 'T': rows = "11111""00100""00100""00100""00100""00100""00100"; break;
        case 'U': rows = "10001""10001""10001""10001""10001""10001""01110"; break;
        case 'V': rows = "10001""10001""10001""10001""10001""01010""00100"; break;
        case 'W': rows = "10001""10001""10001""10101""10101""10101""01010"; break;
        case 'X': rows = "10001""10001""01010""00100""01010""10001""10001"; break;
        case 'Y': rows = "10001""10001""01010""00100""00100""00100""00100"; break;
        case 'Z': rows = "11111""00001""00010""00100""01000""10000""11111"; break;

        case '0': rows = "01110""10001""10011""10101""11001""10001""01110"; break;
        case '1': rows = "00100""01100""00100""00100""00100""00100""01110"; break;
        case '2': rows = "01110""10001""00001""00010""00100""01000""11111"; break;
        case '3': rows = "11110""00001""00001""01110""00001""00001""11110"; break;
        case '4': rows = "00010""00110""01010""10010""11111""00010""00010"; break;
        case '5': rows = "11111""10000""10000""11110""00001""00001""11110"; break;
        case '6': rows = "01110""10000""10000""11110""10001""10001""01110"; break;
        case '7': rows = "11111""00001""00010""00100""01000""01000""01000"; break;
        case '8': rows = "01110""10001""10001""01110""10001""10001""01110"; break;
        case '9': rows = "01110""10001""10001""01111""00001""00001""01110"; break;

        case '.': rows = "00000""00000""00000""00000""00000""01100""01100"; break;
        case ':': rows = "00000""01100""01100""00000""01100""01100""00000"; break;
        case '-': rows = "00000""00000""00000""11111""00000""00000""00000"; break;
        case '[': rows = "01110""01000""01000""01000""01000""01000""01110"; break;
        case ']': rows = "01110""00010""00010""00010""00010""00010""01110"; break;

        case ' ':
        default:
            return;
    }

    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (rows[row * 5 + col] == '1') {
                frame_fill_rect(f,
                                x + col * scale,
                                y + row * scale,
                                scale,
                                scale,
                                color);
            }
        }
    }
}

void frame_print(FrameCore *f, int x, int y, const char *msg, int color, int scale) {
    int start_x = x;

    while (*msg) {
        if (*msg == '\n') {
            y += 9 * scale;
            x = start_x;
        } else {
            frame_draw_char(f, x, y, *msg, color, scale);
            x += 6 * scale;
        }

        msg++;
    }
}

void frame_print_int(FrameCore *f, int x, int y, int val, int color, int scale) {
    char buf[16];
    int i = 0;

    if (val == 0) {
        frame_draw_char(f, x, y, '0', color, scale);
        return;
    }

    if (val < 0) {
        frame_draw_char(f, x, y, '-', color, scale);
        x += 6 * scale;
        val = -val;
    }

    while (val > 0 && i < 15) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    while (i > 0) {
        frame_draw_char(f, x, y, buf[--i], color, scale);
        x += 6 * scale;
    }
}

// ────────────────────────────────────────────────────────────────────
// Draw filled circle
// ────────────────────────────────────────────────────────────────────
void draw_circle_filled(FrameCore *f, int cx, int cy, int r, int color) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = iround(sqrtf((float)(r * r - dy * dy)));
        f->plot_line(cx - dx, cy + dy, cx + dx, cy + dy, color);
    }
}

// ────────────────────────────────────────────────────────────────────
// Draw ring
// ────────────────────────────────────────────────────────────────────
void draw_ring(FrameCore *f, int cx, int cy,
               int r_out, int r_in, int color_ring, int color_fill) {
    draw_circle_filled(f, cx, cy, r_out, color_ring);
    draw_circle_filled(f, cx, cy, r_in,  color_fill);
}

// ────────────────────────────────────────────────────────────────────
// Draw one tick mark
// ────────────────────────────────────────────────────────────────────
void draw_tick(FrameCore *f, int cx, int cy,
               float angle_deg, int r_outer, int r_inner, int color) {
    float rad = deg2rad(angle_deg - 90.0f);

    int x0 = cx + iround(r_outer * cosf(rad));
    int y0 = cy + iround(r_outer * sinf(rad));

    int x1 = cx + iround(r_inner * cosf(rad));
    int y1 = cy + iround(r_inner * sinf(rad));

    f->plot_line(x0, y0, x1, y1, color);
}

// ────────────────────────────────────────────────────────────────────
// Redraw all tick marks
// ────────────────────────────────────────────────────────────────────
void redraw_ticks(FrameCore *f) {
    for (int deg = 0; deg < 360; deg += 5) {
        bool is_cardinal      = (deg % 90 == 0);
        bool is_intercardinal = (deg % 45 == 0) && !is_cardinal;

        int r_tick_out = R_OUTER - 5;
        int r_tick_in, col;

        if (is_cardinal) {
            r_tick_in = R_INNER + 8;
            col = COL_GREEN;
        } else if (is_intercardinal) {
            r_tick_in = R_INNER + 16;
            col = COL_BLUE;
        } else {
            r_tick_in = R_INNER + 22;
            col = COL_LIGHT_GRAY;
        }

        draw_tick(f, CX, CY, (float)deg, r_tick_out, r_tick_in, col);
    }
}

// ────────────────────────────────────────────────────────────────────
// Draw needle — yellow = north tip, red = south tail
// ────────────────────────────────────────────────────────────────────
void draw_needle(FrameCore *f, int cx, int cy, int heading) {
    float rad  = deg2rad((float)heading - 90.0f);
    float perp = rad + M_PI / 2.0f;

    int tx = cx + iround(R_NEEDLE       * cosf(rad));
    int ty = cy + iround(R_NEEDLE       * sinf(rad));

    int bx = cx - iround((R_NEEDLE / 2) * cosf(rad));
    int by = cy - iround((R_NEEDLE / 2) * sinf(rad));

    for (int w = -2; w <= 2; w++) {
        int ox = iround(w * cosf(perp));
        int oy = iround(w * sinf(perp));

        f->plot_line(cx + ox, cy + oy, tx + ox, ty + oy, COL_YELLOW);
        f->plot_line(bx + ox, by + oy, cx + ox, cy + oy, COL_RED);
    }
}

// ────────────────────────────────────────────────────────────────────
// Draw digit using filled segment rectangles
// ────────────────────────────────────────────────────────────────────
void draw_digit(FrameCore *f, int x, int y, int digit, int color, int scale = 4) {
    int w  = 6 * scale;
    int h  = 10 * scale;
    int h2 = h / 2;
    int th = scale;

    bool seg[10][7] = {
        {1, 1, 1, 0, 1, 1, 1},  // 0
        {0, 0, 1, 0, 0, 1, 0},  // 1
        {1, 0, 1, 1, 1, 0, 1},  // 2
        {1, 0, 1, 1, 0, 1, 1},  // 3
        {0, 1, 1, 1, 0, 1, 0},  // 4
        {1, 1, 0, 1, 0, 1, 1},  // 5
        {1, 1, 0, 1, 1, 1, 1},  // 6
        {1, 0, 1, 0, 0, 1, 0},  // 7
        {1, 1, 1, 1, 1, 1, 1},  // 8
        {1, 1, 1, 1, 0, 1, 1},  // 9
    };

    if (digit < 0 || digit > 9) return;

    for (int t = 0; t < th; t++) {
        if (seg[digit][0])
            f->plot_line(x + th,     y + t,       x + w - th, y + t,       color);
        if (seg[digit][1])
            f->plot_line(x + t,      y + th,      x + t,      y + h2 - th, color);
        if (seg[digit][2])
            f->plot_line(x + w - t,  y + th,      x + w - t,  y + h2 - th, color);
        if (seg[digit][3])
            f->plot_line(x + th,     y + h2 + t,  x + w - th, y + h2 + t,  color);
        if (seg[digit][4])
            f->plot_line(x + t,      y + h2 + th, x + t,      y + h - th,  color);
        if (seg[digit][5])
            f->plot_line(x + w - t,  y + h2 + th, x + w - t,  y + h - th,  color);
        if (seg[digit][6])
            f->plot_line(x + th,     y + h - t,   x + w - th, y + h - t,   color);
    }
}

// ────────────────────────────────────────────────────────────────────
// Draw 3-digit number
// ────────────────────────────────────────────────────────────────────
void draw_number(FrameCore *f, int x, int y, int val, int color, int scale = 4) {
    int spacing = 6 * scale + 6;

    draw_digit(f, x,             y, (val / 100) % 10, color, scale);
    draw_digit(f, x + spacing,   y, (val /  10) % 10, color, scale);
    draw_digit(f, x + spacing*2, y,  val        % 10, color, scale);
}

// ────────────────────────────────────────────────────────────────────
// Cardinal letter drawing functions
// ────────────────────────────────────────────────────────────────────
void draw_letter_N(FrameCore *f, int cx, int cy, int color) {
    int s = 7;

    for (int t = -1; t <= 1; t++) {
        f->plot_line(cx - s + t, cy - s, cx - s + t, cy + s, color);
        f->plot_line(cx + s + t, cy - s, cx + s + t, cy + s, color);
    }

    f->plot_line(cx - s,     cy - s, cx + s,     cy + s, color);
    f->plot_line(cx - s + 1, cy - s, cx + s + 1, cy + s, color);
}

void draw_letter_E(FrameCore *f, int cx, int cy, int color) {
    int s = 7;

    for (int t = -1; t <= 1; t++) {
        f->plot_line(cx - s,     cy - s + t, cx + s,   cy - s + t, color);
        f->plot_line(cx - s,     cy     + t, cx + s-2, cy     + t, color);
        f->plot_line(cx - s,     cy + s + t, cx + s,   cy + s + t, color);
        f->plot_line(cx - s + t, cy - s,     cx - s+t, cy + s,     color);
    }
}

void draw_letter_S(FrameCore *f, int cx, int cy, int color) {
    int s  = 7;
    int t2 = 7;

    // Top, middle, bottom
    for (int t = -1; t <= 1; t++) {
        f->plot_line(cx - s, cy - t2 + t, cx + s, cy - t2 + t, color);
        f->plot_line(cx - s, cy      + t, cx + s, cy      + t, color);
        f->plot_line(cx - s, cy + t2 + t, cx + s, cy + t2 + t, color);
    }

    for (int t = -1; t <= 1; t++) {
        f->plot_line(cx - s + t, cy - t2, cx - s + t, cy,      color);
        f->plot_line(cx + s + t, cy,      cx + s + t, cy + t2, color);
    }
}

void draw_letter_W(FrameCore *f, int cx, int cy, int color) {
    int s  = 8;
    int t2 = 7;

    for (int ox = -1; ox <= 1; ox++) {
        for (int oy = -1; oy <= 1; oy++) {
            f->plot_line(cx - s   + ox, cy - t2 + oy,
                         cx - s/2 + ox, cy + t2 + oy, color);

            f->plot_line(cx - s/2 + ox, cy + t2 + oy,
                         cx       + ox, cy      + oy, color);

            f->plot_line(cx       + ox, cy      + oy,
                         cx + s/2 + ox, cy + t2 + oy, color);

            f->plot_line(cx + s/2 + ox, cy + t2 + oy,
                         cx + s   + ox, cy - t2 + oy, color);
        }
    }
}

// ────────────────────────────────────────────────────────────────────
// Place N / E / S / W around the compass ring
// ────────────────────────────────────────────────────────────────────
void draw_cardinal_letters(FrameCore *f, int cx, int cy, int r) {
    draw_letter_N(f, cx,     cy - r, COL_WHITE);
    draw_letter_E(f, cx + r, cy,     COL_CYAN);
    draw_letter_S(f, cx,     cy + r, COL_WHITE);
    draw_letter_W(f, cx - r, cy,     COL_CYAN);
}

// ────────────────────────────────────────────────────────────────────
// Draw the static compass rose
// ────────────────────────────────────────────────────────────────────
void draw_compass_rose(FrameCore *f) {
    f->clr_screen(COL_BG);

    draw_ring(f, CX, CY, R_OUTER, R_OUTER - 4, COL_WHITE, COL_BG);
    draw_circle_filled(f, CX, CY, R_INNER, COL_DARK_GRAY);

    redraw_ticks(f);
    draw_cardinal_letters(f, CX, CY, R_LABEL);

    draw_circle_filled(f, CX, CY, R_HUB, COL_WHITE);
}

// ────────────────────────────────────────────────────────────────────
// Info panel
// ────────────────────────────────────────────────────────────────────
void erase_info_panel(FrameCore *f) {
    // Start below the south S, which reaches down to about y = 393.
    for (int y = 394; y < 455; y++) {
        f->plot_line(20, y, 620, y, COL_BG);
    }
}

void draw_info_panel(FrameCore *f, int heading, const char *cardinal) {
    erase_info_panel(f);

    int lx = 40;
    int ly = 405;

    int sc  = 2;
    int sw2 = 5 * sc;
    int sh2 = 7 * sc;

    // H
    f->plot_line(lx,         ly,        lx,         ly + sh2,  COL_CYAN);
    f->plot_line(lx + 1,     ly,        lx + 1,     ly + sh2,  COL_CYAN);
    f->plot_line(lx,         ly+sh2/2,  lx + sw2,   ly+sh2/2,  COL_CYAN);
    f->plot_line(lx + sw2,   ly,        lx + sw2,   ly + sh2,  COL_CYAN);
    f->plot_line(lx + sw2+1, ly,        lx + sw2+1, ly + sh2,  COL_CYAN);
    lx += sw2 + 6;

    // D
    f->plot_line(lx,         ly,        lx,          ly + sh2,    COL_CYAN);
    f->plot_line(lx + 1,     ly,        lx + 1,      ly + sh2,    COL_CYAN);
    f->plot_line(lx,         ly,        lx + sw2-2,  ly,          COL_CYAN);
    f->plot_line(lx,         ly + sh2,  lx + sw2-2,  ly + sh2,    COL_CYAN);
    f->plot_line(lx + sw2,   ly + 2,    lx + sw2,    ly + sh2-2,  COL_CYAN);
    lx += sw2 + 6;

    // G
    f->plot_line(lx,         ly,        lx + sw2,    ly,          COL_CYAN);
    f->plot_line(lx,         ly,        lx,          ly + sh2,    COL_CYAN);
    f->plot_line(lx + 1,     ly,        lx + 1,      ly + sh2,    COL_CYAN);
    f->plot_line(lx,         ly + sh2,  lx + sw2,    ly + sh2,    COL_CYAN);
    f->plot_line(lx + sw2,   ly+sh2/2,  lx + sw2,    ly + sh2,    COL_CYAN);
    f->plot_line(lx+sw2-2,   ly+sh2/2,  lx + sw2,    ly+sh2/2,    COL_CYAN);
    lx += sw2 + 6;

    // Colon
    f->plot_line(lx, ly + 2,       lx + 1, ly + 2,       COL_CYAN);
    f->plot_line(lx, ly + sh2 - 2, lx + 1, ly + sh2 - 2, COL_CYAN);
    lx += 10;

    // Heading number
    draw_number(f, lx, ly - 4, heading, COL_YELLOW, 4);

    int dig_spacing = 6 * 4 + 6;
    int dig_width   = 6 * 4;
    lx += dig_spacing * 2 + dig_width + 10;

    // Degree symbol
    draw_circle_filled(f, lx, ly, 3, COL_YELLOW);
    draw_circle_filled(f, lx, ly, 1, COL_BG);
    lx += 14;

    // Cardinal direction letters
    const char *cp = cardinal;

    while (*cp) {
        switch (*cp) {
            case 'N':
                draw_letter_N(f, lx + 7, ly + 12, COL_WHITE);
                lx += 22;
                break;

            case 'E':
                draw_letter_E(f, lx + 7, ly + 12, COL_WHITE);
                lx += 22;
                break;

            case 'S':
                draw_letter_S(f, lx + 7, ly + 12, COL_WHITE);
                lx += 22;
                break;

            case 'W':
                draw_letter_W(f, lx + 7, ly + 12, COL_WHITE);
                lx += 22;
                break;
        }

        cp++;
    }
}

// ────────────────────────────────────────────────────────────────────
// 7-seg display
// ────────────────────────────────────────────────────────────────────
void display_degrees(int degrees) {
    for (int i = 0; i < 8; i++) {
        sseg.write_1ptn(0xff, i);
    }

    sseg.write_1ptn(sseg.h2s((degrees / 100) % 10), 2);
    sseg.write_1ptn(sseg.h2s((degrees /  10) % 10), 1);
    sseg.write_1ptn(sseg.h2s( degrees        % 10), 0);
}

// ────────────────────────────────────────────────────────────────────
// LED flash
// ────────────────────────────────────────────────────────────────────
void flash_cardinal_led(int degrees, bool flash_on) {
    int led_bit = -1;

    if      (degrees >= 315 || degrees < 45) led_bit = 15;
    else if (degrees < 135)                  led_bit = 3;
    else if (degrees < 225)                  led_bit = 7;
    else                                     led_bit = 11;

    led.write(flash_on ? (1 << led_bit) : 0);
}

// ────────────────────────────────────────────────────────────────────
// MAIN
// ────────────────────────────────────────────────────────────────────
int main() {
    uart.disp("--- DIGITAL COMPASS ---\n\r");
    compass.init();

    // ── Init PS2 mouse ─────────────────────────────────────────────
    uart.disp("Initialising PS2 mouse...\n\r");

    int ps2_id = ps2.init();

    if (ps2_id != 2) {
        uart.disp("ERROR: PS2 mouse not detected. id=");
        uart.disp(ps2_id);
        uart.disp("\n\r");

        osd.bypass(1);
        frame.bypass(0);
        frame.clr_screen(COL_BLACK);

        frame_print(&frame, 95, 180,
                    "PS2 MOUSE NOT DETECTED",
                    COL_RED, 3);

        while (1) {}
    }

    uart.disp("PS2 mouse ready.\n\r");

    // Disable OSD completely. Use frame buffer text instead.
    osd.bypass(1);
    frame.bypass(0);

    // ── calibration screen using FrameCore text ────────────────
    frame.clr_screen(COL_BLACK);

    frame_print(&frame, 70, 120,
                "ROTATE THE COMPASS 360\nTO CALIBRATE",
                COL_BLUE, 3);

    frame_print(&frame, 85, 230,
                "CLICK MOUSE TO LOCK",
                COL_BLUE, 3);

    // ── Calibration ───────────────────────────────────────────────
    uart.disp("Rotate 360 to calibrate... click mouse to lock.\n\r");

    while (1) {
        int btn;

        compass.get_heading(11.202f, nullptr);

        if (mouse_get_click_edge(&btn)) {
            break;
        }

        sleep_ms(50);
    }

    compass.lock_calibration();

    uart.disp("Locked.\n\r");

    // ── Manual offset screen using FrameCore text ──────────────────
    frame.clr_screen(COL_BLACK);

    frame_print(&frame, 130, 70,
                "CALIBRATION LOCKED",
                COL_BLUE, 3);

    frame_print(&frame, 90, 130,
                "MANUAL OFFSET ADJUSTMENT",
                COL_BLUE, 3);

    frame_print(&frame, 80, 220,
                "LEFT CLICK  KEEP CURRENT",
                COL_BLUE, 2);

    frame_print(&frame, 80, 255,
                "RIGHT CLICK SUBTRACT CURRENT",
                COL_BLUE, 2);

    // ── Manual offset ─────────────────────────────────────────────
    while (1) {
        bool err;
        int cur = compass.get_heading(11.202f, &err);

        uart.disp("\rCurrent: ");
        uart.disp(cur);
        uart.disp(" deg  [left]=keep  [right]=subtract current  ");

        display_degrees(cur);

        // Clear changing heading area
        frame_fill_rect(&frame, 95, 330, 460, 35, COL_BLACK);

        frame_print(&frame, 95, 330,
                    "CURRENT HEADING: ",
                    COL_BLUE, 2);

        frame_print_int(&frame, 300, 330,
                        cur,
                        COL_BLUE, 2);

        frame_print(&frame, 350, 330,
                    " DEG",
                    COL_BLUE, 2);

        int btn;

        if (mouse_get_click_edge(&btn)) {
            if (btn == 1) {
                // Left click: keep current heading, no manual offset.
                frame.clr_screen(COL_BLACK);

                frame_print(&frame, 115, 160,
                            "KEEPING CURRENT HEADING",
                            COL_BLUE, 3);

                frame_print(&frame, 160, 225,
                            "DRAWING COMPASS",
                            COL_BLUE, 3);

                uart.disp("\n\rKeeping current heading.\n\r");

                sleep_ms(800);
                break;
            }

            if (btn == 2) {
                // Right click: subtract currently displayed heading.
                frame.clr_screen(COL_BLACK);

                frame_print(&frame, 120, 145,
                            "SUBTRACTING CURRENT",
                            COL_BLUE, 3);

                frame_print(&frame, 205, 205,
                            "OFFSET:",
                            COL_BLUE, 3);

                frame_print_int(&frame, 355, 205,
                                cur,
                                COL_BLUE, 3);

                compass.set_manual_offset((float)cur);

                uart.disp("\n\rSubtracting current heading as offset: ");
                uart.disp(cur);
                uart.disp("\n\rApplied.\n\r");

                sleep_ms(800);

                frame.clr_screen(COL_BLACK);

                frame_print(&frame, 170, 160,
                            "OFFSET APPLIED",
                            COL_BLUE, 3);

                frame_print(&frame, 160, 225,
                            "DRAWING COMPASS",
                            COL_BLUE, 3);

                sleep_ms(800);
                break;
            }
        }

        sleep_ms(200);
    }

    // ── Draw static rose ──────────────────────────────────────────
    osd.bypass(1);

    uart.disp("Drawing compass rose...\n\r");

    draw_compass_rose(&frame);

    uart.disp("Done.\n\r");

    frame.bypass(0);
    osd.bypass(1);

    // ── Live loop ─────────────────────────────────────────────────
    int  last_heading  = -1;
    bool flash_state   = false;
    int  flash_counter = 0;

    while (1) {
        bool err;
        int  head = compass.get_heading(11.202f, &err);
        const char *card = compass.get_cardinal(head);

        if (head != last_heading) {
            // Erase needle area
            draw_circle_filled(&frame, CX, CY, R_NEEDLE + 5, COL_DARK_GRAY);

            // Restore ticks erased by the circle fill
            redraw_ticks(&frame);

            // Draw new needle
            draw_needle(&frame, CX, CY, head);

            // Hub on top
            draw_circle_filled(&frame, CX, CY, R_HUB, COL_WHITE);

            draw_info_panel(&frame, head, card);
            display_degrees(head);

            last_heading = head;
        }

        uart.disp("\rHeading: ");
        uart.disp(head);
        uart.disp(" | ");
        uart.disp(card);
        uart.disp("       ");

        flash_counter++;

        if (flash_counter >= 2) {
            flash_state   = !flash_state;
            flash_counter = 0;
        }

        flash_cardinal_led(head, flash_state);

        sleep_ms(250);
    }
}
