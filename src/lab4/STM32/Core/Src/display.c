/*
 * display.c
 *
 *  Created on: Oct 28, 2026
 *      Author: phamn
 */

#include "display.h"


extern UART_HandleTypeDef huart2;

void Print_Timestamp_10ms(void) {
  char str[50];
  sprintf(str, "Tick 10ms - Timestamp: %lu\r\n", (unsigned long)get_time());
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}


void LED0_Display(void) {
  HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);

  char str[50];
  sprintf(str, "LED 0 Toggled (0.5s) - Timestamp: %lu ms\r\n",
          (unsigned long)get_time());
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}


void LED1_Display(void) {
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

  char str[50];
  sprintf(str, "LED 1 Toggled (1.0s) - Timestamp: %lu ms\r\n",
          (unsigned long)get_time());
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}


void LED2_Display(void) {
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

  char str[50];
  sprintf(str, "LED 2 Toggled (1.5s) - Timestamp: %lu ms\r\n",
          (unsigned long)get_time());
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}


void LED3_Display(void) {
  HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);

  char str[50];
  sprintf(str, "LED 3 Toggled (2.0s) - Timestamp: %lu ms\r\n",
          (unsigned long)get_time());
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}


void LED4_Display(void) {
  HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);

  char str[50];
  sprintf(str, "LED 4 Toggled (2.5s) - Timestamp: %lu ms\r\n",
          (unsigned long)get_time());
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}
