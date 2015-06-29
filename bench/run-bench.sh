#!/bin/sh

PROGS=("GBST" "DeltaTree" "hGBST" "CBTree" "rbtree" "avltree" "sftree" "NBBST")

#INITS=('2500000' '4194303')

INITS=('4194303')

REPEAT=5
VGMODE=0

echo "STARTING BENCHMARK"

mkdir results

for prg in "${PROGS[@]}"
do
	for init in "${INITS[@]}"
	do
		for rep in `seq 1 $REPEAT`
		do
			echo arg1="$prg",arg2=$init,arg3=$vg,arg4=$rep
			sh ./bench.sh $prg $init $VGMODE $rep 1>results/runall-$prg-$init-$VGMODE-$rep.out 2>results/runall-$prg-$init-$VGMODE-$rep.err
		done
	done 
done

rm -R combined
sh ./parse.sh results

rm -R charts
sh ./plot.sh
