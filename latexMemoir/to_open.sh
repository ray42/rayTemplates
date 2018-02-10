#!/bin/bash

# First we genlabels
cd Contents
grep -n "label{" *.tex --exclude=contents.tex > contents.tex
cd ../Images
find ./  -printf "%f\n" >> ./../Contents/contents.tex
cd ..

gvim -p Contents/contents.tex \
main.tex \
Contents/LeetCodeAlgo1.tex \

#gvim -p Contents/contents.tex \
#main.tex \
#Contents/introduction.tex \
#Contents/DynmcPrgrmmngPt1.tex \
#Contents/DynmcPrgrmmngPt2.tex \
#Contents/DynmcPrgrmmngPt3.tex \
#Contents/DynmcPrgrmmngPt4.tex \
#Contents/LongestPalinSubstring.tex \








