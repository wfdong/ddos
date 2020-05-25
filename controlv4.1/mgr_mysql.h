#define _XOPEN_SOURCE
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"mysql.h"
#include <unistd.h>
#include <crypt.h>

char* crypt_password(char *password);
void create_table_user();
void exist_user_table();
void is_database_exist();
int comp_password_crypt_sql(char* name,char *password_to_check);
void add_new_user(char *sql_query);

