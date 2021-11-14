#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

sem_t zone_a;
sem_t zone_h;
sem_t zone_n;
pthread_cond_t a;

int ch, cn, ca;
int curr_ch;
int curr_cn;
int curr_ca;
int spec_time;
int num_groups;
int *grp_num;
int away_num;
int home_num;
int neutral_num;
int spec_exit = 0;
int goals_done = 0;
int avail = 0;
int done_h = 1;
int done_n = 1;
int done_a = 1;
int exit_a = 1;
int exit_h = 1;
int exit_n = 1;
int *spec_reached;
pthread_mutex_t zones;
pthread_mutex_t home;
pthread_mutex_t neutral;
pthread_mutex_t away;
pthread_mutex_t zone_mutex_h;
pthread_mutex_t zone_mutex_a;
pthread_mutex_t zone_mutex_n;
pthread_cond_t *spc;
pthread_mutex_t h;
pthread_mutex_t n;
pthread_mutex_t a;

int home_goals;
int away_goals;
typedef struct
{
    char name[200];
    char zone[10];
    int time_entered;
    int patience;
    int num_goals;
} groups;
groups *gp;

typedef struct
{
    char team[10];
    int time_elapsed;
    float prob;
    int done;
} goal_chances;
goal_chances *chances;

typedef struct
{
    char name[200];
    int time_entered;
    int grp_num;
    int patience;
    int num_goals;
    char zone[10];
    int entered;
    int wait_start;
    int got_seat;
    int seat_time;
    char got_zone[10];
} spectators;

spectators *sp;
int total;
clock_t start_time;

int goal_scoring;
void spectators_sort()
{
    for (int i = 0; i < total; i++)
    {
        for (int j = 0; j < total - 1; j++)
        {
            if (sp[j].time_entered > sp[j + 1].time_entered)
            {
                spectators temp = sp[j];
                sp[j] = sp[j + 1];
                sp[j + 1] = temp;
            }
        }
    }
}

void update_zone()
{
}
int time_now()
{
    return (int)(clock() - start_time) / CLOCKS_PER_SEC;
}

int buy_H_ticket()
{
    pthread_mutex_lock(&zone_mutex_h);
    pthread_mutex_lock(&home);
    done_h = 0;
    int flag = 0;
    if (ch > 0)
    {
        //for(int i=0;i<100000000;i++){
        //}
        ch -= 1;
        done_h = 1;
        home_num -= 1;
        pthread_mutex_unlock(&home);
        pthread_mutex_unlock(&zone_mutex_h);
        return 1;
    }
    else
    {
        done_h = 1;
        pthread_mutex_unlock(&home);
        //pthread_mutex_unlock(&neutral);
        pthread_mutex_unlock(&zone_mutex_h);
        return 0;
    }
}

int buy_N_ticket()
{
    pthread_mutex_lock(&zone_mutex_n);
    pthread_mutex_lock(&neutral);
    done_n = 0;
    int flag = 0;
    if (cn > 0)
    {
        cn -= 1;
        done_n = 1;
        neutral_num -= 1;
   //     pthread_mutex_unlock(&home);
        pthread_mutex_unlock(&neutral);
   
        pthread_mutex_unlock(&zone_mutex_n);
        return 1;
    }
    else
    {
        done_n = 1;
        //pthread_mutex_unlock(&home);
        pthread_mutex_unlock(&neutral);
        //pthread_mutex_unlock(&away);
        pthread_mutex_unlock(&zone_mutex_n);
        return 0;
    }
}

int buy_A_ticket()
{
    pthread_mutex_lock(&zone_mutex_a);
    pthread_mutex_lock(&away);
    for (int i = 0; i < 10000000; i++)
    {
    }
    done_a = 0;
    int flag = 0;
    if (ca > 0)
    {
        ca -= 1;
        done_a = 1;
        away_num -= 1;
        //printf("exitting with allocating*******\n");
        pthread_mutex_unlock(&away);
        pthread_mutex_unlock(&zone_mutex_a);
        return 1;
    }
    else
    {
        //printf("exittinf without alloting*************\n");
        done_a = 1;
        pthread_mutex_unlock(&away);
        pthread_mutex_unlock(&zone_mutex_a);
        return 0;
    }
}

