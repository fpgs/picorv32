// Display Library
#include <stdint.h>
#include "efb.h"
#include "i2c.h"
#include "ufm.h"
#include "disp.h"

static uint8_t disp_font_buf[16];
static int32_t disp_font_page;

int32_t disp_init() {
    disp_font_page = -1;
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, 0xAE);  // Display Off
    i2c_stop();

    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, 0xA1);  // Segment Re-map
    i2c_stop();

    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, 0x8D);  // Charge Pump Command
    i2c_stop();
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, 0x14);  // Charge Pump Enabled
    i2c_stop();

    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, 0xAF);  // Display On
    i2c_stop();
    
    return 0x00;
}


int32_t disp_fill(uint8_t data){
    int32_t i, j;
    for (i = 0; i < 8 ; i++) {
        i2c_wbyte(I2C_START, DISP_I2C_ADDR);
        i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
        i2c_wbyte(I2C_WRITE, (0xB0|i));  // Page i
        i2c_stop();
        i2c_wbyte(I2C_START, DISP_I2C_ADDR);
        i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
        i2c_wbyte(I2C_WRITE, 0x10);  // Column 0x0_
        i2c_stop();
        i2c_wbyte(I2C_START, DISP_I2C_ADDR);
        i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
        i2c_wbyte(I2C_WRITE, 0x00);  // Column 0x_0
        i2c_stop();
        for (j= 0; j < 128; j++){
            i2c_wbyte(I2C_START, DISP_I2C_ADDR);
            i2c_wbyte(I2C_WRITE, DISP_DATA); 
            i2c_wbyte(I2C_WRITE, data);  // fill data
            i2c_stop();
        }
    }
    return 0x00;
}


int32_t disp_hex(uint32_t x, uint32_t y, uint32_t data){
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, (0xB0|y));  // Page y
    i2c_stop();
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, (0x10|((x>>4)&0x0F)));  // Column x high nibble
    i2c_stop();
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, (x&0x0F));  // Column x low nibble
    i2c_stop();
    int32_t nib, need_page, start_index, i, j;
    for (i = 7; i >= 0 ; i--) {
        nib = ((data >> (i << 2))&0x0F);
        start_index = ((nib & 1)<<3);  // odd bytes start at the 8th byte in the page
        need_page = (nib>>1)+DISP_1ST_PAGE; // two characters per page
        // check to see if the buffer has the data you need before reading UFM
        if (disp_font_page != need_page) {  
            ufm_read_page(need_page, disp_font_buf);
            disp_font_page = need_page;
        }
        for (j= start_index; j < (start_index+5); j++){
            i2c_wbyte(I2C_START, DISP_I2C_ADDR);
            i2c_wbyte(I2C_WRITE, DISP_DATA); 
            i2c_wbyte(I2C_WRITE, (disp_font_buf[j]));  // font data from buffer
            i2c_stop();
        }
        i2c_wbyte(I2C_START, DISP_I2C_ADDR);
        i2c_wbyte(I2C_WRITE, DISP_DATA); 
        i2c_wbyte(I2C_WRITE, 0x00);  
        i2c_stop();

    }
    return 0x00;
}

int32_t disp_str(uint32_t x, uint32_t y, uint8_t *ascii, uint32_t cnt){
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, (0xB0|y));  // Page y
    i2c_stop();
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, (0x10|((x>>4)&0x0F)));  // Column x high nibble
    i2c_stop();
    i2c_wbyte(I2C_START, DISP_I2C_ADDR);
    i2c_wbyte(I2C_WRITE, DISP_COMMAND); 
    i2c_wbyte(I2C_WRITE, (x&0x0F));  // Column x low nibble
    i2c_stop();
    int32_t need_page, start_index, i, j;
    for (i = 0; i < cnt ; i++) {
        start_index = ((ascii[i] & 1)<<3);  // odd bytes start at the 8th byte in the page
        need_page = (ascii[i]>>1)+DISP_1ST_PAGE; // two characters per page
        // check to see if the buffer has the data you need before reading UFM
        if (disp_font_page != need_page) {  
            ufm_read_page(need_page, disp_font_buf);
            disp_font_page = need_page;
        }
        for (j= start_index; j < (start_index+5); j++){
            i2c_wbyte(I2C_START, DISP_I2C_ADDR);
            i2c_wbyte(I2C_WRITE, DISP_DATA); 
            i2c_wbyte(I2C_WRITE, (disp_font_buf[j]));  // font data from buffer
            i2c_stop();
        }
        i2c_wbyte(I2C_START, DISP_I2C_ADDR);
        i2c_wbyte(I2C_WRITE, DISP_DATA); 
        i2c_wbyte(I2C_WRITE, 0x00);  
        i2c_stop();

    }
    return 0x00;
}
