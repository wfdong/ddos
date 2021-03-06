#include<gtk/gtk.h>
#include<string.h>
#include<stdlib.h>
#include "ctrl.h"
#include <time.h>


//gtk_callback.c
GtkWidget *button_login;
GtkWidget *notebook;
GtkBuilder *builder;
GtkWidget *button_start,*button_stop,*button_quit;
GtkWidget *entry_username,*entry_password;
GtkWidget *entry_attack_ip;
GtkWidget *label_state;
GtkWidget *label_total_num;

GtkWidget *label_nums_print;
GtkTextBuffer *buffer;
GtkTextIter iter;


GtkWidget *ssec,*smin,*shour,*sday,*smon,*syear;
GtkWidget *esec,*emin,*ehour,*eday,*emon,*eyear;
GtkWidget *send_rate;
GtkWidget *pkt_size;

void gettime(struct ptime *pstm,struct ptime* petm);
void dialog_information(char *information);
void on_button_login_clicked(GtkButton *button,gpointer data);
void on_radio_released(GtkWidget *radiobutton,gpointer data);
void on_button_confirm_clicked(GtkWidget* button,gpointer data);
void on_button_startlisten_clicked(GtkWidget* button,gpointer data);
void on_button_quit_clicked(GtkWidget* button,gpointer data);

void log_store( char *file_name,char *str_enter);
void str_store( char *file_name,char *str_enter);
GtkEntryCompletion * str_completion(char *file_name);
void on_button_new_user_ok_clicked(GtkWidget* button,gpointer data);
void on_menu_add_user_clicked(GtkWidget* button,gpointer data);
void on_menu_item_help_about_clicked(GtkWidget *button,gpointer data);
void on_menu_item_edit_user_clicked(GtkWidget *button,gpointer data);

void on_edit_user_changed();
void on_changed(GtkWidget * combo,gpointer data);
int  is_user_nama_aviable(char * user_new_name);

void on_button_new_user_cancle_clicked(GtkWidget *button,gpointer data);
void show_user();

void clear_gtk_buf(GtkTextView * text_view);

void on_menu_item_change_victim_port();
void on_menu_item_change_child_thread(GtkWidget* button,gpointer data);
int get_gtk_init();//init the gtk
void *main_exit(void);

