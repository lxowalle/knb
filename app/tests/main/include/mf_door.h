#ifndef __MF_DOOR_H
#define __MF_DOOR_H

#include <stdint.h>
#include "sqlite3.h"

#define MF_DOOR_QRCODE_ENABLE       (0)
#define MF_DOOR_RFID_ENABLE         (1)
#define MF_DOOR_ENABLE_MAX          (32)

#define MF_DOOR_UID_SIZE            (16)
#define MF_DOOR_NAME_SIZE           (16)
#define MF_DOOR_NOTE_SIZE           (16)
#define MF_DOOR_KEY_SIZE            (16)

struct mf_door_op_t;
struct mf_door_config_t;
struct mf_door_t;
typedef struct mf_door_config_t mf_door_config_t;
typedef struct mf_door_op_t mf_door_op_t;
typedef struct mf_door_t mf_door_t;
typedef enum
{
    MF_DOOR_NONE = 0,
    MF_DOOR_IDCARD,
    MF_DOOR_RFID,
    MF_DOOR_NUMBER,
    MF_DOOR_QRCODE,
    MF_DOOR_MAX         // Must be exist
}mf_door_type_t;

typedef struct
{
    char key[16];
    char type[12];
    char value[132];
}mf_door_keyval_t;

struct mf_door_op_t
{
    int (*init)(mf_door_t *door);
    int (*deinit)(mf_door_t *door);
    int (*loop)(mf_door_t *door);
    int (*control)(void *param);
    int (*open)(void);
    int (*close)(void);
    int (*auto_adjust)(int open, int ms);
    int (*encoding)(void *in, void *out);
    int (*decoding)(void *in, void *out);
    int (*db_insert)(mf_door_t *door, mf_door_keyval_t *keyval, int num);
    int (*db_delete)(mf_door_t *door, void *value);
    int (*db_select)(mf_door_t *door, mf_door_keyval_t *keyval, char *dst_key, void *out_val);
    int (*db_update)(mf_door_t *door, int id, mf_door_keyval_t *update);
    int (*db_check_passwd)(mf_door_t *door, void *value);
};

struct mf_door_t
{
    mf_door_type_t type;
    mf_door_op_t *op;
    sqlite3 *db;
    uint8_t enable;
    uint8_t has_db_table;
};

typedef mf_door_t *(*mf_door_create_func_ptr_t)(void);

struct mf_door_config_t
{
    uint32_t init;

    char db_name[20];
    sqlite3 *db;

    uint32_t door_cnt;
    mf_door_t   *door[32];
};

/**
 * @brief 获取操作门的配置信息
 * @return
*/
mf_door_config_t *mf_door_get_cfg(void);

/**
 * @brief 获取操作门的句柄
 * @return
*/
mf_door_t *mf_door_get(mf_door_type_t type);

/**
 * @brief 初始化门相关操作
 * @return
*/
int mf_door_init(char *db_path);

/**
 * @brief 处理循环程序 
 * @return 
*/
int mf_door_loop(void);

/**
 * @brief 反初始化门相关操作
 * @return
*/
int mf_door_deinit(void);

int mf_door_insert_passwd(mf_door_t *door, void *param);
int mf_door_delete_passwd(mf_door_t *door, void *param);
int mf_door_select_passwd(mf_door_t *door, void *param);

#endif 
