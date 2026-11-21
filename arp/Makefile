COMMAND = gcc -Wall -g -I.

all: sender listener

sender: build/arp_sender.o build/arp_ops.o
	$(COMMAND) build/arp_sender.o build/arp_ops.o -o sender

listener: build/arp_listener.o build/arp_ops.o
	$(COMMAND) build/arp_listener.o build/arp_ops.o -o listener

build/arp_sender.o: arp_sender.c
	$(COMMAND) -c arp_sender.c -o build/arp_sender.o
build/arp_listener.o: arp_listener.c
	$(COMMAND) -c arp_listener.c -o build/arp_listener.o
build/arp_ops.o: arp_ops.c
	$(COMMAND) -c arp_ops.c -o build/arp_ops.o
clean:
	rm listener sender build/*.o
