## **Opertain systems and networks**
**Question 1**
- How to run the code??
    -  compile the code using "gcc -pthread main.c"
    - run the executable using ./a.out
- The simulation uses pthreads prominently.

- Each student and ,courses are genrated using labs

>Note: all the below mentioned functioanlities are handled using mutex_locks(for all the shared variables) and are not being mentioned expliclity in the report.The below report is just a brief overview of the assignment.

- The simulation runs indefinetly (while loop)until all the students exit the simulation(while stud_done != num_students).

- when a student intially fills for his or her preferences,the thread function calls

> student_filled(i) where i is student index(0 indexed)
- Different courses have different threads.
- Courses conduct tutorial with a buffer delay of 4 seconds.

- Whenever a student fills he waits for the current Course tutorial to be completed the waits for the course to find The TA implemented in 
> conduct_tut(ind) where ind is course index

- This is done using **pthread_cond_wait**

- When a course succesfully finds a TA it unblocks all the student threads waiting for it using **pthread_cond_broadcast**

- TA are alloted on the basis of the constraints mentioned

- If a student is not able to find the TA for his alloted course then he changed current preference.(line 104)

- Finally a student thread waits/blocks using **pthread_cond_wait** and when a conduct_tut finished it unblocks using **pthread_cond_broadcast**

- if the student is satisfied with the course he exits simulations and the stud_done count is incremented

- For sample check you can run as "./a.out <input.txt"

***
**P shiridi kumar**

**2021121005**