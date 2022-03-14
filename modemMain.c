//模组测试C代码主循环函数，循环获取用户命令及执行结果，在未关闭本app前会一直占用模组的AT口，用于下发和读取
//AT命令执行结果
//通过UART_PORT_PATH 来配置需要打开的AT端口
#include "modemTest.h"

int main(int argc, char **argv)
{
    int fd = 0;
    char cmdBuf [MAX_CMD_LENGTH] = {};
    
    //设置端口属性
    fd = OpenAndSetUartPriority();
    if (fd < 0) {
        printf("Open at port failed!\n");
        return -1;
    }
    
    //开启循环用户输入模式
    while(1){
        scanf("%s", cmdBuf);
        if(strcmp("exit",cmdBuf) == 0) {
            printf("exit at handle process now!\n");
            break;
        }
        writeTDModemline(cmdBuf, fd);
        //at+cfun=1由于内容较多，需要加大等待时长才能正常读取
        if(strcmp("at+cfun=1",cmdBuf) == 0) {
            sleep(5);
        }
        //写入AT指令后一定要做等待，否则会导致读取端口获取不到数据，出现异常at打印
        usleep(300);
        readline(fd);
    }
    //执行AT完毕，关闭AT口
    if (close(fd)  < 0){
        printf("close port fd failed!\n");
        return -1;
    }   
    return 0;
}
