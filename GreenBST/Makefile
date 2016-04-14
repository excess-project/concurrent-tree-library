SRCS    := main.c gbstlock.c
PREC	:= gbst.o
TARGET  := GreenBST
TREE	:= -DGBST -D__PREALLOCGNODES=4095


include  ../common/common.mk

lib: libgreenbst.a

libgreenbst.a: prep ${TARGET}
	ar rcs libgreenbst.a gbstlock.o ${PREC}
