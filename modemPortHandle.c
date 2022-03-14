//处理模组端口属性相关
#include "modemTest.h"

int OpenAndSetUartPriority()
{
    int fd;
    struct termios old_termios ;

    //open port
   chmod("/dev/ttyUSB0", 0666);
    fd = open("/dev/ttyUSB0", O_RDWR);
    if (fd < 0){
        printf("Open %s failed!,error:%d\n","/dev/ttyUSB0", fd);
        printf("please give suppre rights 'sudo modemtest' then have a try!!!\n");
        return -1;
    }

    //get port priority 
    if(tcgetattr(fd, &old_termios) == -1) {
        printf("Get port attr fail.\n");
        close(fd);
        return -1;
    }
#ifdef LTE_MODEM
    old_termios.c_lflag = 0;
    old_termios.c_ispeed = B115200;
    tcsetattr(fd, TCSANOW, &old_termios);
#else
    int temp = 0;
    old_termios.c_cflag     &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
    old_termios.c_cflag     |= CS8 | CREAD | HUPCL;
    old_termios.c_iflag      = IGNBRK | IGNPAR;
    old_termios.c_oflag      = 0;
    old_termios.c_lflag      = 0;
    old_termios.c_cc[VMIN]   = 1;
    old_termios.c_cc[VTIME]  = 0;
    old_termios.c_cflag ^= (CLOCAL | HUPCL);
    old_termios.c_ispeed = B115200;
    tcsetattr(fd, TCSANOW, &old_termios);
    while (temp < 0 && !ok_error(errno))
    if (errno != EINTR)
    {printf("tcsetattr: (line %d)", __LINE__); }
    tcflush(fd, TCIOFLUSH);
#endif
    return fd;
}
