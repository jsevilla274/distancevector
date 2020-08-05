# Distance Vector Network Simulation
Implementation of the Distance Vector algorithm explained in Chapter 4 of Computer Networking: A Top-Down Approach 6th Edition by James Kurose and Keith Ross using the assigment files and example graph given in https://media.pearsoncmg.com/aw/aw_kurose_network_3/labs/lab6/lab6.html

The bulk of what I have written is in the nodeutil.c with additional explanations on how nodes are handled in node0.c. There is a constant in prog3.c called **LINKCHANGES** set to 1 by default which enables a link cost change between nodes 0 and 1 at times 10000 and 20000. This may be turned off by setting the value to 0. When running, I recommend you input `TRACE level: 2` so that you are able to see packet contents and arrival times.

### To compile using gcc:
`gcc prog3.c node0.c node1.c node2.c node3.c nodeutil.c -o prog3`

This program only makes use of stdio.h and stdlib.h and should be capable of running on most machines with a C compiler.