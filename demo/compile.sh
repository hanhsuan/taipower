#!/bin/bash

if [ ! -f cisson.h ]; then
  echo "there is no cisson.h, start to download it for gitlab."
  curl -L https://gitlab.com/Meithal/cisson/-/raw/master/cisson.h?inline=false -o cisson.h
fi
echo "Starting to compile demo program"
if [ -f demo ]; then
  rm -f demo
fi
cc -I./ -I../inc/ demo.c ../src/taipower.c -o demo

echo -e "\nStarting to run example"
for f in $(ls example/)
do
    echo "--------------- run ${f} ---------------"
    ./demo example/${f}
    echo -e "--------------- end ${f} ---------------\n"
done;
