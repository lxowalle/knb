#include "pjsua_app.h"

#define THIS_FILE	"main.c"

static pj_bool_t	    running = PJ_TRUE;
static pj_status_t	    receive_end_sig;
static pj_thread_t	    *sig_thread;
static pjsua_app_cfg_t	    cfg;

/* Called when CLI (re)started */
void on_app_started(pj_status_t status, const char *msg)
{
    pj_perror(3, THIS_FILE, status, (msg)?msg:"");
}

void on_app_stopped(pj_bool_t restart, int argc, char** argv)
{
    if (argv) {
	cfg.argc = argc;
	cfg.argv = argv;
    }

    running = restart;
}

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
static void print_stack(int sig)
{
    void *array[16];
    size_t size;

    size = backtrace(array, 16);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    receive_end_sig = 1;
    exit(1);
}

static void setup_socket_signal()
{
    signal(SIGPIPE, SIG_IGN);
}

static void setup_signal_handler(void)
{
    signal(SIGSEGV, &print_stack);
    signal(SIGABRT, &print_stack);
}

int main(int argc, char *argv[])
{
    setup_signal_handler();
    setup_socket_signal();

    /** Init */
    if (mf_pjsip_init())
    {
        printf("pjsip init error!\n");
    }

    

    /**  Deinit */
    mf_pjsip_deinit();
    return 0;
}
