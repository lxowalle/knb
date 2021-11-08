#include "mf_door.h"
// #include "mf_rfid.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define DOOR_DB_TABLE_NAME   "rfid_door"          // 数据库表名

#define RFID_TAB_NAME_LEN       (16)
#define RFID_TAB_UID_LEN        (16)
#define RFID_TAB_KEY_LEN        (6)
#define RFID_TAB_NOTE_LEN       (50)

/** Database */
typedef struct
{
    uint32_t id;
    char key[RFID_TAB_KEY_LEN];
}rfid_table_member_t;

typedef struct
{
    mf_door_t base;
    rfid_table_member_t db_tbl;
    uint8_t auto_open_door;
}mf_door_rfid_t;

static int create_db_table(void *database)
{
    sqlite3 *db = (sqlite3 *)database;  
    int ret = -1;
    char *err_msg = NULL;

    char *sql = "CREATE TABLE IF NOT EXISTS " DOOR_DB_TABLE_NAME "("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "key BLOB NOT NULL,"
                "uid BLOB NOT NULL"
                ");";

    ret = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, ret);
        sqlite3_free(err_msg);
        return -1;
    }

    printf("rfid create database table\n");

    return 0;
}

static int delete_db_table(void *database)
{
    struct sqlite3 *db = (struct sqlite3 *)database;  
    int ret = -1;
    char *err_msg = NULL;

    char *sql = "DROP TABLE " DOOR_DB_TABLE_NAME ";";    // drop table DOOR_DB_TABLE_NAME;

    ret = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, ret);
        sqlite3_free(err_msg);
        return -1;
    }

    printf("rfid delete database table\n");

    return 0;
}

static int db_insert(void *database, rfid_table_member_t *param)
{
    sqlite3 *db = (sqlite3 *)database;
    rfid_table_member_t *item = (rfid_table_member_t *)param;
    sqlite3_stmt *stmt = NULL;
    int ret = -1;
    char *err_msg = NULL;
    char sql[sizeof(rfid_table_member_t) + 256] = {0};
    
    snprintf(sql, sizeof(sql), "INSERT INTO " DOOR_DB_TABLE_NAME"(key)"
    " VALUES(?);");
    printf("insert sql:%s\n", sql);
    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB perpare error: %d\n", ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 1, item->key, sizeof(item->key), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB bind error:%d\n", ret);
        sqlite3_finalize(stmt);
        return -1;
    }

    ret = sqlite3_step(stmt);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB step error:%s   %d\n", sqlite3_errmsg(db), ret);
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    printf("rfid insert database table\n");

    return 0;
}

static int db_delete(void *database, int id)
{
    sqlite3 *db = (sqlite3 *)database;  

    int ret = -1;
    char *err_msg = NULL;
    char sql[256] = {0};

    int tbl_id = id;

    if (tbl_id < 0)
    {
        ret = delete_db_table(db);
        if (!ret)
            create_db_table(db);
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

static int db_select(void *database, char *key, void *value, rfid_table_member_t *param)
{
    sqlite3 *db = (sqlite3 *)database;
    rfid_table_member_t *member = (rfid_table_member_t *)param;
    sqlite3_stmt *stmt = NULL;
    int ret = -1;
    int has_item = 0;
    char sql[sizeof(rfid_table_member_t) + 256] = {0};

    if (!strcmp(key, "id"))
    {
        snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = %d;", key, (int)value);
    }
    else if (!strcmp(key, "name"))
    {
        snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = '%s';", key, (char *)value);
    }
    else if (!strcmp(key, "note"))
    {
        snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = '%s';", key, (char *)value);
    }
    else
    {
        snprintf(sql, sizeof(sql), "SELECT * FROM " DOOR_DB_TABLE_NAME " WHERE %s = ?;", key);
    }
        
    // db prepare
    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB prepare v2 failed!ret = %d\n", ret);
        return -1;
    }

    // db bind
    if (!strcmp(key, "uid"))
    {
        ret = sqlite3_bind_blob(stmt, 1, value, RFID_TAB_UID_LEN, SQLITE_STATIC);
    }
    else if (!strcmp(key, "key"))
    {
        ret = sqlite3_bind_blob(stmt, 1, value, RFID_TAB_KEY_LEN, SQLITE_STATIC);
    }

    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB bind failed!ret = %d\n", ret);
        sqlite3_finalize(stmt);
        return -1;
    }

    // db step
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int col = 0;
        int len = 0;
        char *str = NULL;

        member->id = sqlite3_column_int(stmt, col);
        col ++;

        // len = sqlite3_column_bytes(stmt, col);
        // str = (char *)sqlite3_column_blob(stmt, col);
        // memcpy(member->uid, str, len);
        // col ++;

        // len = sqlite3_column_bytes(stmt, col);
        // str = (char *)sqlite3_column_text(stmt, col);
        // memcpy(member->name, str, len);
        // col ++;

        len = sqlite3_column_bytes(stmt, col);
        str = (char *)sqlite3_column_blob(stmt, col);
        memcpy(member->key, (const char *)str, len);
        col ++;

        // len = sqlite3_column_bytes(stmt, col);
        // str = (char *)sqlite3_column_text(stmt, col);
        // memcpy(member->note, str, len);
        // col ++;

        has_item = 1;
        break;  // Onlu select first item
    }

    sqlite3_finalize(stmt);