void exit_stadium_h()
{
    pthread_mutex_lock(&home);
    pthread_mutex_lock(&neutral);
    pthread_mutex_lock(&zone_mutex_h);
    exit_h = 0;
    done_h = 0;
    ch += 1;
    done_h = 1;
    exit_h = 1;
    home_num += 1;
    neutral_num += 1;
    pthread_cond_signal(&n);
    pthread_cond_signal(&h);
    pthread_mutex_unlock(&zone_mutex_h);
    pthread_mutex_unlock(&home);
    pthread_mutex_unlock(&neutral);
}

void exit_stadium_a()
{
    pthread_mutex_lock(&away);
    pthread_mutex_lock(&zone_mutex_a);
    exit_a = 0;
    done_a = 0;
    ca += 1;
    away_num += 1;
    exit_a = 1;
    done_a = 1;
    pthread_mutex_unlock(&zone_mutex_a);
    pthread_cond_signal(&a);
    pthread_cond_signal(&n);
    pthread_mutex_unlock(&away);
}

void exit_stadium_n()
{
    pthread_mutex_lock(&home);
    pthread_mutex_lock(&away);
    pthread_mutex_lock(&neutral);
    pthread_mutex_lock(&zone_mutex_n);
    exit_n = 0;
    done_n = 0;
    cn += 1;
    home_num += 1;
    away_num += 1;
    neutral_num += 1;
    exit_n = 1;
    done_n = 1;
    pthread_cond_signal(&n);
    pthread_cond_signal(&h);
    pthread_mutex_unlock(&zone_mutex_n);
    pthread_mutex_unlock(&away);
    pthread_mutex_unlock(&home);
    pthread_mutex_unlock(&neutral);
}

void spec_arrived(int i)
{
    int aval,hval,nval;
    printf("time =%d, %s has reached the staidum\n", (int)time_now(), sp[i].name);
    sp[i].wait_start = (int)time_now();
    if(strcmp(sp[i].zone,"H")==0){
        entry_H(i);
    }
    if(strcmp(sp[i].zone,"A")==0){
        entry_A(i);
    }
    if(strcmp(sp[i].zone,"N")==0){

        entry_N(i);
    }
    spec_exit+=1;

    /*int a;
    if (strcmp(sp[i].zone, "H") == 0)
    {
        pthread_mutex_lock(&home);
        if (home_num <= 0)
        {
            printf("Blocked thread \n");
            printf("%d\n", home_num);
            pthread_cond_wait(&h, &home);
            printf("unblocked thread \n");
        }

        pthread_mutex_unlock(&home);
        a = buy_H_ticket();
        if (a == 0)
        {
            a = buy_N_ticket();
            if (a == 0)
            {
                printf("");
            }
            else
            {
                printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "N");
                sp[i].got_seat = 1;
                strcpy(sp[i].got_zone, "N");
            }
        }
        else
        {
            printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "H");
            sp[i].got_seat = 1;
            strcpy(sp[i].got_zone, "H");
        }
    }

    if (strcmp(sp[i].zone, "N") == 0)
    {
        pthread_mutex_lock(&neutral);
        if (neutral_num <= 0)
        {
            printf("Blocked thread \n");
            printf("%d\n", neutral_num);
            pthread_cond_wait(&n, &neutral);
            printf("Unblocked thread\n");
        }
        pthread_mutex_unlock(&neutral);

        a = buy_N_ticket();
        if (a == 0)
        {
            a = buy_H_ticket();
            if (a == 0)
            {
                a = buy_A_ticket;
                if (a == 0)
                {
                    printf("");
                }
                else
                {
                    printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "A");
                    sp[i].got_seat = 1;
                    strcpy(sp[i].got_zone, "A");
                }
            }
            else
            {
                printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "H");
                sp[i].got_seat = 1;
                strcpy(sp[i].got_zone, "H");
            }
        }
        else
        {
            printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "N");
            sp[i].got_seat = 1;
            strcpy(sp[i].got_zone, "N");
        }
    }
    if (strcmp(sp[i].zone,"A") == 0)
    {
        pthread_mutex_lock(&away);
        if (away_num <= 0)
        {
            printf("Blocked thread\n");
            printf("%d\n", away_num);
            pthread_cond_wait(&a, &away);
            printf("Unblocked thread\n");
        }
        pthread_mutex_unlock(&away);
        a = buy_A_ticket();
        if (a == 0)
        {
            printf("");
        }
        else
        {
            printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "A");
            sp[i].got_seat = 1;
            strcpy(sp[i].got_zone, "A");
        }
    }*/
}

