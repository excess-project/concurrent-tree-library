SRCS    := main.c
TARGET  := CBTree
TREE	:= -DCBTREE

include  ../common/common.mk

lib:
	${CC} -c ${CCFLAGS} main.c -o cbtree.o -DNOT_STANDALONE
	ar rcs libcbtree.a cbtree.o ${PREC}
