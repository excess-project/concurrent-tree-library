SRCS    := gbst.c main.c gbstlock.c
#PREC	:= gbst.o
TARGET  := GreenBST
TREE	:= -fPIC -DGBST -D__PREALLOCGNODES=4095


include  ../common/common.mk

lib: libgreenbst.a

libgreenbst.a: prep ${TARGET}
	ar rcs libgreenbst.a gbstlock.o gbst.o
