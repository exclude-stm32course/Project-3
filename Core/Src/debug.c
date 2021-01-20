#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx_hal.h"

#define MAX_ARGS 20

static char *get_arg(const char *text, const int length)
{

	char *new_text = (char*)malloc(sizeof(char)*(length+1));
	if(!new_text) return NULL;
	memcpy(new_text, text, length);
	new_text[length] = 0;
	return new_text;
}

int get_args(char *text, char ***argv, int *argc)
{
	uint offset = 0;
	uint first_pos = 0;
	char *base_char;
	const int len = strlen(text);

	char **new_argv = malloc(sizeof(char*)*MAX_ARGS);
	if(!*new_argv) return -1;
	char **argv_iterator = new_argv;
	int cnt = 0;

	for(int i = 0; i < len; i++) {
		/* Remove leading and double spaces */
		if(!first_pos && *text == ' ') {
			text++;
			continue;
		}

		/* position saved and the first char is not a space */
		if(!first_pos) {
			first_pos = 1;
			/* Set new tmp_char which is the base address */
			base_char = text;
		}

		/* Increment offset */
		offset++;

		/* check if next char is a space. */
		if(base_char[offset] == ' ') {
			*argv_iterator = get_arg(base_char, offset);
			if(!*argv_iterator) return -1;
			++argv_iterator;
			++cnt;
			/* We know that we should jump one more to the right
			 * because we hit a space
			 * */
			text += offset + 1;
			/* if we won't check next char, increment i as well */
			++i;

			/* Reset counter */
			offset = 0;
			first_pos = 0;
		}
	}

	if(first_pos) {
		*argv_iterator = get_arg(base_char, offset);
		if(!*argv_iterator) return -1;
		++argv_iterator;
		++cnt;
	}

	*argv = new_argv;
	*argc = cnt;
	return 0;
}

void free_args(char **argv, const int argc)
{
	char **argv_iterator = argv;
	for(int i = 0; i < argc ; ++i) {
		free(*argv_iterator);
		++argv_iterator;
	}
	free(argv);
}

void output(UART_HandleTypeDef *huart, const char *fmt, ...)
{
	char buffer[256];
	char *buf_ptr = buffer;
	memset(buffer, 0, 256);

	strcpy(buffer, "DEBUG: ");
	buf_ptr += strlen(buffer);
	va_list args;
	va_start (args, fmt);
	vsprintf (buf_ptr,fmt, args);

	int len = strlen(buffer);
	buffer[len] = '\r';
	buffer[len+1] = '\n';
	len += 2;
	HAL_UART_Transmit(huart, (uint8_t*)buffer, len, 500);
	va_end (args);
}
