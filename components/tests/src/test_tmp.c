#include "test_tmp.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

#define TEST_TMP_ENABLE (1)
/**
 * 临时测试,需要保存的代码一定要移出去!!!!
*/
int fd = -1;
#if TEST_TMP_ENABLE
void test_tmp_init(void)
{
    #define UARTP_FRAME_SIZE (0xFFFF)
    struct
    {
        uint16_t head;
        uint16_t len;
        uint16_t crc;
        uint8_t cmd;
    }req;

    req.head = 0x2424;
    req.crc = 0xFFFF;
    int res = -1;

    uint8_t jpeg[0xFFFF] = {0};
    int jpeg_fd = -1;
    jpeg_fd = open("me2.jpg", O_RDWR);
    if (jpeg_fd < 0)
    {
        perror("open me2.jpg");
    }
    else
    {
        int len = read(jpeg_fd, jpeg, sizeof(jpeg));
        if (len > 0)
        {
            req.len = 7 + len;
            req.cmd = 0x16;
            _INFO_HEX((uint8_t *)&req, 7);
            _INFO_HEX(jpeg, len);
        }
        close(jpeg_fd);
    }

    exit(1);

    fd = open("/dev/ttyS24", O_RDWR | O_NONBLOCK);
    if (fd < 0)
    {
        perror("open send to uartp");
    }
    else
    {
        res = write(fd, &req, sizeof(req));
        if (res < 0)    {perror("write");}
    }


}
#else
void test_tmp_init(void){}
#endif

#if TEST_TMP_ENABLE
void test_tmp_loop(void)
{
    // write
    int res = -1;
    uint8_t req[] = {0x24, 0x24, 0x07, 0x00, 0xFF, 0xFF, 0x00};
    static int count = 0;
    if (count ++ > 30)
    {
        printf("write\n");
        count = 0;
        res = write(fd, &req, sizeof(req));
        if (res < 0)    {perror("write");}
    }

    // read
    static fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 1000 * 10;
    res = select(fd + 1, &fds, NULL, NULL, &time);
    if (res < 0)
    {
        perror("select err");
    }
    else if (FD_ISSET(fd, &fds))
    {
        uint8_t buff[0xFFFF];
        int len = 0;
        printf("read\n");
        len = read(fd, buff, sizeof(buff));
        if (len > 0)
        {
            printf("read\n");
            _INFO_HEX(buff, len);
        }
        else if (len == 0)
        {
            printf("nothing\n");
        }
        else if (len < 0)
        {
            perror("read");
        }
    }
    usleep(1000 * 100);
}
#else
void test_tmp_loop(void){}
#endif