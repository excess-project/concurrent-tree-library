#!/bin/sh

PROGS=("GBST" "DeltaTree" "hGBST" "CBTree" "rbtree" "sftree" "citrus" "nata" "NBBST")

INITS=('65535')

REPEAT=1
VGMODE=0

echo "STARTING BENCHMARK"

mkdir results

for prg in "${PROGS[@]}"
do
	for init in "${INITS[@]}"
	do
		for rep in `seq 1 $REPEAT`
		do
			echo "$prg with Initial $init values, Iteration $rep"
			sh ./bench-small.sh $prg $init $VGMODE $rep 1>results/runall-$prg-$init-$VGMODE-$rep.out 2>results/runall-$prg-$init-$VGMODE-$rep.err
		done
		sh ./parse.sh results/ $init
	done 
done

sh ./plot-min.sh $init
