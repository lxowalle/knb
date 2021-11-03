#include <stdio.h>
#include "test.h"
#include "stdint.h"
#include "string.h"
#include "mf_door.h"
#include "sqlite3.h"
#include "stdlib.h"

static sqlite3 *db = NULL;
typedef struct
{
    uint32_t id;
    uint32_t start_time;
    uint32_t end_time;
    char token[256];
    char note[50];
}qrcode_table_member_t;

static void parse(void *param)
{
    char *para = (char *)param;


}

int main()
{
    // printf("main\n");
    mf_door_init("door.sqlite");

    mf_door_config_t *door_cfg = mf_door_get_cfg();
    db = door_cfg->db;
    // test();

    char **param = {"id" , sizeof("id"),
                    "123", sizeof("123"),

                    "start_time", "1",
                    "end_time"  , "1000",
                    "token"     , "aabbccddeeffgg",
                    "note"      , "this is xxxx"};

    char *data = (char *)calloc(1, 256);


    mf_door_t *door = mf_door_get(MF_DOOR_QRCODE);
    if (door != NULL)
    {
        // mf_door_insert_passwd(door, NULL);

        // mf_door_delete_passwd(door, NULL);

        mf_door_select_passwd(door, NULL);
    }

    mf_door_deinit();

    return 0;
}

