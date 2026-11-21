#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include<arp_ops.h>
#include<sys/socket.h>
#include<linux/if_packet.h>
#include<linux/if_ether.h>
#include<arpa/inet.h>
#include<net/if.h>
#include<string.h>

static void traverse_bytes(uint8_t* data, size_t length){
	printf("----Raw packet bytes----\n");
	for(int i = 0;i < length;i++){
		printf("%02" PRIX8 " ", data[i]);
		if(i % 8 == 0 && i != 0) printf("\n");
	}
	printf("\n");
}

int main(void){
	// input fields and packet construction
	uint8_t src_mac[] = {0x8A, 0x41,0x40, 0x98,0xB7, 0x77};
	uint8_t src_ip[] = {0x0A,0x4D,0x00,0x01};
	uint8_t rec_ip[] = {0x0A,0x4D,0x00,0x02};
	uint8_t rec_mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t op[] = {0x00, 0x01};
	uint8_t eth_dest_mac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t packet[48];
	arp_create(packet, src_mac, src_ip, rec_mac, rec_ip, op, eth_dest_mac);
	printf("Packet size = %zu \n", current_index);
	traverse_bytes(packet, current_index);	
	// socket 
	int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = if_nametoindex("arp0");

	if(addr.sll_ifindex == 0){
		perror("if_nametoindex");
		return 1;
	}

	if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
		perror("bind");
		return 1;
	}
	// destination address and transmission
	
	struct sockaddr_ll dest_addr = {0};
	dest_addr.sll_family = AF_PACKET;
	dest_addr.sll_protocol = htons(ETH_P_ALL);
	dest_addr.sll_ifindex = addr.sll_ifindex;
	dest_addr.sll_halen = ETH_ALEN;
	memset(dest_addr.sll_addr, 0xFF, ETH_ALEN);
	
	ssize_t sent = sendto(
				sockfd,
				packet,
				42,
				0,
				(struct sockaddr*)  &dest_addr,
				sizeof(dest_addr)
			);
	if(sent < 0){
		perror("sendto");
		return 1;
	}else{
		printf("SENT %zu bytes \n", (size_t) sent);
	}
	printf("Waiting for ARP reply requests\n");
	uint8_t buffer[100];
	ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
	if(!(read_bytes(buffer, 12, 2) == 0x0806 && read_bytes(buffer, 20, 2) == 0x0002)){
		printf("Invalid response, not an ARP reply\n");
		return 1;
	}
	if(received < 42){
		printf("Invalid arp reply, not 42 bytes\n");
		return 1;
	}
	if(received < 0){
		perror("recvfrom");
		return 1;
	}
	printf("Received an ARP reply\n");
	uint8_t reply_src_mac[6];
	uint8_t reply_rec_ip[4];
	uint8_t reply_src_ip[4];
	arp_deduce(buffer, reply_src_mac, reply_src_ip, reply_rec_ip);
	printf("Received MAC = ");
	print_mac(reply_src_mac);
	return 0;
}
