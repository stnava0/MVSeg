#!/bin/sh

for FL in $1  # antstheory # ants
  do 
for i in 1 2 3 
do
pdflatex $FL 
bibtex   $FL
done
# cp ${FL}.pdf ../
done 
rm -f *.bbl *.log *.aux *.toc *.blg *.brf *.out *.spl *~ 

