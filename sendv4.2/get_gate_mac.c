#include "get_gate_mac.h"

int get_mask(char *addr_mask)
{
	struct sockaddr_in *addr;
	struct ifreq ifr;
	int sock;
	
	if((sock=socket(AF_INET,SOCK_DGRAM,0))<0)
		return -1;
	strncpy(ifr.ifr_name,DEFAULT_ETH,5);
	if(ioctl(sock,SIOCGIFNETMASK,&ifr)==-1)
	{
		perror("ioctl,mask:");
		return -1;
	}
	addr=(struct sockaddr_in*)&ifr.ifr_addr;
	strncpy(addr_mask,(char *)inet_ntoa(addr->sin_addr),24);	
	close(sock);	
}

int cmp_in_or_out_localnet(char* locall_ip,char *dest_ip)//判断是不是在一个网里，是就返回1，不是返回0
{
	//同时跟掩码进行与运算，如果与运算的结果都一样那么就是一个网段的了
	char mask_ip[30];
	unsigned int  tmp_mask;
	unsigned int  tmp_local;
	unsigned int tmp_dest;
	get_mask(mask_ip);//利用上面的函数得到掩码
//	printf("maskip %s\n",mask_ip);
	tmp_mask=(unsigned int)(inet_addr(mask_ip));
	tmp_local=(unsigned int)(inet_addr(locall_ip));
	tmp_dest=(unsigned int)(inet_addr(dest_ip));	
	int i;	
	int tmp;
	i=tmp_mask & tmp_local;
	tmp=tmp_mask& tmp_dest;
	if (i==tmp)
	//printf("in\n");
	return 1;
	else return 0;
	
}



int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
  struct nlmsghdr *nlHdr;
  int readLen = 0, msgLen = 0;
  do{
    	//收到内核的应答
    	if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
    	{
      		 perror("SOCK READ: ");
     		 return -1;
    	}
  
   	 nlHdr = (struct nlmsghdr *)bufPtr;
   	 //检查header是否有效
   	 if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
   	 {
    		  perror("Error in recieved packet");
      		  return -1;
    	}
  
    	/* Check if the its the last message */
    	if(nlHdr->nlmsg_type == NLMSG_DONE)
   	 {
     		 break;
    	 }
   	 else
   	 {
    	  /* Else move the pointer to buffer appropriately */
     		 bufPtr += readLen;
     		 msgLen += readLen;
    	}
  
      /* Check if its a multi part message */
      if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
     {
    	  /* return if its not */
      		break;
    }
   } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
  	return msgLen;
}

//分析返回的路由信息
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway)
{
  	struct rtmsg *rtMsg;
 	 struct rtattr *rtAttr;
 	 int rtLen;
 	 char *tempBuf = NULL;
 	//2007-12-10
 	 struct in_addr dst;
 	 struct in_addr gate;
 
 	 tempBuf = (char *)malloc(100);
 	 rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
 	 // If the route is not for AF_INET or does not belong to main routing table
 	 //then return.
  	if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
  	return;
 	 /* get the rtattr field */
 	 rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
  	rtLen = RTM_PAYLOAD(nlHdr);
  	for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen))
	{
   		switch(rtAttr->rta_type)
		 {
   			case RTA_OIF:
   			 if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
   			 break;
  			 case RTA_GATEWAY:
    			rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
   			 break;
  			 case RTA_PREFSRC:
   		 	rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
   			 break;
  			 case RTA_DST:
   			 rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
   			 break;
   		}	
      }
  //2007-12-10
  dst.s_addr = rtInfo->dstAddr;
  if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
  {
    gate.s_addr = rtInfo->gateWay;
    strcpy(gateway, (char *)inet_ntoa(gate));//网络地址变本地地址
  }
  free(tempBuf);
  return;
}

int get_gateway(char *gateway)//获取网关ip地址
{
 	 struct nlmsghdr *nlMsg;
	 struct rtmsg *rtMsg;
	 struct route_info *rtInfo;
 	char msgBuf[BUFSIZE];
 
 	int sock, len, msgSeq = 0;
 	//创建 Socket
 	if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
 	{
  		perror("Socket Creation: ");
  		return -1;
 	}
 
 	/* Initialize the buffer */
	 memset(msgBuf, 0, BUFSIZE);
 
 	/* point the header and the msg structure pointers into the buffer */
 	nlMsg = (struct nlmsghdr *)msgBuf;
 	rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);
 
 	/* Fill in the nlmsg header*/
 	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
        nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
 
	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
	 nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
 	nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.
 
 	/* Send the request */
 	if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
	{
 		 printf("Write To Socket Failed...\n");
  		return -1;
 	}
 
	 /* Read the response */
	 if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0)
	 {
 		 printf("Read From Socket Failed...\n");
 		 return -1;
	 }
	 /* Parse and print the response */
	 rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
 	for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len))
	{
		  memset(rtInfo, 0, sizeof(struct route_info));
		  parseRoutes(nlMsg, rtInfo,gateway);
	 }
	 free(rtInfo);
 	 close(sock);
	 return 0;
}

