#!/bin/bash

printf "Compilation de DepthPic.cpp...\n"
g++ -lOpenNI -I /usr/include/ni/ DepthPic.cpp -o DepthPic
printf "Compilation réussie !\n"
printf "Compilation de example.cpp...\n"
cmake .
make
printf "Compilation réussie !\n"
./DepthPic
gnuplot DepthPic.plt
gpicview DepthPic.png
./opencv_example DepthPic.png

 
