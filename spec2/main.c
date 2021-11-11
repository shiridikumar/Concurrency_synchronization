#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
pthread_cond_t a;

int ch, cn, ca;
int curr_ch;
int curr_cn;
int curr_ca;
int spec_time;
int num_groups;
int *grp_num;
int spec_exit = 0;
int goals_done = 0;
int done_h = 0;
int done_n = 0;
int done_a = 0;
pthread_mutex_t zone_mutex_h;
pthread_mutex_t zone_mutex_a;
pthread_mutex_t zone_mutex_n;
pthread_cond_t h;
pthread_cond_t n;
pthread_cond_t a;

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
    done_h=0;
    int flag = 0;
    if (ch > 0)
    {
        ch -= 1;
        done_h = 1;
        pthread_cond_signal(&h);
        pthread_mutex_unlock(&zone_mutex_h);
        return 1;
    }
    else
    {
        done_h=1;
        pthread_cond_signal(&h);
        pthread_mutex_unlock(&zone_mutex_h);
        return 0;
    }
}

int buy_N_ticket()
{
    pthread_mutex_lock(&zone_mutex_n);
    int flag = 0;
    if (cn > 0)
    {
        cn -= 1;
        done_n = 1;
        // pthread_cond_signal(&n);
        pthread_mutex_unlock(&zone_mutex_n);
        return 1;
    }
    else
    {
        pthread_mutex_unlock(&zone_mutex_n);
        return 0;
    }
}

int buy_A_ticket()
{
    pthread_mutex_lock(&zone_mutex_a);
    int flag = 0;
    if (ca > 0)
    {
        ca -= 1;
        done_a = 1;
        // pthread_cond_signal(&a);
        pthread_mutex_unlock(&zone_mutex_a);
        done_a = 0;
        return 1;
    }
    else
    {
        pthread_mutex_unlock(&zone_mutex_a);
        return 0;
    }
}

void spec_arrived(int i)
{
    printf("time =%d, %s has reached the staidum\n", (int)time_now(), sp[i].name);
    int a;
    // printf("%d %d %d\n",done_a,done_h,done_n);
    if (strcmp(sp[i].zone, "H") == 0)
    {
        // printf("%d %d %d\n",done_a,done_h,done_n);
        if (done_h == 0){
            pthread_mutex_lock(&zone_mutex_h);
            printf("blocking thread %d\n", i);
            pthread_cond_wait(&h, &zone_mutex_h);
            printf("unblocked %d\n", i);
            pthread_mutex_unlock(&zone_mutex_h);
        }
        a = buy_H_ticket();
        if (a == 0)
        {

            a = buy_N_ticket();
            if (a == 0)
            {
                printf("time =%d, %s could ot get a seat\n", (int)time_now(), sp[i].name);
                sp[i].wait_start = (int)time_now();
            }
            else
            {
                printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "N");
                sp[i].got_seat = 1;
            }
        }
        else
        {
            printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "H");
            sp[i].got_seat = 1;
        }
    }

    if (strcmp(sp[i].zone, "N") == 0)
    {
        //if(done_n==0){
        //  pthread_cond_wait(&a,&zone_mutex_a);
        //}
        a = buy_N_ticket();
        if (a == 0)
        {
            /* if(done_a==0){
                pthread_cond_wait(&a,&zone_mutex_a);
            }*/
            if (done_h == 0)
            {
                pthread_mutex_lock(&zone_mutex_h);
                printf("blocking thread %d\n", i);
                pthread_cond_wait(&h, &zone_mutex_h);
                printf("unblocked %d\n", i);
                pthread_mutex_unlock(&zone_mutex_h);
            }
            a = buy_H_ticket();
            if (a == 0)
            {
                
                a = buy_A_ticket();
                if (a == 0)
                {
                    printf("time =%d, %s could not get a seat\n", (int)time_now(), sp[i].name);
                    sp[i].wait_start = (int)time_now();
                }
                else
                {
                    printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "A");
                    sp[i].got_seat = 1;
                }
            }
            else
            {
                printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "H");
                sp[i].got_seat = 1;
            }
        }
        else
        {
            printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "N");
            sp[i].got_seat = 1;
        }
    }

    if (strcmp(sp[i].zone, "A") == 0)
    {
        a = buy_A_ticket();
        if (a == 0)
        {
            printf("time =%d, %s could not get a seat\n", (int)time_now(), sp[i].name);
            sp[i].wait_start = (int)time_now();
        }
        else
        {
            printf("time =%d, %s has got a seat in zone %s\n", (int)time_now(), sp[i].name, "A");
            sp[i].got_seat = 1;
        }
    }
}

void start_simulation()
{
    pthread_t spec[total];
    pthread_mutex_init(&zone_mutex_h, NULL);
    pthread_mutex_init(&zone_mutex_a, NULL);
    pthread_mutex_init(&zone_mutex_n, NULL);
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
                spec_exit += 1;
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

int main()
{
    home_goals = 0;
    away_goals = 0;
    scanf("%d %d %d", &ch, &ca, &cn);
    scanf("%d", &spec_time);
    scanf("%d", &num_groups);
    gp = (groups *)malloc(num_groups * sizeof(groups));
    grp_num = (int *)malloc(num_groups * sizeof(int));
    total = 0;
    int ind = 0;
    sp = (spectators *)malloc(num_groups * 1000 * sizeof(spectators));
    for (int i = 0; i < num_groups; i++)
    {
        scanf("%d", &grp_num[i]);
        for (int j = 0; j < grp_num[i]; j++)
        {
            scanf("%s %s %d %d %d", sp[ind].name, sp[ind].zone, &sp[ind].time_entered, &sp[ind].patience, &sp[ind].num_goals);
            sp[ind].grp_num = i;
            sp[ind].entered = 0;
            sp[ind].got_seat = 0;
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