#!/bin/bash

#clean and run

FILE=""
if [ $# -eq 0 ]
  then
  FILE="RNOSchlumberger"
else
  FILE="${1}"
fi

latexmk -C ${FILE}.tex \
  && rm -rf ${FILE}.bbl \
  && ./runlatexmk.sh ${FILE}.tex

