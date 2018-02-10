#!/bin/bash

#clean and run

latexmk -C main.tex && rm -rf main.bbl && ./runlatexmk.sh

