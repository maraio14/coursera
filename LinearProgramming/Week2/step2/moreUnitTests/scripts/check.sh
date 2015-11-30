#! /bin/bash

for fName in `cat toRun`
do
    echo -n $fName ', '
    echo -n `glpsol --math $fName'.ampl' | tail -n 2 | grep -v "Model"`
    echo -n ' , '
    echo -n `cat $fName'.dict.output' | head -n 1`
    echo "  "
done

