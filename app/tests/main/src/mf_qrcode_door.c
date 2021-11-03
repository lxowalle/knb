#include "mf_door.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DOOR_DB_TABLE_NAME   "qrcode_door"          // 数据库表名

#define QRCODE_TAB_NAME_LEN     (20)
#define QRCODE_TAB_TOKEN_LEN    (256)
#define QRCODE_TAB_NOTE_LEN     (50)
typedef struct
{
    uint32_t id;
    char name[QRCODE_TAB_NAME_LEN];
    uint32_t start_time;
    uint32_t end_time;
    char token[QRCODE_TAB_TOKEN_LEN];
    char note[QRCODE_TAB_NOTE_LEN];
}qrcode_table_member_t;

typedef struct
{
    mf_door_t base;
    qrcode_table_member_t db_tbl;
}mf_door_qrcode_t;

static int create_db_table(void *database)
{
    sqlite3 *db = (sqlite3 *)database;  
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

static int insert_db_table_record(void *database, void *param)
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

static int select_db_table_record(mf_door_t *door, void *param)
{
    mf_door_qrcode_t *qd = (mf_door_qrcode_t *)door;
    sqlite3 *db = (sqlite3 *)qd->base.db;
    qrcode_table_member_t *member = &qd->db_tbl;
    sqlite3_stmt *stmt = NULL;
    int res = -1;
    char *err_msg = NULL;
    char sql[sizeof(qrcode_table_member_t) + 256] = {0};
    char *key = "id";
    int value = 2;
    snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = %d;", key, value);

    res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (res != SQLITE_OK)
    {
        fprintf(stderr, "DB prepare v2 failed!res = %d\n", res);
        return -1;
    }

    // res |= sqlite3_bind_blob(stmt, 1, qd->db_tbl.id, sizeof(qd->db_tbl.id), SQLITE_STATIC);
    // res |= sqlite3_bind_int(stmt, 2, qd->db_tbl.start_time);
    // res |= sqlite3_bind_int(stmt, 3, qd->db_tbl.end_time);
    // res |= sqlite3_bind_blob(stmt, 4, qd->db_tbl.token, sizeof(qd->db_tbl.token), SQLITE_STATIC);
    // if (res != SQLITE_OK)
    // {
    //     fprintf(stderr, "DB bind failed!res = %d\n", res);
    //     sqlite3_finalize(stmt);
    //     return -1;
    // }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int col = 0;
        int len = 0;
        char *str = NULL;

        member->id = sqlite3_column_int(stmt, col ++);

        len = sqlite3_column_bytes(stmt, col);
        str = sqlite3_column_blob(stmt, col);
        memcpy(member->name, str, len);
        col ++;

        member->start_time = sqlite3_column_int(stmt, col ++);
        member->end_time = sqlite3_column_int(stmt, col ++);

        len = sqlite3_column_bytes(stmt, col);
        str = sqlite3_column_blob(stmt, col);
        memcpy(member->token, str, len);
        col ++;

        len = sqlite3_column_bytes(stmt, col);
        str = sqlite3_column_blob(stmt, col);
        memcpy(member->note, str, len);
        col ++;
    }

    sqlite3_finalize(stmt);

#if 1
    printf("id:%d name:%s start_time:%d end_time:%d token:%s note:%s\n", 
    member->id, member->name, member->start_time, member->end_time, member->token, member->note);
#endif
    return 0;
}

static int delete_db_table_record(void *database, void *param)
{
    struct sqlite3 *db = (struct sqlite3 *)database;  
    int res = -1;
    char *err_msg = NULL;
    char sql[256] = {0};

    uint32_t tbl_id = 3;

    snprintf(sql, sizeof(sql), "DELETE FROM " DOOR_DB_TABLE_NAME " WHERE id = %d;", tbl_id);

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

static int qrcode_door_init(mf_door_t *door)
{
    mf_door_qrcode_t *dq = (mf_door_qrcode_t *)door;
    int res = -1;
    printf("qrcode door init\n");
 
    res = create_db_table(dq->base.db);
    if (res < 0)
    {
        printf("Create database table failed!res:%d\n", res);
        return -1;
    }

    return 0;
}

static int qrcode_door_deinit(mf_door_t *door)
{
    mf_door_qrcode_t *dq = (mf_door_qrcode_t *)door;
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

static int qrcode_insert_password(mf_door_t *door, void *param)
{
    mf_door_qrcode_t *dq = (mf_door_qrcode_t *)door;
    printf("qrcode insert password\n");

    insert_db_table_record(dq->base.db, NULL);

    return 0;
}

static int qrcode_select_password(mf_door_t *door, void *param)
{
    mf_door_qrcode_t *dq = (mf_door_qrcode_t *)door;
    printf("qrcode select password\n");

    select_db_table_record(dq, NULL);
    return 0;
}

static int qrcode_delete_password(mf_door_t *door, void *param)
{
    mf_door_qrcode_t *dq = (mf_door_qrcode_t *)door;
    printf("qrcode delete password\n");

    delete_db_table_record(dq->base.db, NULL);
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