void start_simulation()
{
    pthread_t spec[total];
    pthread_mutex_init(&zone_mutex_h, NULL);
    pthread_mutex_init(&zone_mutex_a, NULL);
    pthread_mutex_init(&zone_mutex_n, NULL);
    pthread_mutex_init(&h, NULL);
    pthread_mutex_init(&a, NULL);
    pthread_mutex_init(&n, NULL);
    pthread_mutex_init(&home, NULL);
    sem_init(&zone_h,0,ch+cn);
    sem_init(&zone_a,0,ca);
    sem_init(&zone_n,0,cn+ch+ca);
    pthread_mutex_init(&away, NULL);
    pthread_mutex_init(&neutral, NULL);

    pthread_mutex_init(&zones, NULL);
    start_time = clock();
    while (spec_exit != total)
    {
        for (int i = 0; i < total; i++)
        {
            //printf("%d %s %d\n",(int)time_now(),sp[i].name,sp[i].time_entered);
            if ((int)time_now() == sp[i].time_entered && sp[i].entered != 1)
            {
                sp[i].entered = 1;
                pthread_create(&spec[i], NULL, spec_arrived, (i));
            }
        }
    }
    for (int i = 0; i < total; i++)
    {
        pthread_join(spec[i], NULL);
    }
}

void goals_simulation()
{
    int c;
    while (goals_done != goal_scoring)
    {
        for (int i = 0; i < goal_scoring; i++)
        {
            if (chances[i].time_elapsed == (int)time_now() && chances[i].done != 1)
            {
                int nt = (int)time_now();
                float prob = chances[i].prob;
                goals_done += 1;
                chances[i].done = 1;
                srand(time(NULL));
                int rand_int = rand() % (100);
                if (rand_int <= (int)100 * prob)
                {
                    if (strcmp("H", chances[i].team) == 0)
                    {
                        home_goals += 1;
                        c = home_goals;
                    }
                    else
                    {
                        away_goals += 1;
                        c = away_goals;
                    }
                    printf("time =%d, Team %s has scored their %d'th goal\n", nt, chances[i].team, c);
                }
                else
                {
                    if (strcmp("H", chances[i].team) == 0)
                    {
                        c = home_goals;
                    }
                    else
                    {
                        c = away_goals;
                    }
                    printf("time =%d, Team %s has missed the chance to score the %d goal\n", nt, chances[i].team, c + 1);
                }
            }
        }
    }
}

int entry_H(int i){
    sem_wait(&zone_h);
    pthread_mutex_lock(&h);
    pthread_mutex_lock(&n);
    if(home_num==0){
        printf("time=%d ,%s got a seat in zone N\n",(int)time_now(),sp[i].name);
        neutral_num-=1;
        strcpy(sp[i].got_zone,"N");
    }
    else{
        printf("time=%d ,%s got a seat in zone H\n",(int)time_now(),sp[i].name);
        home_num-=1;
        strcpy(sp[i].got_zone,"H");
    }
    sp[i].seat_time=(int)time_now();
    pthread_mutex_unlock(&h);
    pthread_mutex_unlock(&n);
    while ((int)time_now()-sp[i].seat_time<spec_time)
    {
    }
    printf("time=%d , %s watched the match for %d seconds and is leaving\n",(int)time_now(),sp[i].name,(int)time_now()-sp[i].seat_time);
    if(strcmp(sp[i].got_zone,"H")==0){
        pthread_mutex_lock(&h);
        home_num+=1;
        pthread_mutex_unlock(&h);
    }
    if(strcmp(sp[i].got_zone,"N")==0){
        pthread_mutex_lock(&n);
        neutral_num+=1;
        pthread_mutex_unlock(&n);
    }
    sem_post(&zone_h);

    
}
int entry_A(int i){
    pthread_mutex_lock(&a);
    sem_wait(&zone_a);
    printf("time=%d ,%s got a seat in zone A\n",(int)time_now(),sp[i].name);
    away_num-=1;
    strcpy(sp[i].got_zone,"A");
    sp[i].seat_time=(int)time_now();
    pthread_mutex_unlock(&a);
    while ((int)time_now()-sp[i].seat_time<spec_time)
    {
        
    }
    printf("time=%d , %s watched the match for %d seconds and is leaving\n",(int)time_now(),sp[i].name,(int)time_now()-sp[i].seat_time);
    if(strcmp(sp[i].got_zone,"A")==0){
        pthread_mutex_lock(&a);
        away_num+=1;
        pthread_mutex_unlock(&a);
    }

    sem_post(&zone_a);
}

