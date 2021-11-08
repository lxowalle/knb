#include "mf_sqlite.h"
#include "stdio.h"
#include "string.h"

/**
 * @brief 创建数据库
 * @return
*/
int mf_sqlite_create_database(char *filename, sqlite3 **ppDb)
{
    if (filename == NULL || ppDb == NULL)
        return -1;

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
 * @brief 创建一张表
 * @details
 * "key BLOB NOT NULL,uid BLOB NOT NULL"
 * @return
*/
int mf_sqlite_create_table(sqlite3 *database, char *table_name, char *member)
{
    if (database == NULL || table_name == NULL || member == NULL)
        return -1;

    sqlite3 *db = (sqlite3 *)database;
    char *err_msg = NULL;
    int ret = -1;
    char sql[256] = {0};

    int len = snprintf(sql, sizeof(sql), "CREATE TABLE IF NOT EXISTS %s(%s);", table_name, member);
    if (len >= sizeof(sql)) return -1;

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, ret);
        sqlite3_free(err_msg);
        return -1;
    }

    return 0;
}

/**
 * @brief 删除表
 * @return
*/
int mf_sqlite_delete_table(sqlite3 *database, char *table_name)
{
    if (database == NULL || table_name == NULL)
        return -1;

    sqlite3 *db = (sqlite3 *)database;
    char *err_msg = NULL;
    int ret = -1;
    char sql[256] = {0};

    int len = snprintf(sql, sizeof(sql), "DROP TABLE %s;", table_name);
    if (len >= sizeof(sql)) return -1;

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB error: %s %d\n", err_msg, ret);
        sqlite3_free(err_msg);
        return -1;
    }

    return 0;
}

/**
 * @brief 查看表是否存在
 * @return 返回表的数量
*/
int mf_sqlite_check_table(sqlite3 *database, char *table_name)
{
    if (database == NULL || table_name == NULL)
        return -1;

    sqlite3 *db = (sqlite3 *)database;
    sqlite3_stmt *stmt = NULL;
    int ret = -1, count = 0;
    char sql[256] = {0};

    int len = snprintf(sql, sizeof(sql), "SELECT count(*) FROM sqlite_master WHERE type='table' AND name = '%s';", table_name);
    if (len >= sizeof(sql)) return -1;

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB perpare error: %d\n", ret);
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return count;
}

/**
 * @brief 查看表的成员存在
 * @return 返回成员数量
*/
int mf_sqlite_check_member(sqlite3 *database, char *table_name, char *member_name)
{
    if (database == NULL || table_name == NULL || member_name == NULL)
        return -1;

    sqlite3 *db = (sqlite3 *)database;
    sqlite3_stmt *stmt = NULL;
    int ret = -1, count = 0;
    char sql[256] = {0};

    int len = snprintf(sql, sizeof(sql), "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='%s' AND sql LIKE '%% %s %%';", table_name, member_name);
    if (len >= sizeof(sql)) return -1;

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB perpare error: %d\n", ret);
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return count;
}

/**
 * @brief 查看表的成员类型是否匹配
 * @return 返回匹配的数量
*/
int mf_sqlite_check_member_type(sqlite3 *database, char *table_name, char *member_name, char *member_type)
{
    if (database == NULL || table_name == NULL || member_name == NULL || member_type == NULL)
        return -1;

    sqlite3 *db = (sqlite3 *)database;
    sqlite3_stmt *stmt = NULL;
    int ret = -1, count = 0;
    char sql[256] = {0};

    int len = snprintf(sql, sizeof(sql), "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='%s' AND sql LIKE '%s %s %%';", table_name, member_name, member_type);
    if (len >= sizeof(sql)) return -1;

    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "DB perpare error: %d\n", ret);
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return count;
}


/**
 * @brief 增加一行
 * @return
*/
int mf_sqlite_insert(sqlite3 *database, char *table_name, char *member[], int length)
{
    if (database == NULL || table_name == NULL || member == NULL)
        return -1;

    sqlite3 *db = (sqlite3 *)database;
    int index[16] = {0};
    int index_cnt = 0;

    /* 获取有效成员索引 */
    for (int i = 0; i < length; i ++)
    {
        char *member_name = member[i * 3];
        char *member_type = member[i * 3 + 1];
        char *member_data = member[i * 3 + 2];
#if 1
        printf("(%d)%s %s %s\n", i, member_name, member_type, member_data);
#endif
        if ((mf_sqlite_check_member(db, table_name, member_name) > 0)
            && (mf_sqlite_check_member_type(db, table_name, member_name, member_type) > 0))
        {
            index[index_cnt ++] = i * 3;
        }
    }

#if 1
    printf("index cnt:%d\n", index_cnt);
#endif

    /* 添加参数 */
    char sql[512] = "INSERT INTO ";
    int len = snprintf(sql, sizeof(sql), "INSERT INTO %s", table_name);
    if (len >= sizeof(sql)) return -1;

    char tmp[128] = "(";
    char tmp2[128] = "VALUES(";
    for (int i = 0; i < index_cnt; i ++)
    {
        strcat(tmp, member[index[i]]);
        strcat(tmp, ",");
        
        strcat(tmp2, "?");
        strcat(tmp2, ",");
        printf("tmp:%s  tmp2:%s\n", tmp, tmp2);
    }
    strcat(tmp, ")");
    strcat(tmp2, ")");

    return 0;
}

