#!/bin/bash

set -o errexit
set -o nounset

#http://mg.readthedocs.io/latexmk.html

latexmk -pdfps --shell-escape -pvc ${1}
