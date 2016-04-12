ROOT = ../../..

#PCMS	:= /opt/tools/intel-pcm/2.7/lib/cpucounters.o /opt/tools/intel-pcm/2.7/lib/msr.o /opt/tools/intel-pcm/2.7/lib/pci.o /opt/tools/intel-pcm/2.7/lib/client_bw.o

PCMS	:= ../../../../../intelpcm/lib/*.o

include $(ROOT)/common/Makefile.common

ELASTICITY ?= 2

ifeq ($(STM),SEQUENTIAL)
  BINS = $(BINDIR)/sequential-rbtree
else
  BINS = $(BINDIR)/$(STM)-rbtree
endif

.PHONY:	all clean

all:	main

rbtree.o: interface.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/rbtree.o rbtree.c
	$(CXX)  $(CFLAGS) -c -o $(BUILDIR)/rbtree.pcm.o rbtree.c
intset.o: rbtree.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c
	$(CXX) $(CFLAGS) -c -o $(BUILDIR)/intset.pcm.o intset.c

test.o: rbtree.o intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o test.c
	$(CXX) $(CFLAGS) -D__USEPCM -c -o $(BUILDIR)/test.pcm.o test.c

main: intset.o test.o $(PCMS)
	$(CC) $(CFLAGS) $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS)
	$(CXX) $(CFLAGS) -D__USEPCM -I../../../../../intelpcm/include ../../../../../common/pcmpower.c $(BUILDIR)/intset.pcm.o $(BUILDIR)/test.pcm.o $(PCMS) -o "$(BINS).pcm" $(LDFLAGS)

clean:
	-rm -f $(BINS) *.o
