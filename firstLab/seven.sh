#!/bin/bash
ls -lahX | grep .c

printf "Enter filename: "
read FILE

if [[ $FILE == "" ]] 
then
  echo "Aborted"
  exit 1
else
  gcc $FILE
  ./a.out
fi
