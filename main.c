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
pthread_mutex_t *cond_locks;
pthread_cond_t *cond_var;
pthread_cond_t *tutsearch;
pthread_mutex_t *studtut;
pthread_cond_t *tutfin;
courses *cs;
int *cow;
typedef struct{
    float calibre;
    int pref_1;
    int pref_2;
    int pref_3;
    float time;
    int filled;
    int gen;
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
sem_t *try;
pthread_mutex_t stud_total;
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
    if(st[i].gen!=1){
        printf("student %d has filled in preferences for course registration\n",i);
        st[i].gen=1;
    }
    int pref_1=st[i].pref_1;
    int old=1;
    int curr_course=pref_1;
    while(st[i].done!=1){
        if(cs[pref_1].found==-1){
            printf(RED "Student %d has withdrawn from course %s\n" RESET,i,cs[pref_1].course_name);
            if(old==1){
                pref_1=st[i].pref_2;
            }
            else if(old==2){
                pref_1=st[i].pref_3;
            }
            if(old<3){
                printf(RED "Student %d has changed current preference from %s (priority %d) to %s (priority %d)\n" RESET,i,cs[curr_course].course_name,old,cs[pref_1].course_name,old+1);
                old+=1;
                curr_course=pref_1;
            }
            else{
                printf("Student %d could not get any of his preferred courses\n",i);
                st[i].done=1;
                pthread_mutex_lock(&stud_total);
                student_done+=1;
                pthread_mutex_unlock(&stud_total);
            }
            continue;
        }
        int prob1 =(rand()%cs[pref_1].max_slots)+1;
        pthread_mutex_lock(&cour_locks[pref_1]);
        if(cou[pref_1]!=1){
            cou[pref_1]=1;
            probs[pref_1]=prob1;
            pthread_mutex_unlock(&cour_locks[pref_1]);
            //sem_wait(&stut[pref_1]);
            pthread_mutex_lock(&studtut[i]);
            pthread_cond_wait(&tutsearch[pref_1],&studtut[i]);
            pthread_mutex_unlock(&studtut[i]);

        }
        else{
            pthread_mutex_unlock(&cour_locks[pref_1]);
        }
        pthread_mutex_lock(&cour_locks[pref_1]);
        if(cs[pref_1].curr_slots<probs[pref_1]){
            cs[pref_1].curr_slots++;
            printf(GREEN "student %d has been allocated a seat in course %s\n" RESET,i,cs[pref_1].course_name);
            pthread_mutex_unlock(&cour_locks[pref_1]);
 
            pthread_mutex_lock(&cond_locks[i]);
            pthread_cond_wait(&tutfin[pref_1],&cond_locks[i]);
            pthread_mutex_unlock(&cond_locks[i]);
            int interest_prob=(int)(st[i].calibre*cs[pref_1].interest*100);
            srand(time(NULL));
            int random=rand()%100;
            //printf("random****************** %d\n",random);
            if(random<=interest_prob){
                printf(RED "Student %d has selected the course %s permanently\n" RESET,i,cs[pref_1].course_name);
                st[i].done=1;
                pthread_mutex_lock(&stud_total);
                student_done+=1;
                pthread_mutex_unlock(&stud_total);
            }
            else{
                printf(RED "Student %d has withdrawn from course %s\n" RESET,i,cs[pref_1].course_name);
                if(old==1){
                    pref_1=st[i].pref_2;
                }
                else if(old==2){
                    pref_1=st[i].pref_3;
                }
                if(old<3){
                    printf(RED "Student %d has changed current preference from %s (priority %d) to %s (priority %d)\n" RESET,i,cs[curr_course].course_name,old,cs[pref_1].course_name,old+1);
                    old+=1;
                    curr_course=pref_1;
                }
                else{
                    printf("Student %d could not get any of his preferred courses\n",i);
                    st[i].done=1;
                    pthread_mutex_lock(&stud_total);
                    student_done+=1;
                    pthread_mutex_unlock(&stud_total);
                }
            }
        }
        else{
            //printf("%d---------------\n",i);
            pthread_mutex_unlock(&cour_locks[pref_1]);
            cow[pref_1]=1;
            pthread_mutex_lock(&cond_locks[i]);
            pthread_cond_wait(&cond_var[pref_1],&cond_locks[i]);
            pthread_mutex_unlock(&cond_locks[i]);
            //sem_wait(&try[pref_1]);
            //student_filled(i);
            //sem_post(&try[pref_1]);
        } 
        //printf("%d*****************\n",i);
    }

    


}

int time_now(){
    return (time(&seconds)-start_time);
}

