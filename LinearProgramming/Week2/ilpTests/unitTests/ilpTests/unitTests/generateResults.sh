for ((i=1; i <= 10; ++i))
do
    echo ilpTest$i;
    cat preamble.tex > ilpTest$i.tex
    python ../../cuttingPlane.py ilpTest$i | grep -v "Read" >> ilpTest$i.tex;
    echo '\end{document}' >> ilpTest$i.tex
done

for j in `cat toRun`
do
    echo $j;
    cat preamble.tex > $j.tex
    python ../../cuttingPlane.py $j | grep -v "Read" >> $j.tex;
    echo '\end{document}' >> $j.tex
done
