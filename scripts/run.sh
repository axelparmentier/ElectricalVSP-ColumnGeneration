#!/bin/bash

# Print help
if [ ! $# == 3 ]; then
        echo "This script runs a batch of instances."
        echo "Usage: $0 <instances-file> <command-args> <suffix>"
        echo "Where:"
        echo "  instances-file: Text file containing a list of instances."
        echo "  command-args: Command arguments to pass to the executable. They should be between \"\"."
        echo "  suffix: Suffix to append to output folder. Usually it is something regarding the parameters."
        echo "Example:"
        echo "  $0 instances/kshs.txt \"cg -hp ng -ep ng -ng 8 -cuts\" ng8-cuts"
        exit
fi

file=$1
commands=$2
suffix=$3

timestamp=`date -u +%Y.%m.%d.%H.%M.%S`
hostname=`hostname`
dir=out/$timestamp-$hostname-$suffix

mkdir -p out
mkdir -p $dir

~/libs/telegram/telegram -C ">>> Starting EVSP test ${dir} <<<"
while read -r line;
do
	temp=${line##*/}
	out="./$dir/${temp%.*}.txt"
	sem -j 5 timeout -k 15s 360m ./release/evsp ${line} $2 > ${out}
done < $1
sem --wait

~/libs/telegram/telegram -C ">>> Finished EVSP test ${dir} <<<"

