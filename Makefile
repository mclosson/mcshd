all:	mcshd client

mcshd: 
	cc -Wall -o mcshd mcshd.c wrappedsockets.h

client: 
	cc -Wall -o client client.c wrappedsockets.h

clean:
	rm mcshd
	rm client
