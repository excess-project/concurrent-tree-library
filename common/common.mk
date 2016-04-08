CMN_INC	:= ../common

ARCH:=$(shell uname -p)

#Location of profiler library and include (e.g., PAPI)
PROF_LIB:= /opt/papi/5.4.1/lib
PROF_INC:= /opt/papi/5.4.1/include

#Location of Intel PCM library
PCM_DIR:= /opt/tools/intel-pcm/2.7

#Location of GEM5
GEM5_DIR := ../m5

#Addon (default) files
ADDONS	:= ${CMN_INC}/barrier.c ${CMN_INC}/locks.c ${CMN_INC}/bench.c

#Profiling FLAGS, LIBS, ADDONS
SRCPROF = ${CMN_INC}/papicounters.c
ADDFLAG = -I${PROF_INC} -D__USEPROF -D__MULTIPLEX
ADDLD   = -L${PROF_LIB} -Wl,-rpath=${PROF_LIB}
PROFLIB = -lpapi

#Default build objects
OBJS    := ${SRCS:.c=.o} ${ADDONS:.c=.o} 

#Default CFLAGS and LDFLAGS
CCFLAGS = -g -O3 -DNDEBUG -D_REENTRANT -Wall -funroll-loops -fno-strict-aliasing -I${CMN_INC} ${EXTRAC}
LIBS = -lm -lpthread ${EXTRAL}

#Test flags
TEST_CCFLAGS := -D__TEST
TEST_OBJS    := ${SRCS:.c=.o.test} ${ADDONS:.c=.o.test} 

#Energy FLAGS, LIBS, ADDONS
ENE_CCFLAGS := -D__ENERGY

ifeq (${ARCH}, x86_64)
ENE_CC = ${CXX}
SRCENE   = ${CMN_INC}/pcmpower.c 
ENE_CCFLAGS += -std=c++11 -I${PCM_DIR}/include -fpermissive
#ENE_LDFLAGS += -Wl,-rpath=${PCM_DIR}/lib/intelpcm.so -L${PCM_DIR}/intelpcm.so -lintelpcm 
ENE_LDFLAGS += ${PCM_DIR}/lib/client_bw.o  ${PCM_DIR}/lib/cpucounters.o  ${PCM_DIR}/lib/msr.o  ${PCM_DIR}/lib/pci.o
endif

ifeq (${ARCH}, armv7l)
ENE_CC = ${CC}
SRCENE = ${CMN_INC}/armpower.c
endif

ifeq (${ARCH}, unknown)
ENE_CC = ${CC}
SRCENE = ${CMN_INC}/micpower.c
endif

ENE_OBJS += ${SRCENE:.c=.o.ene} ${SRCS:.c=.o.ene} ${ADDONS:.c=.o.ene}

#Simulator (GEM5) FLAGS, LIBS, ADDONS

GEM5_DIR_THREAD := ${GEM5_DIR}/m5thread
GEM5_DIR_OPS	:= ${GEM5_DIR}/m5ops
SIM_CCFLAGS += -D__SIM -I${GEM5_DIR_THREAD} -I${GEM5_DIR_OPS} #-DM5OP_ADDR=0xFFFF0000
SIM_LDFLAGS := -static
SIM_LIBS	:= -lm
SIM_OBJS := ${GEM5_DIR_THREAD}/pthread.o ${GEM5_DIR_OPS}/m5op_x86.o


#Profile FLAGS, LIBS, ADDONS

PROF_OBJS += ${SRCPROF:.c=.o.prof} ${SRCS:.c=.o.prof} ${ADDONS:.c=.o.prof}
PROF_CCFLAGS += ${ADDFLAG}
PROF_LDFLAGS += ${ADDLD} ${PROFLIB}


#BUILD

.PHONY: all clean
all:: copy_obj ${TARGET} ${TARGET}.energy ${TARGET}.profile 

#Plain

copy_obj:
	cp ./obj/* .

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^ ${PREC} ${LIBS} 

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} ${TREE} -o $@ -c $< 

#Energy

${TARGET}.energy: ${ENE_OBJS}
	${ENE_CC} ${ENE_LDFLAGS} ${LDFLAGS} -o $@ $^ ${PREC}.ene ${LIBS} 

${ENE_OBJS}: %.o.ene: %.c
	${ENE_CC} ${ENE_CCFLAGS} ${CCFLAGS} ${TREE} -o $@ -c $< 

#Profile

${TARGET}.profile: ${PROF_OBJS}
	${CC} ${PROF_LDFLAGS} ${LDFLAGS} -o $@ $^ ${PREC}.prof ${LIBS} 

${PROF_OBJS}: %.o.prof: %.c
	${CC} ${PROF_CCFLAGS} ${CCFLAGS} ${TREE} -o $@ -c $< 


#Simulator
#
#${TARGET}.sim: ${SIM_OBJS}
#	${CC} ${SIM_LDFLAGS} ${LDFLAGS} -o $@ $^ ${SIM_LIBS} 
#
#${GEM5_DIR_THREAD}/pthread.o: ${GEM5_DIR_THREAD}/pthread.c ${GEM5_DIR_THREAD}/pthread_defs.h ${GEM5_DIR_THREAD}/tls_defs.h
#	$(CC) $(CCFLAGS) -c ${GEM5_DIR_THREAD}/pthread.c -o ${GEM5_DIR_THREAD}/pthread.o
#
#${GEM5_DIR_OPS}/m5op_x86.o: ${GEM5_DIR_OPS}/m5op_x86.S
#	$(CC) -O2 -c ${GEM5_DIR_OPS}/m5op_x86.S -o ${GEM5_DIR_OPS}/m5op_x86.o	
#
#Test
#
#${TARGET}.test: ${TEST_OBJS}
#	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 
#
#${TEST_OBJS}: %.o.test: %.c
#	${CC} ${CCFLAGS} ${TEST_CCFLAGS} ${TREE} -o $@ -c $< 


clean:: 
	-rm -f *~ ${OBJS} ${TEST_OBJS} ${PREC} ${PREC}.ene ${PREC}.prof ${SIM_OBJS} ${ENE_OBJS} ${PROF_OBJS} ${TARGET} ${TARGET}.test ${TARGET}.sim ${TARGET}.profile ${TARGET}.energy 
