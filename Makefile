all:ScanPort

ScanPort:ScanPort.o
	gcc -o ScanPort ScanPort.o

ScanPort.o:ScanPort.c
	gcc -c ScanPort.c

clean:
	rm ScanPort ScanPort.o
