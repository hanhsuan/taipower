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

if [ ! -f third_party/libsagui/include/sagui.h ]; then
   echo "there is no libsagui, start to download it from github."
   curl -L https://github.com/risoflora/libsagui/releases/download/v3.4.0/libsagui-3.4.0-linux_amd64.tar.gz -o libsagui-3.4.0-linux_amd64.tar.gz
   mkdir -p third_party/libsagui
   tar xzvf libsagui*.tar.gz -C third_party/libsagui --strip 1
   rm -f libsagui*.tar.gz
fi

if [ -f apiserver ]; then
  rm -f apiserver
fi
cc -I./third_party/cisson/ -I./third_party/libsagui/include -I./inc/ -I../inc/ -L./third_party/libsagui/lib64 ../src/taipower.c ./src/comm.c apiserver.c -o apiserver -lsagui

if [ ! -f apiserver ]; then
    echo "compile demo fail"
    exit 1
fi
