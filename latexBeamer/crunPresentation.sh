#!/bin/bash

set -o errexit
set -o nounset

#clean and run

FILE=""
if [ $# -eq 0 ]
  then
  FILE="presentation"
else
  FILE="${1}"
fi

latexmk -C ${FILE}.tex \
  && rm -rf ${FILE}.bbl \
  && ./runlatexmkPresentation.sh ${FILE}.tex

