#!/bin/bash

TOOL=../build/tools/suffixtree_tool
TESTS=../tests
OUTPUTS=../output-files

for i in s1.fasta s2.fasta colorblind_human_gene.fasta colorblind_mouse_gene.fasta; do
    echo $TOOL -i $TESTS/$i --stats=$OUTPUTS/$i.stats --bwt=$OUTPUTS/$i.bwt --post=$OUTPUTS/$i.post --dfs=$OUTPUTS/$i.dfs --lcs ">" $OUTPUTS/$i.lcs;
    $TOOL -i $TESTS/$i --stats=$OUTPUTS/$i.stats --bwt=$OUTPUTS/$i.bwt --post=$OUTPUTS/$i.post --dfs=$OUTPUTS/$i.dfs --lcs > $OUTPUTS/$i.lcs;
done

for i in Human-BRCA2-cds.fasta Sylco.fas chr12.fas; do
    echo $TOOL -i $TESTS/$i --stats=$OUTPUTS/$i.stats --bwt=$OUTPUTS/$i.bwt --lcs ">" $OUTPUTS/$i.lcs;
    $TOOL -i $TESTS/$i --stats=$OUTPUTS/$i.stats --bwt=$OUTPUTS/$i.bwt --lcs > $OUTPUTS/$i.lcs;
done


