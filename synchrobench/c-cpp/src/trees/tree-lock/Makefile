ROOT = ../../..

include $(ROOT)/common/Makefile.common

PCMS	:= ../../../../../intelpcm/cpucounters.o ../../../../../intelpcm/msr.o ../../../../../intelpcm/pci.o ../../../../../intelpcm/client_bw.o

BINS = $(BINDIR)/$(LOCK)-RCU-tree
#CFLAGS+=-DEXTERNAL_RCU

.PHONY:	all clean

all:	main

new_urcu.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/new_urcu.o new_urcu.c
	$(CXX) $(CFLAGS) -c -o $(BUILDIR)/new_urcu.pcm.o new_urcu.c


citrus.o: urcu.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/citrus.o citrus.c
	$(CXX) $(CFLAGS) -c -o $(BUILDIR)/citrus.pcm.o citrus.c

test.o: citrus.h urcu.h
	$(CC) $(CFLAGS) -L. -c -o $(BUILDIR)/test.o test.c
	$(CXX) $(CFLAGS) -D__USEPCM -L. -c -o $(BUILDIR)/test.pcm.o test.c

main: new_urcu.o citrus.o test.o urcu.h
	$(CC) $(CFLAGS) $(BUILDIR)/new_urcu.o $(BUILDIR)/citrus.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS)
	$(CXX) $(CFLAGS) $(BUILDIR)/new_urcu.pcm.o $(BUILDIR)/citrus.pcm.o $(BUILDIR)/test.pcm.o $(PCMS) -o "$(BINS).pcm" $(LDFLAGS)

clean:
	-rm -f $(BINS)
