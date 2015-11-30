#! /bin/bash
echo 'Name, OurTool, GLPSOL' > test.csv

for i in `cat toRun`
do
echo $i 
echo -n $i >> test.csv
echo -n "," >> test.csv
python ../../../solve.py $i.dict > $i.res1;
cp $i.dict.output $i.output
cp preamble.tex $i.tex
python ../../../solveUsingDual.py $i.dict 2>&1 >> $i.tex;
echo "\\end{document}" >> $i.tex
sdiff -s $i.output $i.dict.output
echo ===============
echo -n `cat $i.output`  >>  test.csv
echo -n ',' >> test.csv
echo `glpsol --math $i.ampl | tail -n 4 | grep -v "Model" | grep -v "Time" | grep -v "Memory" | grep -v "Display"` >> test.csv
 
done
