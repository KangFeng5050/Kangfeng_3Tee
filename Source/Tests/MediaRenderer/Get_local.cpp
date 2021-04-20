#include "Get_local.h"

int get_local_ip(char *ifname,char *ip)
{
	char *temp = NULL;
	int inet_sock;
	struct ifreq ifr;
 
	inet_sock = socket(AF_INET,SOCK_DGRAM,0);
 
	memset(ifr.ifr_name,0,sizeof(ifr.ifr_name));
	memcpy(ifr.ifr_name,ifname,strlen(ifname));
 
	if(0 != ioctl(inet_sock,SIOCGIFADDR,&ifr))
	{
		perror("ioctl error!");
		return 1;
	}
 
	temp = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
	memcpy(ip,temp,strlen(temp));
 
	close(inet_sock);
 
	return 0;
 
}

int get_local_mac(char* buf) {
/* get mac interface */
	unsigned char 	Local_MAC[6];
	memset(Local_MAC, 0, 6);
	struct ifreq ifreq;
	int sock;
 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket falied...\r\n");
		return -1;
	}
	//strcpy(ifreq.ifr_name, "ens33");
	strcpy(ifreq.ifr_name, "eth0");
	if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
		perror("mac ioctl error:");
		return -1;
	}
	memcpy(Local_MAC, ifreq.ifr_hwaddr.sa_data, 6);
 
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
 
	close(sock);
	return 0;
}

