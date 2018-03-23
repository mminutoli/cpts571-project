#!/bin/bash

TOOL=../build/tools/suffixtree_tool
TESTS=../tests

$TOOL -i $TESTS/s1.fasta
$TOOL -i $TESTS/s2.fasta
$TOOL -i $TESTS/colorblind_human_gene.fasta
$TOOL -i $TESTS/colorblind_mouse_gene.fasta
$TOOL -i $TESTS/Human-BRCA2-cds.fasta
$TOOL -i $TESTS/Sylco.fas
$TOOL -i $TESTS/chr12.fas

valgrind --tool=massif $TOOL -i $TESTS/s1.fasta
valgrind --tool=massif $TOOL -i $TESTS/s2.fasta
valgrind --tool=massif $TOOL -i $TESTS/colorblind_human_gene.fasta
valgrind --tool=massif $TOOL -i $TESTS/colorblind_mouse_gene.fasta
valgrind --tool=massif $TOOL -i $TESTS/Human-BRCA2-cds.fasta
valgrind --tool=massif $TOOL -i $TESTS/Sylco.fas
valgrind --tool=massif $TOOL -i $TESTS/chr12.fas
