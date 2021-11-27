## **Opertain systems and networks**
**Question 2**
- How to run the code??
    -  compile the code using "gcc -pthread main.c"
    - run the executable using ./a.out
- The simulator uses both semaphores as well as pthread mutex locks .
- entry of the spectators to zones are made using threads (so that they can enter the zones in parllel) using function start_simulation().
- when a spectator tries to enter a zone it waits for the ticket to be available as follows:
> entry_A()

> entry_H()

> entry_N()

- Three semaphores are initalised zone_a,zone_h,zone_n with counter values as ch,cn,ca(which are the capcities of each zones).

- Whenever a person tries to buy a ticket it sem_timedwait for the spectators amount of seconds if there are not tickets it gets blocked and gets signaled once available.

- Simulation starts at t=1sec
- Assumptions and conclusion:
    - A person when leaving a staidum immedately leaves for dinner without any delay.(no bonus)
    - no two goals chances are at the same time
    - All goals are using a single thread (since only one goal at a time).
    - each spectator is genrated using a different thread.

- for sample check you can run as "./a.out <input.txt"

- **Implemented bonus** i.e, people leave for dinner only when all the members of the group leaves the stadium.

***
**P shiridi kumar**

**2021121005**




