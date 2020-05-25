#include "mgr_mysql.h"


int role_id = -1;
char *sql_address="localhost";
char *sql_user_name="root";
char *sql_password="315123";
char *sql_database="mgr_user";
int sql_port=0;
char *sql_sock_name=NULL;
int sql_flag=0;

char* crypt_password(char *password)
{
        char salt[]="$1$6197SN0q";
        char *code;

        code=(char *)malloc(sizeof(char)*34);
        code=(char *)crypt(password,salt);
	return code;
}

int comp_password_crypt_sql(char* name,char *password_to_check)
{
   MYSQL sql;
   MYSQL_RES *res;
   MYSQL_ROW row;
   char* password_real;
   char sql_query[100];
   char* crypt_after;
   const char* role;	    
   int res_num;
   
  mysql_init(&sql);
 
 if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
  {	
	sprintf(sql_query,"select password ,role from user where name='%s'",name);
	res_num=mysql_query(&sql,sql_query);
        if(res_num)
	{
		dialog_information("select password error when query");		
		return;	 
	}
	res=mysql_store_result(&sql);
        if(res)
       {
		if((int)mysql_num_rows(res)==0)
		{
		dialog_information("the user not exist");
		return;
		}

		if((int)mysql_num_rows(res)!=1)
		{
		dialog_information("wrong when query password");
		return;
		}
	
		row=mysql_fetch_row(res);
 		password_real=row[0];
		role=row[1];

		role_id=atoi(role);
		crypt_after=crypt_password(password_to_check);
		if(strcmp(crypt_after,password_real)==0)
		{
			printf("the password is correct\n");
			mysql_close(&sql);
			return 2;
		}
		else
		{
			mysql_close(&sql);
			return 3 ;		
		}
      }
   else
   
	 dialog_information("the result of querying password is null");
 }
 else
	dialog_information("connect database failed\n when check the usrs password");
}



void create_table_user()
{
 MYSQL sql;
 mysql_init(&sql);
 
if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
	{
	 if(  mysql_query(&sql,"create table user("
				     "name varchar(20),"
				     "id int(11) not null AUTO_INCREMENT  primary key,"
				     "password varchar(40),"
				     "role int(11),"
                                     "email char(25)"
                                     " );"
		
				))
 		{
			printf("create table wrong\n");
			dialog_information("create table wrong");
 		}
  	else 
		{
		printf("create table success\n");
		
		mysql_close(&sql);
		}
 	}
 else
	{
	 printf("failed connect when create table user\n");
	 dialog_information("failed connect when create table user");
	}

}

void exist_user_table()
{
 MYSQL sql;
 MYSQL_RES *res;
 
 mysql_init(&sql);
 if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
	{
		mysql_query(&sql,"show tables like 'user';");
		res=mysql_store_result(&sql);
		if(mysql_num_rows(res)==1)
		{
			printf("table user exist\n");
					
		}
		else
		{
			printf("table user not exists\n");
		         create_table_user();
		 }	
		mysql_close(&sql);
	}
else
	{
		printf("failed connect when exact table exist or not \n");
		dialog_information("failed connect when exact table exist or not");	
	}
 
}

void is_database_exist()
{
MYSQL mysql;
  
  mysql_init(&mysql);
 if( mysql_real_connect(&mysql,sql_address,sql_user_name,sql_password,NULL,sql_port,sql_sock_name,sql_flag))
	{
	  if(mysql_select_db(&mysql,"dosuser"))//不存在
	   {
	     printf("database not exist\n");
	    
	    mysql_query(&mysql,"create database dosuser;");
	     if(1==mysql_affected_rows(&mysql))
	 	{
		
		  create_table_user();
		  char super_role[100];
		  sprintf(super_role,"insert into user values('chang','','%s',5,'chang@ouc.edu.cn')",crypt_password("chang"));
	 	  add_new_user(super_role);
		}
	     else 
		{
		  dialog_information("database create failed");
		}
	   }
	 else
	  {
	   exist_user_table();
	  }
	}
  else	
	{
	  dialog_information("connect failed");
	}
   mysql_close(&mysql);	
}



int is_user_nama_aviable(char* user_new_name)
{
	MYSQL sql;
	MYSQL_RES *res;
	char query[200];	

	mysql_init(&sql);
 	if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
	{
	 sprintf(query,"select * from user where name='%s'",user_new_name); 
	 mysql_query(&sql,query);	
	 res=mysql_store_result(&sql);		 
	
	 if(mysql_num_rows(res)==0)
		{
			
			return 1;	
		}
	  else if(mysql_num_rows(res)>=1)
		{
			return(mysql_num_rows(res)+1);	 	
		}		
		else 
		dialog_information("select error when new user");
	}
	else
	{
	  dialog_information("connect database failed when new user");
	}
	mysql_close(&sql);
}

//添加新用户
void add_new_user(char *sql_query)
{
        MYSQL sql;
	MYSQL_RES *res;

	mysql_init(&sql);
  	
	 if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
	{
	  mysql_query(&sql,sql_query);
	 
	  if(mysql_affected_rows(&sql)==1)
		printf("insert new user success\n");
		
	  else 
	 	printf("insert new user failed\n");
	}
	mysql_close(&sql);
}

