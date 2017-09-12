#!/bin/bash
printf "Enter command: "

read cmd

if [[ $cmd == "" ]]
then 
  echo "Aborted"
else
  $cmd
fi
