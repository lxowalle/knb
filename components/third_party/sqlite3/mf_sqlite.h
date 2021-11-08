#ifndef __MF_SQLITE_H
#define __MF_SQLITE_H

#include "sqlite3.h"

/**
 * @brief 创建数据库
 * @return
*/
int mf_sqlite_create_database(char *filename, sqlite3 **ppDb);

/**
 * @brief 创建一张表
 * @details
 * "key BLOB NOT NULL,uid BLOB NOT NULL"
 * @return
*/
int mf_sqlite_create_table(sqlite3 *database, char *table_name, char *member);

/**
 * @brief 删除表
 * @return
*/
int mf_sqlite_delete_table(sqlite3 *database, char *table_name);

/**
 * @brief 查看表是否存在
 * @return 返回表的数量
*/
int mf_sqlite_check_table(sqlite3 *database, char *table_name);

/**
 * @brief 查看表的成员存在
 * @return 返回成员数量
*/
int mf_sqlite_check_member(sqlite3 *database, char *table_name, char *member_name);

/**
 * @brief 查看表的成员类型是否匹配
 * @return 返回匹配的数量
*/
int mf_sqlite_check_member_type(sqlite3 *database, char *table_name, char *member_name, char *member_type);

/**
 * @brief 增加一行
*/
int mf_sqlite_insert(sqlite3 *database, char *table_name, char *member[], int len);



#endif //  __MF_SQLITE_H