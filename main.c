#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int started=0;
pthread_mutex_t str;
pthread_mutex_t *lblock;
typedef struct{
    char course_name[200];
    float interest;
    int max_slots;
    int num_labs;
    int lab_ids[1000];
    int curr_stud;
    int curr_slots;
    int found;
}courses;
courses *cs;
typedef struct{
    float calibre;
    int pref_1;
    int pref_2;
    int pref_3;
    float time;
    int filled;
    int done;
}students;

students *st;

int *crm;
typedef struct{
    char lab_name[200];
    int num_ta;
    int max_num;
    int curr_ta;
    int nta;
    int *tas;
    int *tas_avail;
}labs;

labs *lb;
time_t seconds;
long long start_time;
int student_done=0;
int *curr_course;
int num_students,num_labs,num_courses;
pthread_mutex_t student_mutex;
sem_t *tut;
int *cou;
pthread_mutex_t *cour_locks;
pthread_mutex_t cslock;
pthread_t *cour_threads; 
int *probs;
sem_t *stut;
int split(char *c, char **arr)
{
    int coun = 0;
    int prev = 0;
    for (int i = 0; i < strlen(c); i++)
    {
        if (c[i] == ' ')
        {
            strncpy(arr[coun], c + prev, i - prev);
            prev = i + 1;
            coun++;
        }
    }
    strncpy(arr[coun], c + prev, strlen(c) - prev);
    coun++;
    return coun;
}


int student_filled(int i){
    printf("student %d has filled in preferences for course registration\n",i);
    int pref_1=st[i].pref_1;
    int prob1 =(rand()%cs[pref_1].max_slots)+1;
    pthread_mutex_lock(&cour_locks[pref_1]);
    if(cou[pref_1]!=1){
        cou[pref_1]=1;
        probs[pref_1]=prob1;
        pthread_mutex_unlock(&cour_locks[pref_1]);
        sem_post(&tut[pref_1]);
        sem_wait(&stut[pref_1]);

    }
    else{
        pthread_mutex_unlock(&cour_locks[pref_1]);
    }
    pthread_mutex_lock(&cour_locks[pref_1]);
    if(cs[pref_1].curr_slots<probs[pref_1] && cs[pref_1].found==1){
        cs[pref_1].curr_slots++;
        printf(GREEN "student %d has been allocated a seat in course %s\n" RESET,i,cs[pref_1].course_name);
        pthread_mutex_unlock(&cour_locks[pref_1]);
        st[i].done=1;
    }
    else{
        pthread_mutex_unlock(&cour_locks[pref_1]);
        printf( RED "fuck off there is an error in code %d  %s curr %d\n" RESET,i,cs[pref_1].course_name,cs[pref_1].curr_slots);
    } 

}

int time_now(){
    return (time(&seconds)-start_time);
}

int conduct_tut(int ind){
    sem_wait(&tut[ind]);
    for(int i=0;i<cs[ind].num_labs;i++){
        pthread_mutex_lock(&lblock[cs[ind].lab_ids[i]]);
        for(int j=0;j<lb[cs[ind].lab_ids[i]].num_ta;j++){
            if(lb[cs[ind].lab_ids[i]].tas[j]!=lb[cs[ind].lab_ids[i]].max_num){// && lb[cs[ind].lab_ids[i]].tas_avail[j]){
                lb[cs[ind].lab_ids[i]].tas[j]++;
                lb[cs[ind].lab_ids[i]].tas_avail[j]=0;
                cs[ind].found=1;
                break;
            }
        }
        pthread_mutex_unlock(&lblock[cs[ind].lab_ids[i]]);
    }
    pthread_mutex_lock(&cour_locks[ind]);
    pthread_mutex_unlock(&cour_locks[ind]);
    printf(YELLOW "course %s has been allocated %d seats\n" RESET,cs[ind].course_name,probs[ind]);
    sem_post(&stut[ind]);
    sleep(1);
    pthread_mutex_lock(&cour_locks[ind]);
    printf(BLUE "Tutorial has been started for course %s with %d seats filled out of %d\n" RESET,cs[ind].course_name,cs[ind].curr_slots,probs[ind]);
    pthread_mutex_unlock(&cour_locks[ind]);
}

