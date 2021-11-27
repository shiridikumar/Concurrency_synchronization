#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
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
    int filled;
}students;
students *st;


typedef struct{
    char lab_name[200];
    int num_ta;
    int max_num;
    int curr_ta;
    int nta;
}labs;

labs *lb;
time_t seconds;
long long start_time;
int student_done=0;
int *curr_course;
int num_students,num_labs,num_courses;
pthread_mutex_t student_mutex;
sem_t *tut;
