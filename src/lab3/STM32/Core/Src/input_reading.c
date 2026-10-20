/*
 * input_reading.c
 *
 *  Created on: Oct 19, 2026
 *      Author: phamn
 */


#include "main.h"
#include "input_reading.h"

#define NO_OF_BUTTONS 3
#define DURATION_FOR_AUTO_INCREASING    1000 / TIMER_CYCLE
#define DURATION_FOR_HOLD               500 / TIMER_CYCLE
#define BUTTON_IS_PRESSED GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

extern int TIMER_CYCLE;

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer3[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};

static uint8_t flagForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};
static uint8_t flagForButtonHold[NO_OF_BUTTONS] = {0, 0, 0};
static uint16_t counterForButtonHold[NO_OF_BUTTONS] = {0, 0, 0};

// Bổ sung mảng ánh xạ chân vật lý an toàn
uint16_t button_pins[3] = {MODE_Pin, SET_Pin, SAVE_Pin};
GPIO_TypeDef* button_ports[3] = {MODE_GPIO_Port, SET_GPIO_Port, SAVE_GPIO_Port};

void init_buffer() {
    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        buttonBuffer[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer1[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer2[i] = BUTTON_IS_RELEASED;
        debounceButtonBuffer3[i] = BUTTON_IS_RELEASED;
        flagForButtonPress1s[i] = 0;
        flagForButtonHold[i] = 0;
        counterForButtonPress1s[i] = 0;
        counterForButtonHold[i] = 0;
    }
}

void button_reading(void) {
    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer3[i] = debounceButtonBuffer2[i];
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];


        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(button_ports[i], button_pins[i]);

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i] && debounceButtonBuffer3[i] == debounceButtonBuffer2[i])
            buttonBuffer[i] = debounceButtonBuffer1[i];

        if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
            if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                counterForButtonPress1s[i]++;
            } else {
                flagForButtonPress1s[i] = 1;
                if (counterForButtonHold[i] < DURATION_FOR_HOLD) {
                    counterForButtonHold[i]++;
                    if (counterForButtonHold[i] >= DURATION_FOR_HOLD) {
                        counterForButtonHold[i] = 0;
                        flagForButtonHold[i] = 1;
                    }
                }
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i] = 0;
            counterForButtonHold[i] = 0;
            flagForButtonHold[i] = 0;
        }
    }
}

unsigned char is_button_pressed(uint8_t index){
    if(index >= NO_OF_BUTTONS) return 0;
    return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}

unsigned char is_button_pressed_1s(unsigned char index){
    if(index >= NO_OF_BUTTONS) return 0xff;
    return (flagForButtonPress1s[index] == 1);
}

unsigned char is_button_hold(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (flagForButtonHold[index] == 1);
}

void reset_flagForButtonHold(unsigned char index) {
    flagForButtonHold[index] = 0;
}
