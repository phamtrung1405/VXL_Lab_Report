/*
 * scheduler.c
 *
 *  Created on: Oct 28, 2026
 *      Author: phamn
 */

#include "scheduler.h"
#include "main.h"

// Define states for tasks
#define TASK_FREE     0
#define TASK_WAITING  1
#define TASK_READY    2
#define TASK_RUNNING  3

sTask SCH_tasks_G[SCH_MAX_TASKS];
static int16_t listHead = -1;

static uint8_t readyQueue[SCH_MAX_TASKS];
static uint8_t rqHead = 0;
static uint8_t rqTail = 0;
static uint8_t rqCount = 0;

static volatile uint32_t tick = 0;
static uint32_t next_task_id = 1;
unsigned char Error_code_G = 0;

// Critical section macros using FreeRTOS style or bare-metal CMSIS
#define CRIT_ENTER()  do { primask = __get_PRIMASK(); __disable_irq(); } while(0)
#define CRIT_EXIT()   do { __set_PRIMASK(primask); } while(0)

#define MS_TO_TICKS(ms)  ((ms) / 10)



static void SCH_Insert_Waiting(int16_t index, uint32_t delay) {
    int16_t prev = -1;
    int16_t cur = listHead;

    while (cur >= 0 && delay >= SCH_tasks_G[cur].Delay) {
        delay -= SCH_tasks_G[cur].Delay;
        prev = cur;
        cur = SCH_tasks_G[cur].Next;
    }

    SCH_tasks_G[index].Delay = delay;
    SCH_tasks_G[index].Next = cur;
    SCH_tasks_G[index].State = TASK_WAITING;

    if (cur >= 0) {
        SCH_tasks_G[cur].Delay -= delay;
    }
    if (prev < 0) {
        listHead = index;
    } else {
        SCH_tasks_G[prev].Next = index;
    }
}

static void SCH_Delete(int16_t index) {
    SCH_tasks_G[index].pTask = 0;
    SCH_tasks_G[index].Delay = 0;
    SCH_tasks_G[index].Period = 0;
    SCH_tasks_G[index].TaskID = 0;
    SCH_tasks_G[index].Next = -1;
    SCH_tasks_G[index].State = TASK_FREE;
}

static void SCH_RQ_Push(uint8_t index) {
    readyQueue[rqTail] = index;
    rqTail = (rqTail + 1) % SCH_MAX_TASKS;
    rqCount++;
}

static uint8_t SCH_RQ_Pop(void) {
    uint8_t index = readyQueue[rqHead];
    rqHead = (rqHead + 1) % SCH_MAX_TASKS;
    rqCount--;
    return index;
}

void SCH_Init(void) {
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_Delete(i);
    }
    listHead = -1;
    rqHead = 0;
    rqTail = 0;
    rqCount = 0;
    tick = 0;
    next_task_id = 1;
    Error_code_G = 0;
}

void SCH_Update(void) {
    tick++;

    if (listHead < 0) {
        return;
    }

    if (SCH_tasks_G[listHead].Delay > 0) {
        SCH_tasks_G[listHead].Delay--;
    }

    while (listHead >= 0 && SCH_tasks_G[listHead].Delay == 0) {
        int16_t index = listHead;
        listHead = SCH_tasks_G[index].Next;

        SCH_tasks_G[index].Next = -1;
        SCH_tasks_G[index].State = TASK_READY;
        SCH_tasks_G[index].DueTick = tick;
        SCH_RQ_Push((uint8_t)index);
    }
}

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
    uint32_t primask;
    int16_t index = -1;
    uint32_t delayTicks = MS_TO_TICKS(DELAY);
    uint32_t periodTicks = MS_TO_TICKS(PERIOD);

    if (pFunction == 0) {
        return 0;
    }
    if (PERIOD > 0 && periodTicks == 0) {
        periodTicks = 1;
    }

    CRIT_ENTER();

    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        if (SCH_tasks_G[i].State == TASK_FREE) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        CRIT_EXIT();
        return SCH_MAX_TASKS;
    }

    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Period = periodTicks;
    SCH_tasks_G[index].TaskID = next_task_id++;
    if (next_task_id == 0) {
        next_task_id = 1;
    }

    SCH_Insert_Waiting(index, delayTicks);

    uint32_t id = SCH_tasks_G[index].TaskID;

    CRIT_EXIT();

    return id;
}

void SCH_Dispatch_Tasks(void) {
    uint32_t primask;

    for (;;) {
        void (*fn)(void);
        uint32_t id;
        uint8_t index;

        CRIT_ENTER();

        if (rqCount == 0) {
            CRIT_EXIT();
            break;
        }

        index = SCH_RQ_Pop();

        if (SCH_tasks_G[index].pTask == 0) {
            SCH_Delete(index);
            CRIT_EXIT();
            continue;
        }

        SCH_tasks_G[index].State = TASK_RUNNING;
        fn = SCH_tasks_G[index].pTask;
        id = SCH_tasks_G[index].TaskID;

        CRIT_EXIT();

        fn();

        CRIT_ENTER();

        if (SCH_tasks_G[index].pTask != 0 &&
            SCH_tasks_G[index].TaskID == id &&
            SCH_tasks_G[index].Period > 0) {

            uint32_t elapsed = tick - SCH_tasks_G[index].DueTick;
            uint32_t period = SCH_tasks_G[index].Period;

            SCH_Insert_Waiting(index, (period > elapsed) ? (period - elapsed) : 0);

        } else {
            SCH_Delete(index);
        }

        CRIT_EXIT();
    }


}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    uint32_t primask;
    int16_t index = -1;

    if (taskID == 0) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return RETURN_ERROR;
    }

    CRIT_ENTER();

    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        if (SCH_tasks_G[i].State != TASK_FREE && SCH_tasks_G[i].TaskID == taskID) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        CRIT_EXIT();
        return RETURN_ERROR;
    }

    if (SCH_tasks_G[index].State == TASK_WAITING) {
        int16_t prev = -1;
        int16_t cur = listHead;

        while (cur >= 0 && cur != index) {
            prev = cur;
            cur = SCH_tasks_G[cur].Next;
        }

        if (cur == index) {
            int16_t next = SCH_tasks_G[index].Next;

            if (next >= 0) {
                SCH_tasks_G[next].Delay += SCH_tasks_G[index].Delay;
            }
            if (prev < 0) {
                listHead = next;
            } else {
                SCH_tasks_G[prev].Next = next;
            }
        }

        SCH_Delete(index);

    } else {
        SCH_tasks_G[index].pTask = 0;
        SCH_tasks_G[index].Period = 0;
    }

    CRIT_EXIT();

    return RETURN_NORMAL;
}

uint32_t SCH_Get_Time(void) {
    return tick * 10;
}

//void SCH_Go_To_Sleep(void) {
//    // __WFI();
//}
