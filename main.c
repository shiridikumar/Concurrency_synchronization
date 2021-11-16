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
pthread_t *cou;
pthread_mutex_t cslock;
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
    //find_TA of()
    int pref_1=st[i].pref_1;
    pthread_mutex_lock(&cslock);
    if(curr_course[pref_1]!=1){
        pthread_mutex_unlock(&cslock);
        sem_post(&tut[pref_1]);
    }
    else{
        pthread_mutex_unlock(&cslock);
    }
    //printf("%d--%d--%d\n",curr_course[pref_1],i,pref_1);



    
}

int time_now(){
    return (time(&seconds)-start_time);
}

int conduct_tut(int ind){
    sem_wait(&tut[ind]);
    //printf("%d\n",ind);
    for(int i=0;i<cs[ind].num_labs;i++){
        if(lb[cs[ind].lab_ids[i]].curr_ta>0){
            printf("TA %d from lab %s has been allocated to course %s for his %d'th TA ship\n",lb[cs[ind].lab_ids[i]].num_ta-lb[cs[ind].lab_ids[i]].curr_ta+1,lb[i].lab_name,cs[ind].course_name,lb[cs[ind].lab_ids[i]].nta+1);
            lb[cs[ind].lab_ids[i]].nta++;
            if(lb[cs[ind].lab_ids[i]].nta==lb[cs[ind].lab_ids[i]].max_num){
                lb[cs[ind].lab_ids[i]].curr_ta--;
            }
            break;
        }
    }
    pthread_mutex_lock(&cslock);
    curr_course[ind]=1;
     pthread_mutex_unlock(&cslock);
    sem_post(&tut[ind]);
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
       pthread_join(cou[i],NULL);
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
    


    // courses input

    for(int i=0;i<num_courses;i++){
        scanf("%s %f %d %d",cs[i].course_name,&cs[i].interest,&cs[i].max_slots,&cs[i].num_labs);
        for(int j=0;j<cs[i].num_labs;j++){
            scanf("%d",&cs[i].lab_ids[j]);
        }
        
    }
    tut=(sem_t *)malloc(num_courses*sizeof(sem_t));
    cou=(pthread_t *)malloc(num_courses*sizeof(pthread_t));
    for(int i=0;i<num_courses;i++){
       pthread_create(&cou[i],NULL,conduct_tut,(i));
    }


    //students input
    for(int i=0;i<num_students;i++){
        scanf("%f %d %d %d %f",&st[i].calibre,&st[i].pref_1,&st[i].pref_2,&st[i].pref_3,&st[i].time);
        st[i].filled=0;
    }

    //labs input
    //tut=(sem_t *)malloc(num_courses*sizeof(sem_t));
    for(int i=0;i<num_labs;i++){
        scanf("%s %d %d",lb[i].lab_name,&lb[i].num_ta,&lb[i].max_num);
        lb[i].curr_ta=lb[i].num_ta;
        lb[i].nta=0;
    }
    fill_slots();

}