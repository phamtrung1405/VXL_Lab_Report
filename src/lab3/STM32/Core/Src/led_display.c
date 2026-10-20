/*
 * led_display.c
 *
 *  Created on: Oct 19, 2026
 *      Author: phamn
 */


#include "led_display.h"
#include "main.h"
#include "timer.h"

extern int timer_flag[3];
extern int time_buffer[4];
extern int mode;
extern int count1, count2;
extern int red_temp, yellow_temp, green_temp;

const uint8_t segment_code[10] = {
    0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10
};

void update_led_buffer() {
    if (mode == 1) { // Mode normal
        time_buffer[0] = count1 / 10;
        time_buffer[1] = count1 % 10;
        time_buffer[2] = count2 / 10;
        time_buffer[3] = count2 % 10;
    } else if (mode == 2) { // Mode 2: RED
        time_buffer[0] = red_temp / 10;
        time_buffer[1] = red_temp % 10;
        time_buffer[2] = 0;
        time_buffer[3] = 2;
    } else if (mode == 3) { // Mode 3: YELLOW
        time_buffer[0] = yellow_temp / 10;
        time_buffer[1] = yellow_temp % 10;
        time_buffer[2] = 0;
        time_buffer[3] = 3;
    } else if (mode == 4) { // Mode 4: GREEN
        time_buffer[0] = green_temp / 10;
        time_buffer[1] = green_temp % 10;
        time_buffer[2] = 0;
        time_buffer[3] = 4;
    }
}

void display7SEG(int num) {
    uint8_t code = segment_code[num];
    HAL_GPIO_WritePin(SEG0_GPIO_Port, SEG0_Pin, (code & (1 << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG1_GPIO_Port, SEG1_Pin, (code & (1 << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG2_GPIO_Port, SEG2_Pin, (code & (1 << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG3_GPIO_Port, SEG3_Pin, (code & (1 << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG4_GPIO_Port, SEG4_Pin, (code & (1 << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG5_GPIO_Port, SEG5_Pin, (code & (1 << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG6_GPIO_Port, SEG6_Pin, (code & (1 << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}



void LEDScanning() {
    static int scan_index = 0;

    HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_SET);

    display7SEG(time_buffer[scan_index]);

    switch (scan_index) {
        case 0:
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, GPIO_PIN_RESET);
            scan_index = 1;
            break;
        case 1:
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_RESET);
            scan_index = 2;
            break;
        case 2:
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_RESET);
            scan_index = 3;
            break;
        case 3:
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, GPIO_PIN_RESET);
            scan_index = 0;
            break;
    }
}

