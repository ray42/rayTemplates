#!/bin/bash

set -o errexit
set -o nounset


#http://mg.readthedocs.io/latexmk.html

latexmk -pdf --shell-escape -pvc ${1}