int fill_slots(){
    pthread_t stu[num_students];
    start_time=time(&seconds);
    while(student_done!=num_students){
        int nt=time_now();
        for(int i=0;i<num_students;i++){
            if(st[i].time==nt && st[i].filled==0){
                pthread_create(&stu[i],NULL,student_filled,(i));
                st[i].filled=1;
                student_done+=1;
            }
        }
    }
    for(int i=0;i<num_students;i++){
        pthread_join(stu[i],NULL);
    }
    for(int i=0;i<num_courses;i++){
       pthread_join(cour_threads[i],NULL);
    }
}


int main()
{
    char *inp;
    inp = (char *)malloc(1000 * sizeof(char));
    char* startline;
    startline=(char *)malloc(1000*sizeof(char));
    gets(startline);
    char **arr2;
    arr2=(char **)malloc(1000*sizeof(char *));
    for(int i=0;i<1000;i++){
        arr2[i]=(char *)malloc(1000*sizeof(char));
    }
    int args=split(startline,arr2);
    sscanf(arr2[0],"%d",&num_students);
    sscanf(arr2[1],"%d",&num_labs);
    sscanf(arr2[2],"%d",&num_courses);
    cs=(courses *)malloc(num_courses*sizeof(courses));
    st=(students *)malloc(num_students*sizeof(students));
    lb=(labs *)malloc(num_labs*sizeof(labs));
    curr_course=(int *)malloc(num_courses*sizeof(int));
    
    for(int i=0;i<num_courses;i++){
        scanf("%s %f %d %d",cs[i].course_name,&cs[i].interest,&cs[i].max_slots,&cs[i].num_labs);
        for(int j=0;j<cs[i].num_labs;j++){
            scanf("%d",&cs[i].lab_ids[j]);
            cs[i].curr_stud=0;
        }   
    }
    tut=(sem_t *)malloc(num_courses*sizeof(sem_t));
    stut=(sem_t *)malloc(num_courses*sizeof(sem_t));
    cour_locks=(pthread_mutex_t *)malloc(num_courses*sizeof(pthread_mutex_t));
    crm=(int *)malloc(num_courses*sizeof(int));
    cou=(int *)malloc(num_courses*sizeof(int));
    cour_threads=(pthread_t *)malloc(num_courses*sizeof(pthread_t));


    for(int i=0;i<num_courses;i++){
        sem_init(&tut[i],0,1);
        sem_init(&stut[i],0,1);
        sem_wait(&tut[i]);
        sem_wait(&stut[i]);
       pthread_create(&cour_threads[i],NULL,conduct_tut,(i));
       cs[i].curr_slots=0;
       cs[i].found=0;
       cou[i]=0;
    }


    for(int i=0;i<num_students;i++){
        scanf("%f %d %d %d %f",&st[i].calibre,&st[i].pref_1,&st[i].pref_2,&st[i].pref_3,&st[i].time);
        st[i].filled=0;
        st[i].done=0;
    
    }

    probs=(int *)malloc(num_courses*sizeof(int));
    lblock=(pthread_mutex_t *)malloc(num_labs*sizeof(pthread_mutex_t));
    for(int i=0;i<num_labs;i++){
        scanf("%s %d %d",lb[i].lab_name,&lb[i].num_ta,&lb[i].max_num);
        lb[i].curr_ta=lb[i].num_ta;
        lb[i].nta=0;
        lb[i].tas=(int *)malloc(lb[i].num_ta*sizeof(int));
        lb[i].tas_avail=(int *)malloc(lb[i].num_ta*sizeof(int));
        for(int j=0;j<lb[i].num_ta;j++){
            lb[i].tas[j]=0;
            lb[i].tas_avail[j]=0;
        }
    }
    fill_slots();

}