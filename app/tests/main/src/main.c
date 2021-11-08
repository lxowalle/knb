#include <stdio.h>
#include "test.h"
#include "stdint.h"
#include "string.h"
#include "mf_door.h"
#include "sqlite3.h"
#include "stdlib.h"

static inline void _INFO_HEX(uint8_t *data, int size)
{
    printf("\033[32m[INFO]hex(%d):\033[0m", size);
    for (int i = 0; i < size; i ++)
    {
        printf("%.2X ", data[i]);
    }
    printf("\n");
}

static sqlite3 *db = NULL;
typedef struct
{
    uint32_t id;
    uint32_t start_time;
    uint32_t end_time;
    char token[256];
    char note[50];
}qrcode_table_member_t;

#define DOOR_DB_TABLE_NAME   "rfid_door"          // 数据库表名

#define RFID_TAB_NAME_LEN       (16)
#define RFID_TAB_UID_LEN        (16)
#define RFID_TAB_KEY_LEN        (6)
#define RFID_TAB_NOTE_LEN       (50)

typedef struct
{
    uint32_t id;
    uint8_t key[RFID_TAB_KEY_LEN];
    uint8_t uid[RFID_TAB_UID_LEN];
}rfid_table_member_t;

/**
 * @brief 新增一行。id会自动递增
*/
static int db_insert(void *database, rfid_table_member_t *param)
{
    sqlite3 *db = (sqlite3 *)database;
    rfid_table_member_t *item = (rfid_table_member_t *)param;
    sqlite3_stmt *stmt = NULL;
    int ret = -1;
    char sql[sizeof(rfid_table_member_t) + 256] = {0};
    
    if (database == NULL || param == NULL)
        return -1;

    snprintf(sql, sizeof(sql), "INSERT INTO " DOOR_DB_TABLE_NAME"(key,uid)"
    " VALUES(?,?);");

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB perpare error: %d\n", ret);
        return -1;
    }

    ret |= sqlite3_bind_blob(stmt, 1, item->key, sizeof(item->key), SQLITE_STATIC);
    ret |= sqlite3_bind_blob(stmt, 2, item->uid, sizeof(item->uid), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB bind error:%d\n", ret);
        sqlite3_finalize(stmt);
        return -1;
    }

    ret = sqlite3_step(stmt);
    if (ret != SQLITE_DONE)
    {
        fprintf(stderr, "DB step error:%s   %d\n", sqlite3_errmsg(db), ret);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    printf("rfid insert database table\n");

    return 0;
}

/**
 * @brief 通过id删除一行
*/
static int db_delete(void *database, int id)
{
    sqlite3 *db = (sqlite3 *)database;  

    if (database == NULL)
        return -1;

    int ret = -1;
    char *err_msg = NULL;
    char sql[256] = {0};

    int tbl_id = id;

    if (tbl_id < 0)
    {
        // ret = delete_db_table(db);
        // if (!ret)
        //     create_db_table(db);
    }
    else
    {
        snprintf(sql, sizeof(sql), "DELETE FROM " DOOR_DB_TABLE_NAME " WHERE id = %d;", tbl_id);

        ret = sqlite3_exec(db, sql, NULL, 0, &err_msg);
        if (ret != SQLITE_OK)
        {
            fprintf(stderr, "DB error: %s %d\n", err_msg, ret);
            sqlite3_free(err_msg);
            return -1;
        }
    }

    printf("rfid delete database table\n");

    return 0;
}

/**
 * @brief 通过键值对查询该行的所有数据
*/
static int db_select(void *database, mf_door_keyval_t *keyval, rfid_table_member_t *param)
{
    sqlite3 *db = (sqlite3 *)database;
    rfid_table_member_t *member = (rfid_table_member_t *)param;
    sqlite3_stmt *stmt = NULL;
    int ret = -1;
    int has_item = 0, need_bind_blob = 0, blob_len = 0;
    char sql[sizeof(rfid_table_member_t) + 256] = {0};

    if (database == NULL || keyval == NULL || param == NULL)
        return -1;

    if (!strcmp(keyval->key, "id"))
    {
        snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = %d;", keyval->key, atoi(keyval->value));
    }
    else if(!strcmp(keyval->key, "key"))
    {
        snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = ?;", keyval->key);
        need_bind_blob = 1;
        blob_len = RFID_TAB_KEY_LEN;
    }

    // db prepare
    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB prepare v2 failed!ret = %d\n", ret);
        return -1;
    }

    // db bind
    if (need_bind_blob)
    {
        ret = sqlite3_bind_blob(stmt, 1, keyval->value, blob_len, SQLITE_STATIC);
        if (ret != SQLITE_OK)
        {
            fprintf(stderr, "DB bind failed!ret = %d\n", ret);
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    // db step
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int col = 0;
        int len = 0;
        char *str = NULL;

        member->id = sqlite3_column_int(stmt, col);
        col ++;

        len = sqlite3_column_bytes(stmt, col);
        str = (char *)sqlite3_column_blob(stmt, col);
        memcpy(member->key, (const char *)str, len);
        col ++;

        has_item = 1;
        break;  // Only select first item
    }

    sqlite3_finalize(stmt);

#if 0
    printf("id:%d\n", member->id);
    _INFO_HEX(member->key, sizeof(member->key));
#endif
    if (has_item)
        return 0;
    else
        return -1;
}

