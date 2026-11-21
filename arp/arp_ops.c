#include<arp_ops.h>
#include<stdlib.h>
#include<inttypes.h>
#include<string.h>
#include<stdio.h>

size_t current_index;

void print_mac(uint8_t* mac){
	for(int i = 0;i < 6;i++){
		printf("%02X", mac[i]);
		if(i != 5) printf(":");	
	}
	printf("\n");
}

void print_ipv4(uint8_t* ip){
	for(int i = 0;i < 4;i++){
		printf("%u", ip[i]);
		if(i != 3) printf(".");
	}
	printf("\n");
}

static void write_bytes(uint8_t* data,const uint8_t* to_write, size_t length){
	uint8_t* data_start = data + current_index;
	for(int i = 0;i < length;i++){
		data_start[i] = to_write[i];
	}	
	current_index += length;
}

void arp_create(uint8_t* packet, uint8_t* src_mac, uint8_t* src_ip, uint8_t* rec_mac, uint8_t* rec_ip,
		uint8_t* op, uint8_t* eth_dest_mac
		){
	// add ethernet frame headers
	current_index = 0;
	write_bytes(packet, eth_dest_mac, 6);
	write_bytes(packet, src_mac,6);
	const uint8_t eth_type[] = {0x08,0x06};
	write_bytes(packet, eth_type, 2);
	// end of ethernet frame header
	// arp header
	const uint8_t ht_pt_hl_pl_op[] = {
		// hardware type
		0x00, 0x01,
		// protocol type
		0x08, 0x00,
		// hardware address length
		0x06,
		// protocol address length
		0x04,
	};
	write_bytes(packet, ht_pt_hl_pl_op,6);
	write_bytes(packet, op, 2);
	write_bytes(packet, src_mac, 6);
	write_bytes(packet, src_ip, 4);
	write_bytes(packet, rec_mac, 6);
	write_bytes(packet, rec_ip, 4);
}

uint64_t read_bytes(uint8_t* data, size_t start, size_t length){
	uint64_t ans = 0;
	uint8_t* data_start = data + start;
	for(int i =0;i < length;i++){
		ans = (ans << 8) | (*(data_start + i) & 0xFF);
	}	
	return ans;
}

int arp_deduce(uint8_t* packet, uint8_t* src_mac, uint8_t* src_ip, uint8_t* rec_ip){
	//validate eth_type, operation
	if(read_bytes(packet, 12, 2) != 0x0806 || (read_bytes(packet,20, 2) != 0x0001 && read_bytes(packet, 20, 2) != 0x0002)){
		return -1;	
	}
	// src_mac starts at byte 22
	// src ip at byte 34
	// rec ip at byte 38
	memcpy(src_mac, packet + 22, 6);
	memcpy(src_ip, packet + 28, 4);
	memcpy(rec_ip, packet + 38, 4);
	return 0;
}


