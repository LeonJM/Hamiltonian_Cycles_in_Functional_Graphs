# Hamiltonian Cycles in Families of Functional Graphs of Quadratic Polynomials over Prime Fields
This is the repository as mentioned in the MQU 2022 software engineering thesis titled *Hamiltonian Cycles in Families of Functional Graphs of Quadratic Polynomials over Prime Fields*, written by [me](https://github.com/LeonJM) and supervised by [Professor Bernard Mans](https://researchers.mq.edu.au/en/persons/bernard-mans).

## Overview
This software will find *Hamiltonian cycles* (HC) in a specific kind of graph (see thesis). 

There are two main versions of the software; `mainV4.cpp` and `mainV6.cpp`. 

`mainV4.cpp` utilises multithreading and will, by default, create **as many threads as possible** to find HCs. With each thread computing an instance of unique graph. Each thread computes for HCs through a backtracking algorithm.

`mainV6.cpp` also utilises multithreading, but not as chaotic as v4. For each instance of a graph, `mainV6.cpp` creates two threads to compute for a HC; one thread computes by backtracking, and the other computes by way of the Held-Karp algorithm. Note that for graphs over thirty vertices, this version **will run into memory issues**.

## Usage [^1]
Generally, both versions of the software will prompt the user for $n$ and $k$ in that order.

Then the software will *try* (NP-complete) to do its thing and compute for a HC with that configuration. If it is unable to find one, the software will retry with $k+1$.

Additionally, `mainV4.cpp` has an extra functionality that allows for computation over a range of $n$, instead of just one $n$. Simply prefix the `-` character before $n$.

[^1]:This software is developed for the sole purpose of aiding the thesis. Not intended for general use. This software assumes the user will enter valid inputs (see thesis).