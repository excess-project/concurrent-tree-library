#!/bin/sh

PROGS1=("GBST" "DeltaTree" "hGBST" "CBTree")

PROGS2=("rbtree" "avltree" "sftree" "btrees")

current_dir=$(pwd)
script_dir=$(dirname $0)

if [ $script_dir = '.' ]
then
	script_dir="$current_dir"
fi

echo "Building Binaries..."
echo $BASEDIR

for prg in "${PROGS1[@]}"
do
	echo $prg
	rm $script_dir/$prg
	cd $script_dir/../$prg 
	make clean
	make
	ln -s $script_dir/../$prg/$prg $script_dir/$prg 
done

#Synchrobench

cd ../synchrobench
cd estm-0.3.0
make clean
make

cd ..
make clean
make estm
rm $script_dir/rbtree
rm $script_dir/avltree
rm $script_dir/sftree

ln -s $(pwd)/bin/lf-rt $script_dir/rbtree
ln -s $(pwd)/bin/lf-st $script_dir/avltree
ln -s $(pwd)/bin/lf-st-nest $script_dir/sftree


#NBBST

cd ../NBBST

rm CMakeCache.txt       
rm -R CMakeFiles/
rm cmake_install.cmake

cmake .
make 
ln -s $(pwd)/bin/btrees $script_dir/NBBST

