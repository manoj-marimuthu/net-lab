#ifndef ARP_OPS_H
#define ARP_OPS_H

#include<stdlib.h>
#include<inttypes.h>

extern size_t current_index;
void print_mac(uint8_t* mac);
void print_ipv4(uint8_t* ip);
uint64_t read_bytes(uint8_t* data, size_t start, size_t length);
void arp_create(uint8_t* packet, uint8_t* src_mac, uint8_t* src_ip, uint8_t* rec_mac, uint8_t* rec_ip, uint8_t* op, uint8_t* eth_dest_mac);
int arp_deduce(uint8_t* packet, uint8_t* src_mac, uint8_t* src_ip, uint8_t* rec_ip);

#endif
