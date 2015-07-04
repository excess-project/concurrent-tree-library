TARGET  := GBST
TARGET-P:= GBST.pcm 
SRCS    := main.c  
OBJS    := ${SRCS:.c=.o}
OBJS-P  := ${SRCS:.c=.pcm.o}

PCMS	:= ../intelpcm/cpucounters.o ../intelpcm/msr.o ../intelpcm/pci.o ../intelpcm/client_bw.o

PREC	:= tree.o

CCFLAGS = -g -O2 -DNDEBUG -D_REENTRANT -Wall -funroll-loops -fno-strict-aliasing
CXXFLAGS = ${CCFLAGS} -fpermissive -I../intelpcm -D__USEPCM
LDFLAGS = -L../intelpcm
LIBS    = -lm -lpthread

.PHONY: all clean distclean 

all:: ${TARGET} ${TARGET-P} 

${TARGET}: ${OBJS} ${PREC}
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $< 

${TARGET-P}: ${OBJS-P} ${PREC} ${PCMS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS-P}: %.pcm.o: %.c
	${CXX} ${CXXFLAGS} -o $@ -c $< 

clean:: 
	-rm -f *~ ${OBJS} ${OBJS-P} ${TARGET} ${TARGET-P} 

distclean:: clean
