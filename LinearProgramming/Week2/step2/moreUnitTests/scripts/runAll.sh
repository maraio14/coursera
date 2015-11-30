#! /bin/bash

for fName in `cat toRun`
do
    dictName=$fName'.dict'
    cat preamble.tex > $fName'.tex'
    python ../../solve.py $dictName >> $fName'.tex'
    echo '\end{document}' >> $fName'.tex'
    latexmk --pdf $fName'.tex'
    latexmk -c $fName'.tex'

done
