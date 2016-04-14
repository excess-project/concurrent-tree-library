SRCS    := main.c
PREC	:= dtree.o
TARGET  := DeltaTree
TREE	:= -DDTREE

include  ../common/common.mk

lib: prep ${TARGET}
	ar rcs libdeltatree.a ${PREC}
