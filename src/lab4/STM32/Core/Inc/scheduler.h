/*
 * scheduler.h
 *
 *  Created on: Oct 28, 2026
 *      Author: phamn
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>
#include <string.h>
#include "main.h"


#define SCH_MAX_TASKS       5
#define NO_TASK_ID          0
#define RETURN_NORMAL       0
#define RETURN_ERROR        1


#define SCH_TICK_MS         10


extern unsigned char Error_code_G;


#define ERROR_SCH_TOO_MANY_TASKS    1
#define ERROR_SCH_CANNOT_DELETE_TASK 2


typedef struct {
    void (*pTask)(void);
    uint32_t Delay;
    uint32_t Period;
    uint32_t TaskID;
    uint32_t DueTick;       // Thời điểm (tick) lúc task đến hạn
    int16_t  Next;
    uint8_t  State;
} sTask;


void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);


uint32_t SCH_Get_Time(void);
// void SCH_Go_To_Sleep(void);

#endif /* INC_SCHEDULER_H_ */