#if 1
    // printf("id:%d name:%s key:%s note:%s\n", 
    // member->id, member->name, member->key, member->note);
#endif
    if (has_item)
        return 0;
    else
        return -1;
}

static int db_update(void *database, int id,  mf_door_keyval_t *update)
{
    sqlite3 *db = (sqlite3 *)database;
    sqlite3_stmt *stmt = NULL;
    int ret = -1, has_item = 0, need_bind_blob = 0, blob_len = 0;
    char sql[sizeof(rfid_table_member_t) + 256] = {0};
    mf_door_keyval_t *dst = (mf_door_keyval_t *)update;

    if (!strcmp(dst->key, "id"))
    {
        snprintf(sql, sizeof(sql), "UPDATE " DOOR_DB_TABLE_NAME " SET %s = %d WHERE id = %d;", dst->key, (int)dst->value, id);
    }
    else if (!strcmp(dst->key, "name") || !strcmp(dst->key, "note"))
    {
        snprintf(sql, sizeof(sql), "UPDATE " DOOR_DB_TABLE_NAME " SET %s = '%s' WHERE id = %d;", dst->key, (char *)dst->value, id);
    }
    else if (!strcmp(dst->key, "uid"))
    {
        snprintf(sql, sizeof(sql), "UPDATE " DOOR_DB_TABLE_NAME " SET %s = ? WHERE id = %d;", dst->key, id);
        need_bind_blob = 1;
        blob_len = RFID_TAB_UID_LEN;
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

static int rfid_door_init(mf_door_t *door)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    int ret = -1;
    printf("rfid door init\n");
 
    // mf_brd_spi_t spi_cfg = 
    // {
    //   .bits = 8,
    //   .speed = 500000
    // };
    // Pcd_io_init(&spi_cfg);
    // PcdReset();
    // M500PcdConfigISOType('A');

    ret = create_db_table(dq->base.db);
    if (ret < 0)
    {
        printf("Create database table failed!ret:%d\n", ret);
        return -1;
    }

    return 0;
}

static int rfid_door_deinit(mf_door_t *door)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    int ret = -1;
    printf("rfid door deinit\n");

    /* Delete database table */
    ret = delete_db_table(dq->base.db);
    if (ret < 0)
    {
        printf("Create database table failed!ret:%d\n", ret);
        return -1;
    }

    return 0;
}

