#!/bin/bash
FILE=$HOME/.bashrc
RUN=0
COMPILE=0

while getopts "f:cr" opt
do
case $opt in
f) FILE=$OPTARG;;
c) COMPILE=1;;
r) RUN=1;;
esac
done

cat $FILE

if [[ $COMPILE == 1 ]]
then 
gcc $FILE
fi

if [[ $RUN == 1 ]]
then
./a.out
fi
