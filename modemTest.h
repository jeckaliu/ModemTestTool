//modemTest.c head file
#ifndef MODEMTEST_H__
#define MODEMTEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include<sys/stat.h>

#define ok_error(num) ((num)==EIO)
#define UART_PORT_PATH "/dev/ttyUSB0"
#define MAX_CMD_LENGTH (8 * 1024)
#define MAX_AT_RESPONSE (8 * 1024)

//#define LTE_MODEM 0
char *readline(int fd);
int OpenAndSetUartPriority();
int writeline (const char *s, int fd);
int writeTDModemline (const char *s, int fd);

#endif //MODEMTEST_H__