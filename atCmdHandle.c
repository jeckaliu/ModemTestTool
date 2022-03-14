//处理模组AT指令相关
#include "modemTest.h"

/* for input buffering */
static char s_ATBuffer[MAX_AT_RESPONSE+1];
static char *s_ATBufferCur = s_ATBuffer;
 
 int writeline (const char *s, int fd)
{
    size_t cur = 0;
    size_t len = strlen(s);
    ssize_t written;
    int atBufLen = 0;
    char *atBuf = NULL;

    if (fd < 0 ) {
        return -1;
    }
    if (s != NULL) {
        atBufLen = strlen(s);
        if (atBufLen > 0) {
            atBuf = (char*)malloc(atBufLen + 2 + 1);
        }
        if (atBuf != NULL) {
            memcpy(atBuf, s, atBufLen);
            atBuf[atBufLen] = '\r';
           atBuf[atBufLen + 1] = '\n';
           atBuf[atBufLen + 2] = '\0';
            len = atBufLen + 2 + 1+ 1;
            /* the main string */
            while (cur < len) {
                do {
                    written = write (fd, atBuf + cur, len - cur);
                } while (written < 0 && errno == EINTR);
            
                if (written < 0) {
                    printf("write cmd [%s] failed.\n", s);
                    free(atBuf);
                    return -1;
                }
                cur += written;
            }
            free(atBuf);
            if (written >= 0) {
                return 0;
            }
        } else {
            printf("%s()[%u]: malloc fail",__FUNCTION__, __LINE__);
        }
    }
    return 0;
}

char * findNextEOL(char *cur)
{
    if (cur[0] == '>' && cur[1] == ' ' && cur[2] == '\0') {
        /* SMS prompt character...not \r terminated */
        return cur+2;
    }

    // Find next newline
    while (*cur != '\0' && *cur != '\r' && *cur != '\n') cur++;

    return *cur == '\0' ? NULL : cur;
}

 char *readline(int fd)
{
    ssize_t count;

    char *p_read = NULL;
    char *p_eol = NULL;
    char *ret;

    /* this is a little odd. I use *s_ATBufferCur == 0 to
     * mean "buffer consumed completely". If it points to a character, than
     * the buffer continues until a \0
     */
    //清理全局缓存buf防止数据追加
    memset(s_ATBuffer, 0, sizeof(s_ATBuffer));
    memset(s_ATBufferCur, 0, sizeof(s_ATBufferCur));
    if (*s_ATBufferCur == '\0') {
        /* empty buffer */
        s_ATBufferCur = s_ATBuffer;
        *s_ATBufferCur = '\0';
        p_read = s_ATBuffer;
    } else {   /* *s_ATBufferCur != '\0' */
        /* there's data in the buffer from the last read */

        // skip over leading newlines
        while (*s_ATBufferCur == '\r' || *s_ATBufferCur == '\n')
            s_ATBufferCur++;

        p_eol = findNextEOL(s_ATBufferCur);

        if (p_eol == NULL) {
            /* a partial line. move it up and prepare to read more */
            size_t len;

            len = strlen(s_ATBufferCur);

            memmove(s_ATBuffer, s_ATBufferCur, len + 1);
            p_read = s_ATBuffer + len;
            s_ATBufferCur = s_ATBuffer;
        }
        /* Otherwise, (p_eol !- NULL) there is a complete line  */
        /* that will be returned the while () loop below        */
    }

    while (p_eol == NULL) {
        if (0 == MAX_AT_RESPONSE - (p_read - s_ATBuffer)) {
            printf("ERROR: Input line exceeded buffer\n");
            /* ditch buffer and start over again */
            s_ATBufferCur = s_ATBuffer;
            *s_ATBufferCur = '\0';
            p_read = s_ATBuffer;
        }

        do {
            count = read(fd, p_read, MAX_AT_RESPONSE);
        } while (count < 0 && errno == EINTR);
     
        if (count > 0) {
            p_read[count] = '\0';
            // skip over leading newlines
            while (*s_ATBufferCur == '\r' || *s_ATBufferCur == '\n')
                s_ATBufferCur++;

            p_eol = findNextEOL(s_ATBufferCur);
            p_read += count;
        } else if (count <= 0) {
            /* read error encountered or EOF reached */
            if(count == 0) {
                printf("atchannel: EOF reached");
            } else {
                printf("atchannel: read error %s", strerror(errno));
            }
            return NULL;
        }
    }
printf(">>%s", s_ATBufferCur);
    /* a full line in the buffer. Place a \0 over the \r and return */
    ret = s_ATBufferCur;
    *p_eol = '\0';
    s_ATBufferCur = p_eol + 1; /* this will always be <= p_read,    */
                              /* and there will be a \0 at *p_read */
    return ret;
}

int writeTDModemline (const char* s, int fd)
{
    size_t cur = 0;
    size_t len = strlen(s);
    ssize_t written;

    /* the main string */
    while (cur < len) {
        do {
            written = write (fd, s + cur, len - cur);
        }
        while (written < 0 && errno == EINTR);

        if (written < 0) {
            return -1;
        }
        cur += written;
    }

    /* the \r  */
    do {
        written = write (fd, "\r" , 1);
    }
    while ((written < 0 && errno == EINTR) || (written == 0));
    if (written < 0) {
        return -1;
    }
    return 0;
}