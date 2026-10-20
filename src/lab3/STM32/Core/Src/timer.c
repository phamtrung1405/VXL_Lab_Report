/*
 * timer.c
 *
 *  Created on: Oct 19, 2026
 *      Author: phamn
 */


#include "main.h"
#include "timer.h"
#include "input_reading.h"

extern int time_buffer[4];
extern int timer_counter[4];
extern int timer_flag[4];
extern int TIMER_CYCLE;

void setTimer(int index, int duration){
    timer_counter[index] = duration / TIMER_CYCLE;
    timer_flag[index] = 0;
}

void timer_run(){
    for(int i = 0; i < 3; i++){
        if(timer_counter[i] > 0){
            timer_counter[i]--;
            if(timer_counter[i] == 0) timer_flag[i] = 1;
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        button_reading();
        timer_run();
    }
}
