#!/bin/bash
FILES=""
I=1

for ARG in $@
do
  if [[ $I == 1 ]]
  then
    CAT="$ARG"
  else
    FILES=$FILES"$ARG "
  fi
I=$[$I+1]
done

tar -czf $CAT/archive.tar.gz $FILES