int get_ifi(char *dev, char * mac, int macln, struct in_addr *lc_addr, int ipln)
{
        int reqfd, n;
        struct ifreq macreq;

        reqfd = socket(AF_INET, SOCK_DGRAM, 0);
        strcpy(macreq.ifr_name, dev);

        /* 获取本地接口MAC地址*/
        if(ioctl(reqfd, SIOCGIFHWADDR, &macreq) != 0)
                return 1;
        memcpy(mac, macreq.ifr_hwaddr.sa_data, macln);

        /* 获取本地接口IP地址*/
        if(ioctl(reqfd, SIOCGIFADDR, &macreq) != 0)
                return 1;
        memcpy(lc_addr, &((struct sockaddr_in *)(&macreq.ifr_addr))->sin_addr, ipln);

        return 0;
}

void prmac(u_char *ptr)
{
        printf(" MAC is: %02x:%02x:%02x:%02x:%02x:%02x\n",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
}

void usage_quit(char *arg0)
{
        fprintf(stderr, "Usage: %s <query_IP>\n", arg0);
        exit(1);
}


void get_dest_mac(char *locall_ip,char *mac_ip,char buf[])//mac_ip其实就是dest目的地址的ip，buf里返回的是最后要求的mac
{
        /*if(argc != 2)
                usage_quit(argv[0]);*/

        int fd, salen, n;
        u_char *mac;
        int runout=0;
        char recv_buf[120], rep_addr[16];
        struct in_addr lc_addr, req_addr;
        struct sockaddr_ll reqsa, repsa;
        struct arp_pkt 
	{
                struct ether_header eh;
                struct ether_arp ea;
                u_char padding[18];
        } req;
	char get_mac_ip[30];//要获取mac的ip，网内就是目的ip ，网外的该地址就是网关的
	
	int in_or_out=cmp_in_or_out_localnet(locall_ip,mac_ip);
	
	while(1)
  { 	if(runout==10) in_or_out=0;
	if(in_or_out==0)//不在在一个网里
	{
		//char gateway[255]={0};
		get_gateway(get_mac_ip);//获取网关ip地址
		printf("Gateway:%s\n", get_mac_ip);//打印出网关地址，这里不了
	}
	else	
	
	strcpy(get_mac_ip,mac_ip);//在一个网里要获取的mac地址的ip就是目的ip
	
        bzero(&reqsa, sizeof(reqsa));
        reqsa.sll_family = PF_PACKET;
        reqsa.sll_ifindex = if_nametoindex(DEFAULT_ETH);


        if((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
                perror("Socket error");
                exit(1);
        }

        mac = (char *)malloc(ETH_ALEN);
        bzero(&req, sizeof(req));

        if(get_ifi(DEFAULT_ETH, mac, ETH_ALEN, &lc_addr, INLEN))
	 {
                fprintf(stderr, "Error: Get host’s information failed\n");
                exit(0);
        }
         
        memcpy(req.eh.ether_dhost, MAC_BCAST_ADDR, ETH_ALEN);
        memcpy(req.eh.ether_shost, mac, ETH_ALEN);
        req.eh.ether_type = htons(ETHERTYPE_ARP);

        /* 填写arp数据 */
        req.ea.arp_hrd = htons(ARPHRD_ETHER);
        req.ea.arp_pro = htons(ETHERTYPE_IP);
        req.ea.arp_hln = ETH_ALEN;
        req.ea.arp_pln = INLEN;
        req.ea.arp_op = htons(ARPOP_REQUEST);
        memcpy(req.ea.arp_sha, mac, ETH_ALEN);
        memcpy(req.ea.arp_spa, &lc_addr, INLEN);
        inet_aton(get_mac_ip,(struct in_addr *) req.ea.arp_tpa);//这里填充了要获取的mac主机对应的ip地址

        bzero(recv_buf, sizeof(recv_buf));
        bzero(&repsa, sizeof(repsa));
        salen = sizeof(struct sockaddr_ll);

        if((n = sendto(fd, &req, sizeof(req), 0, (struct sockaddr *)&reqsa, sizeof(reqsa))) <= 0)
	 {
                perror("Sendto error");
                exit(1);
        }



        while(1) 
	{
                if((n = recvfrom(fd, recv_buf, sizeof(req), 0, (struct sockaddr *)&repsa, &salen)) <= 0)
		 {
                        perror("Recvfrom error");
                       return;
                 }
                if(ntohs(*(__be16 *)(recv_buf + 20))==2 && !memcmp(req.ea.arp_tpa, recv_buf + 28, 4))
		 {
                        
			*buf=*(recv_buf+22);*(buf+1)=*(recv_buf+23);*(buf+2)=*(recv_buf+24);*(buf+3)=*(recv_buf+25);*(buf+4)=*(recv_buf+26);*(buf+5)=*(recv_buf+27);
                 
                       return;
                 }
                else
                    {
                   	 printf("Have recive data and is not what i want \n");
                   	 runout++;
                   	 if(runout==10)
                   	 break;
                    }
        }
  }
	
        free(mac);
}
