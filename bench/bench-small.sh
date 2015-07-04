#!/bin/sh -f

seed=0
initial=$2
cmd=$1
range=$((initial*2))
valgrind=$3

if [ $valgrind -eq 0 ]; then
	echo "STARTING $cmd $initial"

	for update_rate in 0 20 50
	do
		for i in 1 6 12 18 24
		do
   			./$cmd -s $seed -i $initial -n $i -u $update_rate -r $range -f 0 $extra #-t 1023
			sleep 1
		done
	done
else
	extra="-v"
	for update_rate in 0 50
        do
          	for i in 1 8 16
		do
                        mkdir -p $PBS_O_WORKDIR/perf/$cmd/$cmd-$initial-$update_rate-$i.out.$4
			#/home/ibrahim/local/bin/operf -e CPU_CLK_UNHALTED:100000,LLC_MISSES:100000,LLC_REFS:100000,BR_INST_RETIRED:100000,BR_MISS_PRED_RETIRED:100000 -d $PBS_O_WORKDIR/perf/$cmd/$cmd-$initial-$update_rate-$i.out.$4 $PBS_O_WORKDIR/$cmd -s $seed -i $initial -n $i -u $update_rate -r $range -f 0

			/usr/bin/perf record -f -g -e cycles:u,branches:u,branch-misses:u,cache-references:u,cache-misses:u -o $PBS_O_WORKDIR/perf/$cmd/$cmd-$initial-$update_rate-$i.out $PBS_O_WORKDIR/$cmd -s $seed -i $initial -n $i -u $update_rate -r $range -f 0 $extra

                        sleep 1
        	done
	done
fi

