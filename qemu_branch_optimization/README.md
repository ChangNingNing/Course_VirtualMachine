# Qemu 0.13.0 Branch Optimization
### Note in Implementation
* Data structure of *shadow stack*
    * Use a structure, *shadow_pair*, to store the *guest_eip* and *host_eip*.
    * A structure of array, *shadow_hash_list*, to maintain the array of *shadow_pair*.
    * A stack, *shadow_stack*, stores the address of the corresponding *shadow_pair*.
    * *shadow_pair* is a link-list to deal with the *collision*.
* *pop_shack()* function
    * The number of *pop* would more than the number of *push* about 1, since it would not call *push* while calling the main program but it would call *pop* while leaving.
    * Take more attention on the assembly code when dealing with
    *   ```
        (ptr)->element1
        (*ptr).element1
        ```
* Benchmark - Coremark_v1.0 compile
	* *make PORT_DIR=linux clean*
	* *make PORT_DIR=linux PORT_CFLAGS="-O2 -m32"*

