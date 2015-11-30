#!/bin/sh
echo -n "Name, My solution, GLPSOL " > out.csv
for ((i=1; i <= 10; ++i))
do
    echo  -n "ilpTest$i, " >> out.csv
    echo -n `cat ilpTest$i.output`, >> out.csv
    echo  `glpsol --math --cuts ilpTest$i.math | tail -n 3 | grep -v "Time" | grep -v "Memory" | grep -v "Display" | grep -v "Model" ` >> out.csv 
done

for j in `cat toRun`
do
    
   echo  -n "$j, " >> out.csv
    echo -n `cat $j.output`, >> out.csv
    echo  `glpsol --math --cuts $j.math | tail -n 3 | grep -v "Time" | grep -v "Memory" | grep -v "Display" | grep -v "Model" ` >> out.csv 
done

