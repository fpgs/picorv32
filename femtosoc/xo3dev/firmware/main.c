// simple downcounter for femtosoc

#include <stdint.h>
#include "efb.h"
#include "i2c.h"
#include "ufm.h"
#include "disp.h"

#define WB_BASE   0x04000000
#define EFB_BASE  (WB_BASE + 0x0)
#define EFB       ((efb_t *) EFB_BASE)

#define REG_LEDS (*(volatile uint32_t*)0x01000000)
#define DELAY_CNT 500

void delay_ms(int32_t ms) {
    int32_t dly;
    while(ms > 0) {
        ms -= 1;
        dly = DELAY_CNT;
        while(dly > 0) {
        dly -= 1;
        }
    }
}

void main() {
    int32_t i;
    REG_LEDS = 0xFB;
    i2c_init(EFB);
    ufm_init(EFB);
    delay_ms(100); // allow time for display to power on
    REG_LEDS = 0xD0;
    disp_init();
    disp_fill(0x00);
    disp_hex(0, 7, 0xDEADBEEF);
    disp_hex(0, 5, 0x01234567);
    disp_hex(48, 5, 0x89ABCDEF);
    uint8_t ascii_str[16] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F};
    disp_str(0, 3, ascii_str, 16);
    for (i=0; i <16; i++) {
        ascii_str[i] += 0x10;
    }
    disp_str(0, 2, ascii_str, 16);
    for (i=0; i <16; i++) {
        ascii_str[i] += 0x10;
    }
    disp_str(0, 1, ascii_str, 16);

    
    delay_ms(1000);
    i = -1;
    while(1) {
        i += 1;
        REG_LEDS = i >> 8;
        EFB->TCOCRSET[0] = i; 
        if ((i & 0xFF)==0) {
            disp_hex(0,7,i);
        }       
        delay_ms(5);
    }
}