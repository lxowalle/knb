#include "mf_door.h"
#include <stdlib.h>
#include <stdio.h>

static mf_door_config_t mf_door_cfg = {0};

#if MF_DOOR_QRCODE_ENABLE
extern mf_door_t *door_qrcode_create(void);
#endif /* MF_DOOR_QRCODE_ENABLE */

/* Add more */
static mf_door_create_func_ptr_t create_list[] = 
{
#if MF_DOOR_QRCODE_ENABLE
    door_qrcode_create
#endif /* MF_DOOR_QRCODE_ENABLE */
};

/**
 * @brief 获取操作门的配置信息
 * @return
*/
mf_door_config_t *mf_door_get_cfg(void)
{
    return &mf_door_cfg;
}

/**
 * @brief 获取操作门的句柄
 * @return
*/
mf_door_t *mf_door_get(mf_door_type_t type)
{
    mf_door_config_t *door_cfg = mf_door_get_cfg();
    uint32_t door_cnt = door_cfg->door_cnt;
    int i = 0;

    for (i = 0; i < door_cnt; i ++)
    {
        if (door_cfg->door[i]->type == type)
        {
            return door_cfg->door[i];
        }
    }

    return NULL;
}

static int create_door_database(char *filename, sqlite3 **ppDb)
{
    int res = -1;

    /* 初始化数据库 */
    res = sqlite3_open(filename, ppDb);
    if (res)
    {
        printf("Can't open database:%s\n", sqlite3_errmsg(*ppDb));
        sqlite3_close(*ppDb);
        return -1;
    }

    return 0;
}

/**
 * @brief 初始化门相关操作
 * @return
*/
int mf_door_init(char *db_path)
{
    mf_door_config_t *door_cfg = mf_door_get_cfg();
    mf_door_t *door;
    uint32_t door_cnt;
    int res = -1;

    if (door_cfg->init)
        return -1;

    /* 创建数据库 */
    res = create_door_database(db_path, &door_cfg->db);
    if (res < 0)
    {   
        printf("Create door database failed.res:%d\n", res);
        return -1;
    }

    /* 初始化门的操作 */
    door_cnt = sizeof(create_list) / sizeof(create_list[0]);
    for (int i = 0; i < door_cnt; i ++)
    {
        door = create_list[i]();
        if (door == NULL)
        {
            printf("door[%d] create failed\n", i);
        }
        else
        {
            door_cfg->door[door_cfg->door_cnt ++] = door;
            res = door->op->init(door_cfg);                         // 初始化
            door->valid = res ? 0 : 1;
        }
    }

    door_cfg->door_cnt = door_cnt;
    door_cfg->init = 1;

    return 0;
}

/**
 * @brief 反初始化门相关操作
 * @return
*/
int mf_door_deinit(void)
{
    mf_door_config_t *door_cfg = mf_door_get_cfg();
    mf_door_t *door;
    uint32_t door_cnt = door_cfg->door_cnt;

    if (!door_cfg->init)
        return -1;

    for (int i = 0; i < door_cnt; i ++)
    {
        door = door_cfg->door[i];
        door->op->deinit(door_cfg);
        free(door);
        door = NULL;
    }

    door_cfg->door_cnt = 0;
    door_cfg->init = 0;

    return 0;
}

/**
 * @brief 开门
 * @return
*/
int mf_door_open(mf_door_t *door)
{
    if (door == NULL)
        return -1;

    return door->op->open();
}

/**
 * @brief 关门
 * @return
*/
int mf_door_close(mf_door_t *door)
{
    if (door == NULL)
        return -1;

    return door->op->close();
}

/**
 * @brief 存入密码
 * @return
*/
int mf_door_insert_passwd(mf_door_t *door, void *param)
{
    mf_door_config_t *door_cfg = mf_door_get_cfg();

    if (door == NULL && param == NULL)
        return -1;

    return door->op->insert_passwd(door_cfg, param);
}

/**
 * @brief 查询密码
 * @return
*/
int mf_door_select_passwd(mf_door_t *door, void *param)
{
    if (door == NULL && param == NULL)
        return -1;

    return door->op->select_passwd(param);    
}

/**
 * @brief 删除密码
 * @return
*/
int mf_door_delete_passwd(mf_door_t *door, void *param)
{
    if (door == NULL && param == NULL)
        return -1;

    return door->op->delete_passwd(param);    
}