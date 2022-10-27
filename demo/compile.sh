#!/bin/bash

if [ ! -f third_party/cisson/cisson.h ]; then
  echo "there is no cisson.h, start to download it from gitlab."
  mkdir -p third_party/cisson
  curl -L https://gitlab.com/Meithal/cisson/-/raw/master/cisson.h?inline=false -o third_party/cisson/cisson.h
fi
echo "Starting to compile demo program"
if [ -f demo ]; then
  rm -f demo
fi
cc -I./third_party/cisson/ -I./inc/ -I../inc/ ../src/taipower.c ./src/comm.c demo.c -o demo

if [ ! -f demo ]; then
    echo "compile demo fail"
    exit 1
fi

echo -e "\nStarting to run example"
for f in $(ls example/)
do
    echo "--------------- run ${f} ---------------"
    ./demo example/${f}
    echo -e "--------------- end ${f} ---------------\n"
done
