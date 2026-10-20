/*
 * input_processing.c
 *
 *  Created on: Oct 19, 2026
 *      Author: phamn
 */


#include "input_processing.h"
#include "input_reading.h"
#include "main.h"
#include "timer.h"
#include "led_display.h"

extern int mode;
extern int red_temp, yellow_temp, green_temp;
extern int red_value, yellow_value, green_value;
extern int count1, count2;
extern int timer_flag[4];

enum ButtonState { BUTTON_RELEASED, BUTTON_PRESSED, BUTTON_PRESSED_MORE_THAN_1_SECOND };
enum ButtonState buttonState[3] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};
static int traffic_state = 0;

void resetCountValue() {
    traffic_state = 0;
    count1 = red_value;
    count2 = green_value;
    setTimer(2, 1000);
    timer_flag[2] = 0;
}

void UpdateMode() {
    switch (buttonState[0]) {
        case BUTTON_RELEASED:
            if (is_button_pressed(0)) {
                buttonState[0] = BUTTON_PRESSED;
                mode++;
                if (mode > 4) {
                    mode = 1;
                    resetCountValue();
                }

                HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_RESET);

                if (mode == 2) {
                    red_temp = red_value;
                    setTimer(1, 250);
                } else if (mode == 3) {
                    yellow_temp = yellow_value;
                    setTimer(1, 250);
                } else if (mode == 4) {
                    green_temp = green_value;
                    setTimer(1, 250);
                }
            }
            break;
        case BUTTON_PRESSED:
            if (!is_button_pressed(0)) {
                buttonState[0] = BUTTON_RELEASED;
            }
            break;
        default:
            break;
    }
}

static void increaseTempValue() {
    switch (mode) {
        case 2:
            red_temp++;
            if (red_temp > 99) red_temp = 1;
            break;
        case 3:
            yellow_temp++;
            if (yellow_temp > 99) yellow_temp = 1;
            break;
        case 4:
            green_temp++;
            if (green_temp > 99) green_temp = 1;
            break;
    }
}

static void setDurationValue() {
    switch (mode) {
        case 2:
            red_value = red_temp;
            if (red_value <= yellow_value) {
                red_value = yellow_value + 1;
                red_temp = red_value;
            }
            green_value = red_value - yellow_value;
            green_temp = green_value;
            break;

        case 3:
            yellow_value = yellow_temp;
            red_value = green_value + yellow_value;
            if (red_value > 99) {
                red_value = 99;
                yellow_value = red_value - green_value;
                yellow_temp = yellow_value;
            }
            red_temp = red_value;
            break;

        case 4:
            green_value = green_temp;
            red_value = green_value + yellow_value;
            if (red_value > 99) {
                red_value = 99;
                green_value = red_value - yellow_value;
                green_temp = green_value;
            }
            red_temp = red_value;
            break;

        default:
            break;
    }
}

void UpdateDurationValue() {
    switch (buttonState[1]) {
        case BUTTON_RELEASED:
            if (is_button_pressed(1)) {
                buttonState[1] = BUTTON_PRESSED;
                increaseTempValue();
            }
            break;
        case BUTTON_PRESSED:
            if (!is_button_pressed(1)) {
                buttonState[1] = BUTTON_RELEASED;
            } else if (is_button_pressed_1s(1)) {
                buttonState[1] = BUTTON_PRESSED_MORE_THAN_1_SECOND;
                increaseTempValue();
            }
            break;
        case BUTTON_PRESSED_MORE_THAN_1_SECOND:
            if (!is_button_pressed(1)) {
                buttonState[1] = BUTTON_RELEASED;
            } else if (is_button_hold(1)) {
                reset_flagForButtonHold(1);
                increaseTempValue();
            }
            break;
    }

    switch (buttonState[2]) {
            case BUTTON_RELEASED:
                if (is_button_pressed(2)) {
                    buttonState[2] = BUTTON_PRESSED;
                    setDurationValue();
                }
                break;
            case BUTTON_PRESSED:
                if (!is_button_pressed(2)) {
                    buttonState[2] = BUTTON_RELEASED;
                }
                break;
            
            case BUTTON_PRESSED_MORE_THAN_1_SECOND:
                if (!is_button_pressed(2)) {
                    buttonState[2] = BUTTON_RELEASED;
                }
                break;
            default:
                break;
       }
}

void fsm_for_input_processing(void) {
    UpdateMode();
    UpdateDurationValue();
}

void fsm_traffic_light_run(void) {
    switch (mode) {
        case 1:
            if (count1 <= 0 && count2 <= 0) {
                resetCountValue();
            }
            switch (traffic_state) {
                case 0:
                    HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
                    break;
                case 1:
                    HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_RESET);
                    break;
                case 2:
                    HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_RESET);
                    break;
                case 3:
                    HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_RESET);
                    break;
            }

            if (timer_flag[2] == 1) {
                setTimer(2, 1000);
                if (count1 > 0) count1--;
                if (count2 > 0) count2--;

                switch (traffic_state) {
                    case 0:
                        if (count2 <= 0) {
                            traffic_state = 1;
                            count2 = yellow_value;
                        }
                        break;
                    case 1:
                        if (count2 <= 0) {
                            traffic_state = 2;
                            count1 = green_value;
                            count2 = red_value;
                        }
                        break;
                    case 2:
                        if (count1 <= 0) {
                            traffic_state = 3;
                            count1 = yellow_value;
                        }
                        break;
                    case 3:
                        if (count1 <= 0) {
                            traffic_state = 0;
                            count1 = red_value;
                            count2 = green_value;
                        }
                        break;
                }
            }
            break;

        case 2:
            if (timer_flag[1] == 1) {
                setTimer(1, 250);
                HAL_GPIO_TogglePin(R0_GPIO_Port, R0_Pin);
                HAL_GPIO_TogglePin(R1_GPIO_Port, R1_Pin);
                HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_RESET);
            }
            break;

        case 3:
            if (timer_flag[1] == 1) {
                setTimer(1, 250);
                HAL_GPIO_TogglePin(Y0_GPIO_Port, Y0_Pin);
                HAL_GPIO_TogglePin(Y1_GPIO_Port, Y1_Pin);
                HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(G0_GPIO_Port, G0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(G1_GPIO_Port, G1_Pin, GPIO_PIN_RESET);
            }
            break;

        case 4:
            if (timer_flag[1] == 1) {
                setTimer(1, 250);
                HAL_GPIO_TogglePin(G0_GPIO_Port, G0_Pin);
                HAL_GPIO_TogglePin(G1_GPIO_Port, G1_Pin);
                HAL_GPIO_WritePin(R0_GPIO_Port, R0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Y0_GPIO_Port, Y0_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Y1_GPIO_Port, Y1_Pin, GPIO_PIN_RESET);
            }
            break;
    }
}
