#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

int ch, cn, ca;
int curr_ch;
int curr_cn;
int curr_ca;
int spec_time;
int num_groups;
int *grp_num;
int spec_exit = 0;
pthread_mutex_t zone_mutex_h;
pthread_mutex_t zone_mutex_a;
pthread_mutex_t zone_mutex_n;

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
    int flag = 0;
    if (ch > 0)
    {
        ch -= 1;
        pthread_mutex_unlock(&zone_mutex_h);
        return 1;
    }
    else
    {
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
        pthread_mutex_unlock(&zone_mutex_a);
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
    printf("time =%d, %s has reached the staidum\n",(int)time_now(), sp[i].name);
    int a;
    if (strcmp(sp[i].zone,"H")==0)
    {
        a = buy_H_ticket();
        if (a == 0)
        {
            a = buy_N_ticket();
        }
        if (a == 0)
        {
            printf("time =%d,%s is waiting at the gate for the ticket\n", (int)time_now(),sp[i].name);
        }
        else
        {
            printf("time =%d,%s entered the stadium\n", (int)time_now(),sp[i].name);
        }
    }

    if (strcmp(sp[i].zone,"N")==0)
    {
        a = buy_N_ticket();
        if (a == 0)
        {
            a = buy_H_ticket();
        }
        if (a == 0)
        {
            a = buy_A_ticket();
        }
        if (a == 0){
            printf("time =%d,%s is waiting at the gate for the ticket\n",(int)time_now(), sp[i].name);
        }
        else
        {
            printf("time =%d,%s entered the stadium\n",(int)time_now(), sp[i].name);
        }
    }

    if(strcmp(sp[i].zone,"A")==0){
        a=buy_A_ticket();
        if (a == 0){
            printf("time =%d,%s is waiting at the gate for the ticket\n",(int)time_now(), sp[i].name);
        }
        else
        {
            printf("time =%d,%s entered the stadium\n", (int)time_now(),sp[i].name);
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
                spec_exit+=1;
            }
        }
    }
    for (int i = 0; i < total; i++)
    {
            pthread_join(spec[i], NULL);
    }
    printf("%d %d %d",ch,ca,cn);
}


int main()
{
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
            ind++;
        }
        total += grp_num[i];
    }

    scanf("%d", &goal_scoring);
    chances = (goal_chances *)malloc(goal_scoring * sizeof(goal_chances));
    for (int i = 0; i < goal_scoring; i++)
    {
        scanf("%s %d %f", chances[i].team, &chances[i].time_elapsed, &chances[i].prob);
    }
    //printf("helloooooooo\n");
    spectators_sort();
    start_simulation();
}