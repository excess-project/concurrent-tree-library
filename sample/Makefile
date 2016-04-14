.PHONY: all clean

all: test_cbtree test_greenbst test_deltatree test_bluebst

../CBTree/libcbtree.a:
	cd ../CBTree && $(MAKE) lib

../GreenBST/libgreenbst.a:
	cd ../GreenBST && $(MAKE) lib

../DeltaTree/libdeltatree.a:
	cd ../DeltaTree && $(MAKE) lib

../BlueBST/libbluebst.a:
	cd ../BlueBST && $(MAKE) lib

test_cbtree: ../CBTree/libcbtree.a lib_test.c
	${CC} -O3 -o test_cbtree lib_test.c -DMAP_USE_CBTREE -I../common -L../CBTree -lcbtree -lpthread -lm

test_greenbst: ../GreenBST/libgreenbst.a lib_test.c
	${CC} -O3 -o test_greenbst lib_test.c -DMAP_USE_GBST -I../common -L../GreenBST -lgreenbst -lpthread -lm

test_deltatree: ../DeltaTree/libdeltatree.a lib_test.c
	${CC} -O3 -o test_deltatree lib_test.c -DMAP_USE_DTREE -I../common -L../DeltaTree -ldeltatree -lpthread -lm

test_bluebst: ../BlueBST/libbluebst.a lib_test.c
	${CC} -O3 -o test_bluebst lib_test.c -DMAP_USE_BBST -I../common -L../BlueBST -lbluebst -lpthread -lm

clean:
	rm test_cbtree test_greenbst test_deltatree test_bluebst
