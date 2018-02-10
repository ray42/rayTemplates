#!/bin/bash


## First part is word count via texcount, then I examined the output and
## extracted the second line.
## This is reliant on the format of texcount, which currently works on my
## laptop.
NWORDS=$(texcount -1 -sum -inc ./main.tex | sed -n '2p')


#echo "${NWORDS}"

echo "\\def\\wordcount{$NWORDS}"

#NOW=$(date +"%c")
#echo "Hello, date is: $NOW"





