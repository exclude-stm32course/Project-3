#pragma once

typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

int get_args(char *text, char ***argv, int *argc);
void free_args(char **argv, const int argc);
void output(UART_HandleTypeDef *huart, const char *fmt, ...);


