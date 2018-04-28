#ifndef UART_H
#define UART_H 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>


void termios_setup(struct termios * my_term, int descriptor);

void read_byte(int fd,char *received);

void uart_setup(void);

#endif
