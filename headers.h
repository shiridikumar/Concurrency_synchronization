#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
typedef struct{
    char course_name[200];
    float interest;
    int max_slots;
    int num_labs;
    int lab_ids[1000];
}courses;
courses *cs;


typedef struct{
    float calibre;
    int pref_1;
    int pref_2;
    int pref_3;
    float time;
}students;

students *st;


typedef struct{
    char lab_name[200];
    int num_ta;
    int max_num;
}labs;

labs *lb;