int conduct_tut(int ind){
    while(student_done!=num_students){
        sleep(4);
        //printf(RED "%s ******* %d ********* again ***\n" RESET,cs[ind].course_name,student_done);
        //sem_wait(&try[ind]);
        int i,j;
        for(i=0;i<cs[ind].num_labs;i++){
            pthread_mutex_lock(&lblock[cs[ind].lab_ids[i]]);
            for(j=0;j<lb[cs[ind].lab_ids[i]].num_ta;j++){
                if(lb[cs[ind].lab_ids[i]].tas[j]!=lb[cs[ind].lab_ids[i]].max_num && lb[cs[ind].lab_ids[i]].tas_avail[j]){
                    lb[cs[ind].lab_ids[i]].tas[j]++;
                    lb[cs[ind].lab_ids[i]].tas_avail[j]=0;
                    pthread_mutex_lock(&cour_locks[ind]);
                    cs[ind].found=1;
                    pthread_mutex_unlock(&cour_locks[ind]);
                    printf(GREEN "TA %d from lab %s has been allocated to course %s for his %d'th TA ship\n" RESET,j+1,lb[cs[ind].lab_ids[i]].lab_name,cs[ind].course_name,lb[cs[ind].lab_ids[i]].tas[j]);
                    break;
                }
            }
            pthread_mutex_unlock(&lblock[cs[ind].lab_ids[i]]);
            if(cs[ind].found==1){
                break;
            }
        }
        if(cs[ind].found==1){
            printf(YELLOW "course %s has been allocated %d seats\n" RESET,cs[ind].course_name,probs[ind]);
            //sem_post(&stut[ind]);
            pthread_cond_broadcast(&tutsearch[ind]);
            printf(BLUE "Tutorial has been started for course %s with %d seats filled out of %d\n" RESET,cs[ind].course_name,cs[ind].curr_slots,probs[ind]);
            pthread_mutex_lock(&cour_locks[ind]);
            sleep(2);
            cs[ind].found=0;
            lb[cs[ind].lab_ids[i]].tas_avail[j]=1;
            lb[cs[ind].lab_ids[i]].tas[j];
            cs[ind].curr_slots=0;
            cou[ind]=0;
            pthread_mutex_unlock(&cour_locks[ind]);
            //sem_post(&try[ind]);
            //printf("qweqweqweqwe\n");
            pthread_cond_broadcast(&tutfin[ind]);
            pthread_cond_broadcast(&cond_var[ind]);
            if(student_done==num_students){
                break;
            }
        }
        else{
            pthread_cond_broadcast(&tutsearch[ind]);
            pthread_cond_broadcast(&tutfin[ind]);
            pthread_cond_broadcast(&cond_var[ind]);
            pthread_mutex_lock(&cour_locks[ind]);
            cs[ind].found=-1;
            printf("Course %s does not have any TA mentors eligible and is removed from course offerings\n",cs[ind].course_name);
            pthread_mutex_unlock(&cour_locks[ind]);
            break;
        }
    }

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
    try=(sem_t *)malloc(num_courses*sizeof(sem_t));
    cour_locks=(pthread_mutex_t *)malloc(num_courses*sizeof(pthread_mutex_t));
    crm=(int *)malloc(num_courses*sizeof(int));
    cow=(int *)malloc(num_courses*sizeof(int));
    cou=(int *)malloc(num_courses*sizeof(int));
    cond_locks=(pthread_mutex_t *)malloc(num_students*sizeof(pthread_mutex_t));
    cour_threads=(pthread_t *)malloc(num_courses*sizeof(pthread_t));
    cond_var=(pthread_cond_t *)malloc(num_courses*sizeof(pthread_cond_t));
    studtut=(pthread_mutex_t *)malloc(num_students*sizeof(pthread_mutex_t));
    tutsearch=(pthread_cond_t *)malloc(num_courses*sizeof(pthread_cond_t));
    tutfin=(pthread_cond_t *)malloc(num_courses*sizeof(pthread_cond_t));

    for(int i=0;i<num_courses;i++){
        sem_init(&tut[i],0,1);
        sem_init(&stut[i],0,1);
        sem_wait(&tut[i]);
        sem_wait(&stut[i]);
       pthread_create(&cour_threads[i],NULL,conduct_tut,(i));
       cs[i].curr_slots=0;
       cs[i].found=0;
       cow[i]=0;
       cou[i]=0;
    }


    for(int i=0;i<num_students;i++){
        scanf("%f %d %d %d %f",&st[i].calibre,&st[i].pref_1,&st[i].pref_2,&st[i].pref_3,&st[i].time);
        st[i].filled=0;
        st[i].done=0;
        st[i].gen=0;
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
            lb[i].tas_avail[j]=1;
        }
    }
    fill_slots();

}