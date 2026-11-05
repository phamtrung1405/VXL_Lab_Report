/*
 * timer.c
 *
 *  Created on: Oct 28, 2026
 *      Author: phamn
 */

#include "timer.h"

uint32_t system_time = 0;

void timer_run(void) {
    system_time += 10;
}


uint32_t get_time(void) {
    return system_time;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        SCH_Update();
        timer_run();
    }
}
