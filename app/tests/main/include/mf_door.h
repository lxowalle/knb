#ifndef __MF_DOOR_H
#define __MF_DOOR_H

#include <stdint.h>
#include "sqlite3.h"

#define MF_DOOR_QRCODE_ENABLE      (1)
#define MF_DOOR_ENABLE_MAX         (32)

struct mf_door_op_t;
struct mf_door_config_t;
typedef struct mf_door_config_t mf_door_config_t;
typedef struct mf_door_op_t mf_door_op_t;

typedef enum
{
    MF_DOOR_NONE = 0,
    MF_DOOR_IDCARD,
    MF_DOOR_RFID,
    MF_DOOR_NUMBER,
    MF_DOOR_QRCODE,
    MF_DOOR_MAX         // Must be exist
}mf_door_type_t;

struct mf_door_op_t
{
    int (*init)(mf_door_config_t *door_config);
    int (*deinit)(mf_door_config_t *door_config);
    int (*open)(void);
    int (*close)(void);
    int (*auto_adjust)(int open, int ms);
    int (*insert_passwd)(mf_door_config_t *door_config, void *param);
    int (*select_passwd)(void *param);
    int (*delete_passwd)(void *param);
    int (*encoding)(void *in, void *out);
    int (*decoding)(void *in, void *out);
    int (*control)(void *param);
};

typedef struct
{
    mf_door_type_t type;
    mf_door_op_t *op;
    uint8_t valid;
    uint8_t has_db_table;
}mf_door_t;

typedef mf_door_t *(*mf_door_create_func_ptr_t)(void);

struct mf_door_config_t
{
    uint32_t init;

    char db_name[20];
    struct sqlite3 *db;

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
 * @brief 反初始化门相关操作
 * @return
*/
int mf_door_deinit(void);

int mf_door_insert_passwd(mf_door_t *door, void *param);

#endif 
