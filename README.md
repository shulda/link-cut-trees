# Link-Cut trees implementation

## Intro

This project contains my implementation of link-cut trees that I did as an programming assignment for my Programming course. It was meant as a piece of library for programming competition, hence the choice of macros etc.

The link_cut_tree.cpp file is a full implementation of a solution to the following problem:

## Problem

Create an algorithm that is able for a given forest on `n` vertices, each with a given integer value, process the following queries:
- `a x y` -- add the edge between vertices `x` and `y`
- `r x y` -- remove the edge between vertices `x` and `y`
- `s x n` -- set the value of vertex `x` to be `n`
- `c x y` -- write `YES` to the STDOUT if vertices `x` and `y` are connected (otherwise output `NO`)
- `u x y d` -- increase the values of all vertices on the unique path between vertices `x` and `y` by `d`
- `m x y` -- output the minimum value on the path between `x` and `y`

We can always assume that the queries are valid (e.g. the edge being removed exists etc.).

### Input format

On the first line on STDIN there are two integers `n and `m`, i.e. the number of vertices in the forest and the number of queries respectively.
On the second line there are `n` integers -- the starting values of vertices `1, 2, ..., n`.
On the third line there are `n` integers `p_1, p_2, ..., p_n`, each from the set `0, 1, ..., n`, where `p_i` is the number of the parent of vertex `i` (or 0 if `i` has no parent)
Then on each of the following `m` lines there is a single query specified above.

## Files

The project contains three files

### `input_generator.cpp`

This file generates random inputs for the problem. It has a couple of command line parameters:
`input_generator n m [forrest_coeff [tree_width_coeff [alter_structure]]]`
where `n` is the number of vertices in the forest, `m` is the number of queries. `forrest_coeff` (default value = `0.0003`) denotes the probability that a vertex >1 will have no parent (vertex 1 never has a parent). `tree_width_coeff` (default value = 10) determines the expected depth of the forest. Parent of vertex `i` is chosen as `parent[i] = rnd(i - tree_width_coeff, i-1)`. For `tree_width_coeff=1` it generates a path, for `tree_width_coeff=n` the expected depth is only logarithmical. Finally `alter_structure` is a flag whether to generate queries `a x y` and `r x y` (they are generated randomly and thus the depth of the trees becomes logarithmical quite fast).

### `bruteforce.cpp`

This file is a bruteforce which solves the stated problem. It has worst case complexity `O(n)` for operation.

### `link_cut_tree.cpp`

The main file with LC trees implementation. The LC trees are implemented as taught on the 2016 course *Data Structures II* at Faculty of Mathematics and Physics, Charles University. That means that the implementation consists of a forest of Splay trees connected by thin edges.

The main purpose of the file was to create a easily reusable code for programming competitions such as ACM-ICPC, which means that the code tries to be concise, does use some global variables and is all in one file.

It consists of three main parts: The Splay trees implementation (supports lazy updates), representation of thick paths in LC-forest and representation of the forest itself using decomposition to thick paths.