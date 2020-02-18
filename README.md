# binary_process_tree
Execution of queries on a high level binary file with records.

## Description
The processes of the binary process tree execute the search of records in the binary input file.
Each leaf node searches for the wanted records in a distinctive part of the file.
The internal nodes combine the results taken from their children and they forward them to their parents.
The root node sorts the records that form the answer to the query.

## Prerequisites
GCC (g++) compiler.

## Compilation
`$ make clean`

`$ make`

## Usage
`$ ./myfind -d <Datafile> -h <Height> -p <Pattern> (-s)`

Datafile: the binary input file.

Height: the depth of the full binary search tree, 1 < Height < 5.

Pattern: the (sub)string we are searching in the binary file. 

-s: if you skip this flag, every leaf node searches the same amount of records in the file.

## Author
Vasilis Panagakis

## Date
November 2018
