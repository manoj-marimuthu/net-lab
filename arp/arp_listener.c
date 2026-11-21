#include<stdio.h>
#include<inttypes.h>
#include<sys/socket.h>
#include<linux/if_ether.h>
#include<linux/if_packet.h>
#include<arpa/inet.h>
#include<net/if.h>
#include<unistd.h>
#include<arp_ops.h>
#include<string.h>

int main(){
	int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));	
	if(sockfd < 0){
		perror("socket");
		return 1;
	}	
	uint8_t buffer[1000];
	ssize_t received = recvfrom(
				sockfd,
				buffer,
				sizeof(buffer),
				0,
				NULL,
				NULL);
	if(received < 0){
		perror("recvfrom");
		return 1;
	}
	else{
		printf("Received %zu bytes\n", (size_t) received);
	}
	uint8_t src_mac[10];
	uint8_t src_ip[10];
	uint8_t rec_ip[10];
	uint8_t my_ip[] = {0x14, 0x23, 0x02, 0x02};
	int status = arp_deduce(buffer, src_mac, src_ip, rec_ip);
	if(status < 0){
		printf("Deduction of packet as an ARP request failed, possible due to incorrect operation/eth_type/ip mismatch\n");
		return 1;
	}
	printf("Source MAC address = ");
	print_mac(src_mac);
	printf("Source IP = ");
	print_ipv4(src_ip);
	printf("Receiver IP = ");
	print_ipv4(rec_ip);
	int eq = 1;
	for(int i =0; i < 4;i++){
		if(rec_ip[i] != my_ip[i]){
			eq = 0;
			break;
		}
	}
	if(!eq){
		printf("IP address not matching, not a request for my MAC address\n");
		return 1;
	}else{
		printf("IP address matching, sending MAC address\n");
	}
	uint8_t packet[48];
	uint8_t reply_src_ip[] = {0x14, 0x23, 0x02, 0x02};
	uint8_t reply_my_mac[] =  {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA};
       	uint8_t reply_rec_ip[] = {0x14, 0x23, 0x02, 0x01};
	uint8_t reply_rec_mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
	uint8_t op[] = {0x00, 0x02};
	arp_create(packet, reply_my_mac, reply_src_ip, reply_rec_mac, reply_rec_ip, op, reply_rec_mac);

	// transmit the packet back to host that requested
	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ARP);
	addr.sll_ifindex = if_nametoindex("eth0");
	addr.sll_halen = ETH_ALEN;
	memcpy(addr.sll_addr, src_mac, 6);
	size_t sent = sendto(sockfd, packet, 42, 0, (struct sockaddr*) &addr, sizeof(addr));
	if(sent < 0){
		perror("sendto");
		return 1;
	}else{
		printf("Sent %zu bytes back to requester host\n", sent);
	}
	return 0;
}
