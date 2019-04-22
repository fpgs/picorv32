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
    int32_t i,j;
    uint8_t ascii_str[16];
    REG_LEDS = 0xFB;
    i2c_init(EFB);
    ufm_init(EFB);
    delay_ms(100); // allow time for display to power on
    REG_LEDS = 0xD0;
    disp_init();
    disp_fill(0x00);
    for (j=0; j < 8 ; j++) {
        for (i=0; i <16; i++) {
            ascii_str[i] = (j<<4)+i;
        }
        disp_str(0, j, ascii_str, 16);
    }
    
    delay_ms(1000);
    disp_chr(48, 7, ' ');
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