int entry_N(int i){
    sem_wait(&zone_n);
    pthread_mutex_lock(&n);
    pthread_mutex_lock(&h);
    pthread_mutex_lock(&a);
    if(neutral_num<=0){
        if(home_num<=0){
            if(away_num<=0){
                printf("********* %s\n",sp[i].name);
            }
            else{
                printf("time=%d ,%s got a seat in zone A--- %d\n",(int)time_now(),sp[i].name,away_num);
                away_num-=1;
                strcpy(sp[i].got_zone,"A");
            }
        }
        else{
            printf("time=%d ,%s got a seat in zone H\n",(int)time_now(),sp[i].name);
            home_num-=1;
            strcpy(sp[i].got_zone,"H");
        }
    }
    else{
        printf("time=%d ,%s got a seat in zone N\n",(int)time_now(),sp[i].name);
        neutral_num-=1;
        strcpy(sp[i].got_zone,"N");
    }
    pthread_mutex_unlock(&h);
    pthread_mutex_unlock(&a);
    pthread_mutex_unlock(&n);
    //printf("time=%d ,%s got a seat in zone N\n",(int)time_now(),sp[i].name);
    sp[i].seat_time=(int)time_now();
    while ((int)time_now()-sp[i].seat_time<spec_time)
    {
        
    }
    
    printf("time=%d , %s watched the match for %d seconds and is leaving\n",(int)time_now(),sp[i].name,(int)time_now()-sp[i].seat_time);
    if(strcmp(sp[i].got_zone,"H")==0){
        pthread_mutex_lock(&h);
        home_num+=1;
        pthread_mutex_unlock(&h);
    }
    if(strcmp(sp[i].got_zone,"A")==0){
        pthread_mutex_lock(&a);
        away_num+=1;
        pthread_mutex_unlock(&a);
    }
    if(strcmp(sp[i].got_zone,"N")==0){
        pthread_mutex_lock(&n);
        neutral_num+=1;
        pthread_mutex_unlock(&n);
    }
    sem_post(&zone_n);
}
int main()
{
    home_goals = 0;
    away_goals = 0;
    scanf("%d %d %d", &ch, &ca, &cn);
    scanf("%d", &spec_time);
    scanf("%d", &num_groups);
    away_num = ca;
    home_num = ch ;
    neutral_num = cn;
    gp = (groups *)malloc(num_groups * sizeof(groups));
    grp_num = (int *)malloc(num_groups * sizeof(int));
    total = 0;
    int ind = 0;
    sp = (spectators *)malloc(num_groups * 1000 * sizeof(spectators));
    spc = (pthread_cond_t *)malloc(num_groups * 1000 * sizeof(pthread_cond_t));
    spec_reached = (int *)malloc(num_groups * 1000 * sizeof(int));
    for (int i = 0; i < num_groups; i++)
    {
        scanf("%d", &grp_num[i]);
        for (int j = 0; j < grp_num[i]; j++)
        {
            scanf("%s %s %d %d %d", sp[ind].name, sp[ind].zone, &sp[ind].time_entered, &sp[ind].patience, &sp[ind].num_goals);
            sp[ind].grp_num = i;
            sp[ind].entered = 0;
            sp[ind].got_seat = 0;
            spec_reached[i] = 0;
            ind++;
        }
        total += grp_num[i];
    }

    scanf("%d", &goal_scoring);
    chances = (goal_chances *)malloc(goal_scoring * sizeof(goal_chances));
    for (int i = 0; i < goal_scoring; i++)
    {
        scanf("%s %d %f", chances[i].team, &chances[i].time_elapsed, &chances[i].prob);
        chances[i].done = 0;
    }
    //printf("helloooooooo\n");
    spectators_sort();
    pthread_t spectators_mutex;
    pthread_t goals_mutex;
    pthread_create(&spectators_mutex, NULL, start_simulation, NULL);
    pthread_create(&goals_mutex, NULL, goals_simulation, NULL);
    pthread_join(spectators_mutex, NULL);
    pthread_join(goals_mutex, NULL);
}