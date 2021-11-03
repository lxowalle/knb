#include <stdio.h>
#include "test.h"
#include "stdint.h"
#include "string.h"
#include "mf_door.h"
#include "sqlite3.h"


static sqlite3 *db = NULL;
int main()
{
    // printf("main\n");
    mf_door_init("door.sqlite");

    mf_door_config_t *door_cfg = mf_door_get_cfg();
    db = door_cfg->db;
    // test();

    mf_door_t *door = mf_door_get(MF_DOOR_QRCODE);
    if (door != NULL)
    {
        mf_door_insert_passwd(door, NULL);
    }

    mf_door_deinit();
#if 0
    char *db_path = "mf_door_db.sqlite";
    int ret = -1;
    ret = sqlite3_open(db_path, &db);
    if( ret ){
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    char *err_msg = NULL;

    // create table
    char* sql = "CREATE TABLE IF NOT EXISTS face("
"id INTEGER PRIMARY KEY AUTOINCREMENT,"
"uid BLOB,"
"auth INTEGER,"
"name TEXT NOT NULL,"
"job TEXT,"
"note TEXT,"
"ftr_passwd BLOB,"
"ftr_face BLOB,"
"ftr_finger BLOB,"
"ftr_card BLOB,"
"ftr_idcard BLOB"
");";

    ret = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    if( ret != SQLITE_OK ){
        fprintf(stderr, "DB error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
#endif
    return 0;
}