static void rfid_loop(void)
{
	// /* 扫描卡片
	// 	如果读到了卡
	// 		读出第1个扇区的内容
	// 		然后遍历UID，对比是否为存储在模块的人员
	// 		如果是就打开门。并上传到云端
	// 	未读到卡片则继续
	// */
	// uint8_t stat, id[4], uid_in_card[16];
	// const uint8_t keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	// stat = PcdRequest(PICC_REQALL, id);

	// if(MI_OK == stat) {
	// 	if(MI_OK == PcdAnticoll(id)) {
	// 		if(MI_OK == PcdSelect(id)) {
	// 			printf("card ID: %02X %02X %02X %02X\r\n", id[0], id[1], id[2], id[3]);
	// 			if(MI_OK == PcdAuthState(PICC_AUTHENT1A, 0x01, keyA, id)) {
	// 				if(MI_OK == PcdRead(0x01, uid_in_card)) {
	// 					printf("read: ");
	// 					for(int i = 0; i < 16; i++) {
	// 						printf("%02X, ", uid_in_card[i]);
	// 					}
	// 					printf("\r\n");
	// 				}
	// 			} else {
	// 				printf("card auth failed\r\n");
	// 			}

	// 			PcdHalt();
	// 		} else {
	// 			printf("select card failed\r\n");
	// 		}
	// 	} else {
	// 		printf("Anticoll failed\r\n");
	// 	}
	// }
}

static int rfid_door_loop(mf_door_t *door)
{
    int ret = 0;

    rfid_loop();

    return ret;
}

static int rfid_doot_control(void *param)
{
    printf("rfid control\n");
    return 0;
}

static int rfid_open_door(void)
{
    printf("rfid open the door\n");
    // mf_brd.set_relay(1, 0);
    return 0;
}

static int rfid_close_door(void)
{
    printf("rfid close the door\n");
    // mf_brd.set_relay(0, 0);
    return 0;
}

static int rfid_auto_adjust_door(int open, int ms)
{
    printf("rfid auto adjust door\n");
    // mf_brd.set_relay(open, ms);
    return 0;
}

// char item[][] = {"key", "type", "value", "key", "type", "value"};

static int rfid_insert_password(mf_door_t *door, char param[][15], int num)
{
    // printf("rfid insert password\n");
    // char **par = (char **)param;

    // mf_door_db_item_t item = {0};

    // for (int i = 0; i < num; i ++)
    // {
    //     // get key
    //     // get type
    //     // get value
    // }


    // struct timeval time;
    // gettimeofday(&time, NULL);

    // if (!strcmp(key, "uid"))
    // {
    //     memset(item.uid, uid, sizeof(item.uid));
    //     strcpy(item.name, "rfid door");
    //     item.reg_time = time.tv_sec;
    //     item.start_time = -1;
    //     item.end_time = -1;
    //     memset(item.key, 0xFF, sizeof(item.key));
        
    //     strcpy(item.note, "rfid door");
    // }

    return 0;
}

static int rfid_select_password(mf_door_t *door, void *param)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    printf("rfid select password\n");

    // select_db_table_record(&dq->base, NULL);
    return 0;
}

static int rfid_delete_password(mf_door_t *door, void *param)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    printf("rfid delete password\n");


    return 0;
}

static int rfid_encoding(void *in, void *out)
{
    printf("rfid encoding\n");
    return 0;
}

static int rfid_decoding(void *in, void *out)
{
    printf("rfid decoding\n");
    return 0;
}


typedef enum
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_INT,
    ITEM_TYPE_STRING,
    ITEM_TYPE_HEXARRAY
};

typedef enum
{
    ITEM_KEY_ID,
    ITEM_KEY_UID,
    ITEM_KEY_NAME,
    ITEM_KEY_KEY,
    ITEM_KEY_NOTE,
};

static int parse_key(char *key)
{
    if (!strcmp("id", key))  // id
    {
        return ITEM_KEY_ID;
    }
    else if (!strcmp("uid", key))
    {
        return ITEM_KEY_UID;
    }
    else if (!strcmp("name", key))
    {
        return ITEM_KEY_NAME;
    }
    else if (!strcmp("key", key))
    {
        return ITEM_KEY_KEY;
    }
    else if (!strcmp("note", key))
    {
        return ITEM_KEY_NOTE;
    }
}

