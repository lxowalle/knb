#include "mf_door.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DOOR_DB_TABLE_NAME   "qrcode_door"          // 数据库表名

typedef struct
{
    mf_door_t base;
}mf_door_qrcode_t;

static int create_db_table(void *database)
{
    struct sqlite3 *db = (struct sqlite3 *)database;  
    int res = -1;
    char *err_msg = NULL;

    char *sql = "CREATE TABLE IF NOT EXISTS " DOOR_DB_TABLE_NAME "("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "start_time INTEGER NOT NULL,"
                "end_time INTEGER NOT NULL,"
                "token BLOB NOT NULL,"
                "note TEXT"
                ");";

    res = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (res != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, res);
        sqlite3_free(err_msg);
        return -1;
    }

    printf("qrcode create database table\n");

    return 0;
}

static int delete_db_table(void *database)
{
    struct sqlite3 *db = (struct sqlite3 *)database;  
    int res = -1;
    char *err_msg = NULL;

    char *sql = "DROP TABLE " DOOR_DB_TABLE_NAME ";";    // drop table DOOR_DB_TABLE_NAME;

    res = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (res != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, res);
        sqlite3_free(err_msg);
        return -1;
    }

    printf("qrcode delete database table\n");

    return 0;
}

static int insert_db_table(void *database, void *param)
{
    struct sqlite3 *db = (struct sqlite3 *)database;  
    int res = -1;
    char *err_msg = NULL;

    char sql[256] = {0};
    char *tbl_name = "none";
    uint32_t tbl_start_time = 1;
    uint32_t tbl_end_time = 2000;
    char *tbl_token = "123";
    char *tbl_note = "none note";
    snprintf(sql, sizeof(sql), "INSERT INTO " DOOR_DB_TABLE_NAME"(name,start_time,end_time,token,note)"
    " VALUES('%s',%d,%d,'%s','%s');", tbl_name, tbl_start_time, tbl_end_time, tbl_token, tbl_note);

    res = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (res != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, res);
        sqlite3_free(err_msg);
        return -1;
    }

    printf("qrcode insert database table\n");

    return 0;
}

static int qrcode_door_init(mf_door_config_t *door_config)
{
    mf_door_config_t *door_cfg = (mf_door_config_t *)door_config;
    int res = -1;
    printf("qrcode door init\n");

    /* Create database table */
    res = create_db_table(door_cfg->db);
    if (res < 0)
    {
        printf("Create database table failed!res:%d\n", res);
        return -1;
    }

    return 0;
}

static int qrcode_door_deinit(mf_door_config_t *door_config)
{
    mf_door_config_t *door_cfg = (mf_door_config_t *)door_config;
    int res = -1;
    printf("qrcode door deinit\n");

    /* Delete database table */
    // res = delete_db_table(door_cfg->db);
    // if (res < 0)
    // {
    //     printf("Create database table failed!res:%d\n", res);
    //     return -1;
    // }

    return 0;
}

static int qrcode_open_door(void)
{
    printf("qrcode open the door\n");
    return 0;
}

static int qrcode_close_door(void)
{
    printf("qrcode close the door\n");

    return 0;
}

static int qrcode_auto_adjust_door(int open, int ms)
{
    printf("qrcode auto adjust door\n");

    return 0;
}

static int qrcode_insert_password(mf_door_config_t *door_config, void *param)
{
    mf_door_config_t *door_cfg = (mf_door_config_t *)door_config;
    printf("qrcode insert password\n");

    insert_db_table(door_cfg->db);

    return 0;
}

static int qrcode_select_password(void *param)
{
    printf("qrcode select password\n");
    return 0;
}

static int qrcode_delete_password(void *param)
{
    printf("qrcode delete password\n");
    return 0;
}

static int qrcode_encoding(void *in, void *out)
{
    printf("qrcode encoding\n");
    return 0;
}

static int qrcode_decoding(void *in, void *out)
{
    printf("qrcode decoding\n");
    return 0;
}

static int qrcode_control(void *param)
{
    printf("qrcode control\n");
    return 0;
}

static mf_door_op_t door_qrcode_op = 
{
    qrcode_door_init,
    qrcode_door_deinit,
    qrcode_open_door,
    qrcode_close_door,
    qrcode_auto_adjust_door,
    qrcode_insert_password,
    qrcode_select_password,
    qrcode_delete_password,
    qrcode_encoding,
    qrcode_decoding,
    qrcode_control
};

mf_door_t *door_qrcode_create(void)
{
    mf_door_qrcode_t    *mdq;

    mdq = (mf_door_qrcode_t *)calloc(1, sizeof(mf_door_qrcode_t));
    if (NULL != mdq)
    {
        mdq->base.type = MF_DOOR_QRCODE;
        mdq->base.op = &door_qrcode_op;
    }
    else
    {
        return NULL;
    }

    return (mf_door_t *)mdq;
}