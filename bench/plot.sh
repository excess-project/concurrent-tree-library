#!/bin/bash

PROGS=("GBST" "DeltaTree" "hGBST" "CBTree" "rbtree" "sftree" "citrus" "nata" "NBBST")
PROG_NAME=("GBST" "DeltaTree" "hGBST" "CBTree" "rbtree" "sftree" "RCU" "Natarajan" "NBBST")


INITS=($1)

MAX_THREAD=24

#FOR ARM (4 cores w/ 100%)
#UPD_RATES=(1  3  5  10 20 50 100)
#START=(24 20 16 0  4  12  28)
#END=(27 23 19 3  7  15  31)
#SEARCH_START=8
#SEARCH_END=11
#

UPD_RATES=(20 50)
START=(24 48)
END=(47 71)

SEARCH_START=0
SEARCH_END=23

AVG=0
SD=0

#Now Create Graph

nl='\\n'
str=""

for init in "${INITS[@]}"
do

AVG=56
SD=57

j=0

str="set datafile separator \",\"\n"
str=$str"set term epslatex newstyle standalone color dashed 8 \
header ' \\\renewcommand{\\\familydefault}{\\\sfdefault}\
\\\usepackage{sansmath}\
\\\usepackage{helvet}\
\\\sansmath' \n"

str=$str"set output \"perf-${init}.tex\"\n"
str=$str"set multiplot layout 2,2\n"

str=$str"unset xlabel\n\
set ylabel \"operations/second\" \n"

str=$str"set xlabel 'no. of threads'\n"

str=$str"unset key\n\

set pointsize 0.7\n

set format y \"\$%.1t*10^{%T}\$\"\n\

set xrange [1:${MAX_THREAD}]\n\

set xtics nomirror\n"
#set ytics nomirror\n"


if [ $j -eq "0" ]; then
    #str=$str"\nset title \" \"\n"
    str=$str"\nset label 1 '100\\% Search'\n"
    str=$str"set label 1 at graph 0.05, 0.85\n"
else
    sr=`expr 100 - $up`
    str=$str"\nunset title\n"
    str=$str"\nset label 1 '${sr}\\% Search'\n"
    str=$str"set label 1 at graph 0.05, 0.85\n"
fi


if [ $j -eq "0" ]; then
str=$str'plot '
fi
i=0
for prg in "${PROGS[@]}"
do
	if [ $j -eq "0" ]; then
		str=$str"\"./combined/${prg}-${init}.csv\" every ::${SEARCH_START}::${SEARCH_END} using 4:((\$5+\$6+\$7)/(\$${AVG}/1000)) notitle w lines ls $((i+1)),"
		str=$str"\"./combined/${prg}-${init}.csv\" every ::${SEARCH_START}::${SEARCH_END} using 4:((\$5+\$6+\$7)/(\$${AVG}/1000)):((\$5+\$6+\$7)/(\$${AVG}/1000))*(\$${SD}/\$${AVG}) title '${PROG_NAME[${i}]}' w yerrorbars ls $((i+1)),"
	fi
	i=`expr $i + 1`
done


for up in "${UPD_RATES[@]}"
do


    str=$str"\nunset title\n"
    str=$str"\nset label 1 '${up}\\% Update'\n"
    str=$str"set label 1 at graph 0.05, 0.85\n"


	str=$str"set xlabel 'no. of threads'\n

	set xrange [1:${MAX_THREAD}]\n\
	set xtics nomirror\n"

    str=$str'plot '
    i=0
    for prg in "${PROGS[@]}"
    do
        if [ $i -ne "0" ]; then
            str=$str", "
        fi

        str=$str"\"./combined/${prg}-${init}.csv\" every ::${START[$j]}::${END[$j]} using 4:((\$5+\$6+\$7)/(\$${AVG}/1000)) notitle  w lines ls $((i+1)),"
        str=$str"\"./combined/${prg}-${init}.csv\" every ::${START[$j]}::${END[$j]} using 4:((\$5+\$6+\$7)/(\$${AVG}/1000)):((\$5+\$6+\$7)/(\$${AVG}/1000))*(\$${SD}/\$${AVG}) title '${PROG_NAME[${i}]}' w yerrorbars ls $((i+1))"

        i=`expr $i + 1`
    done


j=`expr $j + 1`
done

str=$str"\n\

unset border\n\
unset tics\n\
unset label\n\
unset xlabel\n\
unset ylabel\n\
unset arrow\n\
unset title\n\
unset object\n\

set key box\n\
set key spacing 1.1\n\
set key right \n\

set xrange [-1:1]\n\
set yrange [-1:1]\n\
plot NaN w linespoints ls 1 title '${PROG_NAME[0]}'"

for (( i=1; i<${#PROG_NAME[@]}; i++ ));
do
    str=$str", NaN w linespoints title '${PROG_NAME[$i]}' ls $((i+1)) lw .7 pointtype $((i+1))"
done

str=$str"\n\
unset multiplot\n\
"

echo -e $str > plot-${init}
gnuplot plot-${init}

rm plot-${init}
mkdir charts
mv perf-${init}* charts/

cd charts

/usr/bin/pdflatex perf-${init}.tex

done

