#!/bin/sh

PROGS1=("GreenBST" "DeltaTree" "BlueBST" "CBTree" "BSTTK" "citrus" "LFBST" "SVEB")

#PROGS2=("rbtree" "avltree" "sftree" "btrees")

current_dir=$(pwd)
script_dir=$(dirname $0)

if [ $script_dir = '.' ]
then
	script_dir="$current_dir"
fi

# load module environment on cluster
. $HOME/.bashrc

module load compiler/gnu/4.8.2
module load tools/intel-pcm/2.7
module load papi/5.4.1

export CC=gcc
export CXX=g++

echo "Building Binaries..."
echo $BASEDIR

for prg in "${PROGS1[@]}"
do
	echo $prg
	rm "$script_dir/$prg"
	cd "$script_dir/../$prg"
	make clean
	make
	ln -s "$script_dir/../$prg/$prg" "$script_dir/$prg"
done

#Synchrobench

cd "$script_dir/../synchrobench/c-cpp"
make clean
make estm
#make spinlock
#make lockfree

rm "$script_dir/rbtree"
rm "$script_dir/sftree"
#rm "$script_dir/nata"
#rm "$script_dir/citrus"


ln -s "$(pwd)/bin/ESTM-rbtree" "$script_dir/rbtree"
ln -s "$(pwd)/bin/ESTM-specfriendly-tree" "$script_dir/sftree"
#ln -s "$(pwd)/bin/lockfree-bst" "$script_dir/nata"
#ln -s "$(pwd)/bin/SPIN-RCU-tree $script_dir/citrus"


#NBBST

cd "$script_dir/../NBBST"

rm CMakeCache.txt
rm -R CMakeFiles/
rm cmake_install.cmake

cmake .
make
ln -s "$(pwd)/bin/btrees" "$script_dir/NBBST"