/**
 * @brief 通过id索引，并更新该行数据
*/
static int db_update(void *database, int id,  mf_door_keyval_t *update)
{
    sqlite3 *db = (sqlite3 *)database;
    sqlite3_stmt *stmt = NULL;
    int ret = -1, need_bind_blob = 0, blob_len = 0;
    char sql[sizeof(rfid_table_member_t) + 256] = {0};
    mf_door_keyval_t *dst = (mf_door_keyval_t *)update;

    if (database == NULL || update == NULL)
        return -1;

    if (!strcmp(dst->key, "id"))
    {
        snprintf(sql, sizeof(sql), "UPDATE " DOOR_DB_TABLE_NAME " SET %s = %d WHERE id = %d;", dst->key, atoi(dst->value), id);
    }
    else if (!strcmp(dst->key, "key"))
    {
        snprintf(sql, sizeof(sql), "UPDATE " DOOR_DB_TABLE_NAME " SET %s = ? WHERE id = %d;", dst->key, id);
        need_bind_blob = 1;
        blob_len = RFID_TAB_KEY_LEN;
    }

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB prepare v2 failed!ret = %d\n", ret);
        return -1;
    }

    if (need_bind_blob)
    {
        ret = sqlite3_bind_blob(stmt, 1, dst->value, blob_len, SQLITE_STATIC);
        if (ret != SQLITE_OK)
        {
            fprintf(stderr, "DB bind failed!ret = %d\n", ret);
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    ret = sqlite3_step(stmt);
    if(ret != SQLITE_DONE)
    {
        printf("DB step fail!ret:%d\n", ret);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}


static sqlite3_stmt *s_iterate_stmt = NULL;

/**
 * @brief 迭代获取数据，准备函数
*/
static int db_iterate_prepare(void)
{
    int ret = 0;
    char sql[256];

    snprintf(sql, sizeof(sql),"SELECT id,key FROM " DOOR_DB_TABLE_NAME ";");

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &s_iterate_stmt, NULL);
    if(ret != SQLITE_OK){printf("DB prepare fail: %d\n", ret);return -1;}

    return 0;
}

/**
 * @brief 迭代获取数据，执行该函数前必须执行准备函数db_iterate_prepare
 * @return -1 参数错误 -2 迭代完成 >0 返回行的id值
*/
static int db_iterate(rfid_table_member_t *member)
{
    if (s_iterate_stmt == NULL || member == NULL)
        return -1;

    rfid_table_member_t *item = (rfid_table_member_t *)member;
    int ret = 0, id = 0;

    ret = sqlite3_step(s_iterate_stmt);
    if (ret == SQLITE_ROW)
    {
        int len = 0, cnt = 0;
        uint8_t *str;
        id = sqlite3_column_int(s_iterate_stmt, cnt);
        member->id = id;
        cnt ++;

        len = sqlite3_column_bytes(s_iterate_stmt, cnt);
        str = (uint8_t *)sqlite3_column_blob(s_iterate_stmt, cnt);
        memcpy(item->key, str, len);
        cnt ++;

        len = sqlite3_column_bytes(s_iterate_stmt, cnt);
        str = (uint8_t *)sqlite3_column_blob(s_iterate_stmt, cnt);
        memcpy(item->uid, str, len);
        cnt ++;
    }
    else if (ret == SQLITE_DONE)
    {
        sqlite3_finalize(s_iterate_stmt);
        s_iterate_stmt = NULL;
        return -2;
    }

    return id;
}

/**
 * @brief 打印数据表
*/
static int db_list_tab(void)
{
    int ret = 0;
    ret |= db_iterate_prepare();

    rfid_table_member_t member = {0};
    while (db_iterate(&member) > 0)
    {
        printf("id:%d ", member.id);
        _INFO_HEX((uint8_t *)member.key, sizeof(member.key));
        _INFO_HEX((uint8_t *)member.uid, sizeof(member.uid));
    }

    return ret;
}

int main()
{
    // printf("main\n");
    mf_door_init("door.sqlite");

    mf_door_config_t *door_cfg = mf_door_get_cfg();
    db = door_cfg->db;

    mf_door_t *door = mf_door_get(MF_DOOR_RFID);
    if (door != NULL)
    {
        int ret = 0;

        // add
        {
            printf("====>Insert\n");
            rfid_table_member_t mem = {0};
            memset(mem.key, 0x39, sizeof(mem.key));
            memset(mem.uid, 0x39, sizeof(mem.uid));
            mem.id = 299;
            ret = db_insert(door->db, &mem);

            memset(mem.uid, 0x17, sizeof(mem.uid));
            memset(mem.key, 0x17, sizeof(mem.key));
            ret = db_insert(door->db, &mem);
            if (ret){printf("error %d!!!\n\n", __LINE__);}
            printf("Insert result\n");
            db_list_tab();
            printf("================================\n");
        }
        
        // update
        {
            printf("====>Update\n");
            mf_door_keyval_t update = {
                .key = "key"
            };
            memset(update.value, 0x99, sizeof(update.value));
            ret = db_update(door->db, 1, &update);

            mf_door_keyval_t update2 = {
                .key = "id",
                .value = "299"
            };
            ret = db_update(door->db, 1, &update2);
            printf("Update result\n");
            db_list_tab();
            printf("================================\n");
        }

        // select
        {
            printf("====>Select\n");
            rfid_table_member_t mem = {0};
            mf_door_keyval_t val = 
            {
                .key = "id",
                .value = "299"
            };
            memset(&mem, 0, sizeof(mem));
            ret = db_select(door->db, &val, &mem);
            printf("Select result\n");
            printf("Select mem=>id:%d", mem.id);
            _INFO_HEX((uint8_t *)mem.key, sizeof(mem.key));
            db_list_tab();
            printf("================================\n");
        }

        // delete
        {
            // printf("====>Delete\n");
            // ret = db_delete(door->db, 1);
            // printf("Delete result\n");
            // db_list_tab();
            // printf("================================\n");
        }
        // select
    }

    // mf_door_deinit();

    return 0;
}

