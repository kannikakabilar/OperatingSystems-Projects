# RAM-Page-Handler
The RAM Page Handler is a virtual memory simulator. It manages page table operations and performs page replacements. This assignment was done to demonstrate our understanding of the theory behind memory management as part of the Operating Systems course. There are two main tasks in this assignment:<br/>
       1. Implement virtual-to-physical address translation and demand paging using a two-level page table <br/><br/>
       The main driver for the memory simulator, sim.c, reads memory reference traces in the format produced by the fastslim.py tool from valgrind memory traces. The simulator is executed as ./sim -f -m -s -a where memory size and swapfile size are the number of frames of simulated physical memory and the number of pages that can be stored in the swapfile, respectively. The swapfile size should be as large as the number of unique virtual pages in the trace, which you should be able to determine easily. The format of a page table entry is shown below here: <br/> <br/>
    | 31 --- Frame Number --- 12 | 11 --- Unused --- 4 | 3 --- Pg ONSWAP | 2 --- Pg REF | 1 --- Pg DIRTY | 0 --- Pg VALID | 
    
 <br/>
       The swapfile functions are all implemented in the swap.c file, along with bitmap functions to track free and used space in the swap file, and to move virtual pages between the swapfile and (simulated) physical memory. The swap_pagein and swap_pageout functions take a frame number and a swap offset as arguments. The simulator code creates a temporary file in the current directory where it is executed to use as the swapfile, and removes this file as part of the cleanup when it completes.    
<br/><br/>
2. Implement 4 different page replacement algorithms: FIFO, LRU, Clock, OPT (remove the file that has not been used for the longest time). <br/>
The 4 page replacement algorithms were compared and analyzed on its performance as shown below. <br/><br/>

![Quote](https://github.com/kannikakabilar/RAM-Page-Handler/blob/main/analysis_shot.png)
    
