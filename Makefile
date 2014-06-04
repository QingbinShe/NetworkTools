all:ScanPort SynFlood

ScanPort:ScanPort.o
	gcc -o ScanPort ScanPort.o

ScanPort.o:ScanPort.c
	gcc -c ScanPort.c

SynFlood:SynFlood.o
	gcc -o SynFlood SynFlood.o

SynFlood.o:SynFlood.c
	gcc -c SynFlood.c

clean:
	rm ScanPort ScanPort.o
	rm SynFlood SynFlood.o
