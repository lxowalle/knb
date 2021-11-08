#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "mf_sqlite.h"

#define THIS_FILE	"main.c"

#define _TEST(a, fmt, ...)\
do{\
    if (a){\
        printf("\033[31m[ ERROR ] \033[0m"fmt, ##__VA_ARGS__);}\
    else{\
        printf("\033[32m[  OK   ] \033[0m"fmt, ##__VA_ARGS__);\
    }\
}while(0)

static int exit_flag = 0;

static void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        exit_flag = 1;
    }
}

static void setup_signal_handler(void)
{
    signal(SIGINT, &signal_handler);
    signal(SIGABRT, &signal_handler);
}


int main(int argc, char *argv[])
{
    setup_signal_handler();

#define TEST_DB_NAME    "test.db"
#define TEST_TABLE_NAME "tab1"

    // --创建数据库
    sqlite3 *db;
    mf_sqlite_create_database(TEST_DB_NAME, &db);

    // --创建一张表
    char *tab = "key BLOB NOT NULL, uid BLOB NOT NULL";
    mf_sqlite_create_table(db, TEST_TABLE_NAME, tab);

    int tab_cnt = mf_sqlite_check_table(db, TEST_TABLE_NAME);
    printf("table count :%d\n", tab_cnt);

    int member_cnt = mf_sqlite_check_member(db, TEST_TABLE_NAME, "uid");
    printf("member count :%d\n", member_cnt);

    int member_type_cnt = mf_sqlite_check_member_type(db, TEST_TABLE_NAME, "uid", "blo");
    printf("member type count :%d\n", member_type_cnt);

    // --增加一行
    char *data[] = {"uid", "blob", "123",
                    "key", "blob", "keyaa"};
    mf_sqlite_insert(db, TEST_TABLE_NAME, data, 2);

    // --删除一张表
    // mf_sqlite_delete_table(db, "tab1");
}
