#!/bin/bash

#http://mg.readthedocs.io/latexmk.html

latexmk -pdfps --shell-escape -pvc ${1}
