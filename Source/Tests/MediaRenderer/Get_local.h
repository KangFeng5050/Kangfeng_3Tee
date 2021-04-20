#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
 
#define IF_NAME "eth0"  //network port

/*
#define ifr_name ifr_ifrn.ifrn_name 
#define ifr_hwaddr ifr_ifru.ifru_hwaddr 
#define ifr_addr ifr_ifru.ifru_addr 
#define ifr_dstaddr ifr_ifru.ifru_dstaddr 
#define ifr_broadaddr ifr_ifru.ifru_broadaddr 
#define ifr_netmask ifr_ifru.ifru_netmask 
#define ifr_flags ifr_ifru.ifru_flags 
#define ifr_metric ifr_ifru.ifru_ivalue 
#define ifr_mtu  ifr_ifru.ifru_mtu 
#define ifr_map  ifr_ifru.ifru_map 
#define ifr_slave ifr_ifru.ifru_slave 
#define ifr_data ifr_ifru.ifru_data 
#define ifr_ifindex ifr_ifru.ifru_ivalue 
#define ifr_bandwidth ifr_ifru.ifru_ivalue    
#define ifr_qlen ifr_ifru.ifru_ivalue 
#define ifr_newname ifr_ifru.ifru_newname 
#define ifr_settings ifr_ifru.ifru_settings
#define IFHWADDRLEN 6

struct ifreq{
 union
 {
  char ifrn_name[IFNAMSIZ];  
 } ifr_ifrn;
 
 union {
  struct sockaddr ifru_addr;
  struct sockaddr ifru_dstaddr;
  struct sockaddr ifru_broadaddr;
  struct sockaddr ifru_netmask;
  struct  sockaddr ifru_hwaddr;
  short ifru_flags;
  int ifru_ivalue;
  int ifru_mtu;
  struct  ifmap ifru_map;
  char ifru_slave[IFNAMSIZ]; 
  char ifru_newname[IFNAMSIZ];
  void __user * ifru_data;
  struct if_settings ifru_settings;
 }ifr_ifru;
};*/

int get_local_ip(char *ifname,char *ip);
int get_local_mac(char* buf);

