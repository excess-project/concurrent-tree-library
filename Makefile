TARGET  := CBTree 
SRCS    := main.c  
OBJS    := ${SRCS:.c=.o} 
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 

CCFLAGS = -g -fno-omit-frame-pointer -O3 -DNDEBUG -D_REENTRANT -Wall -funroll-loops -fno-strict-aliasing
CXXFLAGS = -g -O3 -DNDEBUG -D_REENTRANT -Wall -funroll-loops -fno-strict-aliasing -I../intelpcm
LDFLAGS = -L../intelpcm
LIBS    = -lm -lpthread

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CCFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

clean:: 
	-rm -f *~ *.o *.dep ${TARGET} 

distclean:: clean
