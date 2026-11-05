/*
 * timer.h
 *
 *  Created on: Oct 28, 2026
 *      Author: phamn
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"
#include "scheduler.h"
#include <stdint.h>

void timer_run(void);
uint32_t get_time(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* INC_TIMER_H_ */
