#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if_arp.h>
#include <asm/types.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <features.h> 

#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
        #include <netpacket/packet.h>
        #include <net/ethernet.h>
#else
        #include <asm/types.h>
        #include <linux/if_packet.h>
        #include <linux/if_ether.h>
#endif
#include <netinet/if_ether.h>

#define INLEN 4
#define MAC_BCAST_ADDR  (uint8_t *) "\xff\xff\xff\xff\xff\xff"
#define BUFSIZE 8192
#define DEFAULT_ETH "eth1"
 
struct route_info
{
	u_int dstAddr;
	u_int srcAddr;
 	u_int gateWay;
 	char ifName[IF_NAMESIZE];
};

int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId);
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway);

int get_gateway(char *gateway);

int get_ifi(char *dev, char * mac, int macln, struct in_addr *lc_addr, int ipln);
void prmac(u_char *ptr);
void usage_quit(char *arg0);

void get_dest_mac(char *local_ip,char *mac_ip,char buf[]);























