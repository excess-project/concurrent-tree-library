ROOT = ../../..

PCMS	:= ../../../../../intelpcm/cpucounters.o ../../../../../intelpcm/msr.o ../../../../../intelpcm/pci.o ../../../../../intelpcm/client_bw.o

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
	$(CXX) $(CFLAGS) -D__USEPCM $(BUILDIR)/intset.pcm.o $(BUILDIR)/test.pcm.o $(PCMS) -o "$(BINS).pcm" $(LDFLAGS)

clean:
	-rm -f $(BINS) *.o
