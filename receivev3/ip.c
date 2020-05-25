
#include<memory.h>
#include "define.h"



unsigned long
resolve (char *host)
{

  struct hostent *he;
  struct sockaddress tmp;
  he = gethostbyname (host);
  if (he)
    {
      memcpy ((caddr_t) & tmp.add, he->h_addr, he->h_length);
   
	}
  else
    return (0);
  return (tmp.add);
}

char *
ntoa (u32 in)
{
  struct in_addr ad;
  ad.s_addr = in;
  return (inet_ntoa (ad));
}

int
isip (char *ip)
{
  int a, b, c, d;
  sscanf (ip, "%d.%d.%d.%d", &a, &b, &c, &d);
  if (a < 0)
    return 0;
  if (a > 255)
    return 0;
  if (b < 0)
    return 0;
  if (b > 255)
    return 0;
  if (c < 0)
    return 0;
  if (c > 255)
    return 0;
  if (d < 0)
    return 0;
  if (d > 255)
    return 0;
  return 1;
}

u16
cksum (u16 * buf, int nwords)
{
  register unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

unsigned short
ip_sum (addr, len)
     unsigned short *addr;
     int len;
{
  register int nleft = len;
  register unsigned short *w = addr;
  register int sum = 0;
  unsigned short answer = 0;
 int other;
  while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
  if (nleft == 1)
    {
      sum+= (*(unsigned char *) w)<<8;
      
    }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}
u16
udptcp_cksum (u8 * buf,int len)
{/*
  register unsigned long sum=0;
register int count = 6;
len=len>>1;
char buff[2];
buff[0]=htons(0);
buff[1]=17;
//printf("%d",htons(17));
for(;count<10;count++)
sum+=*(buf+count);
sum+=*(u16 *)buff;
sum+=*(buf+12);
buf+=10;
  for (count=0; count<len; count++)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;*/
int sum;
 struct ip *ih = (struct ip *) buf;
Psd_Header *psd_header;
if(len%2==0)
psd_header=(Psd_Header *)(buf+len);
else
{
psd_header=(Psd_Header *)(buf+len+1);
*(buf+len)=*(buf+len-1);*(buf+len-1)=0;
}
len-= sizeof (struct ip);
 psd_header->destip=ih->dst;
 psd_header->sourceip=ih->src;
  psd_header->mbz=0;
if(ih->pro == TCP)

psd_header->ptcl=0x06;

else 

psd_header->ptcl=0x11;

  psd_header->plen=htons(len);
  if(len%2==0)
  return ip_sum ((u16 *)(buf+sizeof (struct ip)),len+12);
  else 
return ip_sum ((u16 *)(buf+sizeof (struct ip)),len+13);
}
