#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mf_pjsip.h"

#define THIS_FILE	"main.c"

static void signal_handler(int sig)
{

}

static void setup_signal_handler(void)
{
    signal(SIGSEGV, &signal_handler);
    signal(SIGABRT, &signal_handler);
}

int main(int argc, char *argv[])
{
    setup_signal_handler();

    /** Init */
    if (mf_pjsip_init())
    {
        printf("pjsip init error!\n");
    }

    

    /**  Deinit */
    mf_pjsip_deinit();
    return 0;
}
