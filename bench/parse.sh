#!/bin/bash

contains() { for e in "${@:2}"; do [[ "$e" = "$1" ]] && return 1; done; return 0; }
isnumber() { test "$1" && printf '%f' "$1" >/dev/null 2>&1; }

list="$(find ./$1 -name *$2*.err)"

mkdir -p ./combined
rm ./combined/*$2*

avail_files=()
avail_inits=()
max_iter=0

for file in $list
do
	filename=$(basename "$file")
	extension="${filename##*.}"
	filename="${filename%.*}"
	arr=(${filename//-/ })
	arrLen=${#arr[@]}

	vg=${arr[$arrLen-2]}
	rep=${arr[$arrLen-1]}
	
	if [[ $rep -gt $max_iter ]]
	then
		max_iter=$rep
	fi

	contains ${arr[1]} ${avail_files[@]}
	if [[ $? == 0 ]]
	then
		echo ${arr[1]}
		avail_files+=("${arr[1]}")		
	fi

	init=${arr[$arrLen-3]}

	contains $init ${avail_inits[@]}
        if [[ $? == 0 ]]
        then
            	echo $init
                avail_inits+=($init)
        fi
	
	newfile="./combined/${arr[1]}-$init.csv"
	if [ $vg -ne 1 ]
	then 
		grep "0:" $file > ./combined/${arr[1]}-$init.$rep.dat
	fi
done

echo $max_iter
echo ${avail_files[1]}

mean_start=10
fields=10

for init in ${avail_inits[@]}
do
    for tree in ${avail_files[@]}
    do
	current_line=1
	keep_going=1
	while [[ $keep_going == 1 ]]
	do
		str=""

		for iter in $(seq 1 1 $max_iter)
		do
			str+="$(sed -n ${current_line}p ./combined/$tree-$init.$iter.dat)"
			if [[ $str == "" ]]
			then
				keep_going=0
				break
			fi
			str+=", "
		done
		(( current_line += 1 ))

		str=$(echo $str | tr -d ' ')

		IFS=, read -r -a arr <<< "$str"
		
		#echo "${arr[15]}"

		aggr=""
		
		for iter in $(seq $mean_start 1 $fields)
		do
			sum=0
			numbers=""
			for step in $(seq 1 1 $max_iter)
			do
				tempcol=$((iter + (fields+1) * (step-1)))
				#echo "Tempcol:$tempcol"
				temp=${arr[$tempcol]}
				#echo "Temp: $temp"

				isnumber $temp || break

				sum=$((sum + temp))
				numbers+=$temp$'\n'	
			done

			numbers=$(echo "$numbers" | sed 's/ *$//')

			#echo "$numbers"

			stdev=$(
    				echo "$numbers" |
        			awk '{s+=$1; sumsq+=$1*$1}END{print sqrt(sumsq/NR - (s/NR)**2)}'
			)
			#echo $stdev
			sum=$(echo "$sum/$max_iter" | bc -l)
			
			aggr+="$sum, $stdev"
		done
		#echo MAX_ITER : $max_iter
		echo $str$aggr >> ./combined/$tree-$init.csv

	done
    done
done

