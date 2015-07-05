#VERSION = TCMALLOC_NO_UPDATE

ROOT = ../../..

include $(ROOT)/common/Makefile.common

PCMS	:= ../../../../../intelpcm/cpucounters.o ../../../../../intelpcm/msr.o ../../../../../intelpcm/pci.o ../../../../../intelpcm/client_bw.o

.PHONY:	all clean
all:	main

BINS = $(BINDIR)/lockfree-bst

CC = g++
CFLAGS += -std=gnu++0x

main: test.o
	$(CC) $(CFLAGS) test.o -o $(BINS) $(LDFLAGS)
	$(CXX) $(CFLAGS) test.pcm.o $(PCMS) -o "$(BINS).pcm" $(LDFLAGS)

test.o: wfrbt.h test.c
	$(CC) $(CFLAGS) -c test.c
	$(CXX) $(CFLAGS) -D__USEPCM -c test.c -o test.pcm.o

clean:
	-rm -f $(BINS) *.o