static int parse_item_char(mf_door_keyval_t *key_value, int num, rfid_table_member_t* member)
{
    rfid_table_member_t *item = (rfid_table_member_t *)member;
    mf_door_keyval_t *val = (mf_door_keyval_t *)key_value;
    for (int i = 0; i < num; i ++)
    {
        /* get type */ 
        int type = ITEM_TYPE_NONE;
        if (!strcmp("int", val[i].type))
        {
            type = ITEM_TYPE_INT;
        }
        else if (!strcmp("string", val[i].type))
        {
            type = ITEM_TYPE_STRING;
        }
        else if (!strcmp("hex", val[i].type))
        {
            type = ITEM_TYPE_HEXARRAY;
        }
        else
        {
            type = ITEM_TYPE_NONE;
        }

        /* set value */
        if (!strcmp("id", val[i].key) && type == ITEM_TYPE_INT)  // id
        {
            item->id = atoi(val[i].value);
        }
        // else if (!strcmp("uid", val[i].key) && type == ITEM_TYPE_HEXARRAY)
        // {
        //     memcpy(item->uid, val[i].value, sizeof(item->uid));
        // }
        // else if (!strcmp("name", val[i].key) && type == ITEM_TYPE_STRING)
        // {
        //     memcpy(item->name, val[i].value, sizeof(item->name));
        //     item->name[sizeof(item->name) - 1] = '\0';
        // }
        else if (!strcmp("key", val[i].key) && type == ITEM_TYPE_HEXARRAY)
        {
            memcpy(item->key, val[i].value, sizeof(item->key));
        }
        // else if (!strcmp("note", val[i].key) && type == ITEM_TYPE_STRING)
        // {
        //     memcpy(item->note, val[i].value, sizeof(item->note));
        //     item->note[sizeof(item->note) - 1] = '\0';
        // }
    }

    return 0;
}

static int rfid_db_insert(mf_door_t *door, mf_door_keyval_t *keyval, int num)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    rfid_table_member_t member = {0};

    parse_item_char(keyval, num, &member);

#if 1
    printf("key:");
    for (int i = 0; i < sizeof(member.key); i ++)
        printf("%x ", member.key[i]);
    printf("\n");

    // printf("uid:");
    // for (int i = 0; i < sizeof(member.uid); i ++)
    //     printf("%x ", member.uid[i]);
    // printf("\n");

    // printf("id:%d\n" "name:%s\n" "note:%s\n", member.id, member.name, member.note);
#endif

    return db_insert(dq->base.db, &member);
}

static int rfid_db_delete(mf_door_t *door, void *value)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;

    return db_delete(dq->base.db, *(int *)value);
}

static int rfid_db_select(mf_door_t *door, mf_door_keyval_t *keyval, char *dst_key, void *out_val)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    rfid_table_member_t member = {0};
    int ret = 0;

    parse_item_char(keyval, 1, &member);

    ret = db_select(dq->base.db, keyval->key, keyval->value, &member);
    if (ret) {return -1;}

    int key = parse_key(dst_key);
    switch (key)
    {
    case ITEM_KEY_ID:
        *(int *)out_val = member.id;
    break;
    case ITEM_KEY_KEY:
        memcpy(out_val, member.key, RFID_TAB_KEY_LEN);
    break;
    default:break;
    }

    return 0;
}

static int rfid_db_update(mf_door_t *door, int id, mf_door_keyval_t *update)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;

    return db_update(dq->base.db, id, update);
}

static int rfid_db_check_passwd(mf_door_t *door, void *value)
{
    mf_door_rfid_t *dq = (mf_door_rfid_t *)door;
    rfid_table_member_t member = {0};
    char *passwd = (char *)value;
    int ret = 0;

    ret = db_select(dq->base.db, "key", passwd, &member);
    if (ret)
    {
        return -1;
    }

    return 0;
}

static mf_door_op_t door_rfid_op = 
{
    rfid_door_init,
    rfid_door_deinit,
    rfid_door_loop,
    rfid_doot_control,
    rfid_open_door,
    rfid_close_door,
    rfid_auto_adjust_door,
    rfid_encoding,
    rfid_decoding,
    rfid_db_insert,
    rfid_db_delete,
    rfid_db_select,
    rfid_db_update,
    rfid_db_check_passwd
};

mf_door_t *door_rfid_create(void)
{
    mf_door_rfid_t    *mdq;

    mdq = (mf_door_rfid_t *)calloc(1, sizeof(mf_door_rfid_t));
    if (NULL != mdq)
    {
        mdq->base.type = MF_DOOR_RFID;
        mdq->base.op = &door_rfid_op;
    }
    else
    {
        return NULL;
    }

    return (mf_door_t *)mdq;
}