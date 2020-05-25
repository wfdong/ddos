#OBJ=ctrl_main.o gtk_callback.o mysql.o ctrl.o
all: 
	gcc -o control -g ctrl_main.c gtk_callback.c mysql.c ctrl.c `pkg-config --libs --cflags gtk+-2.0`  -I/usr/include/mysql -L/usr/local/mysql/lib -lmysqlclient -export-dynamic


