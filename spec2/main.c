#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <math.h>

sem_t zone_a;
sem_t zone_h;
sem_t zone_n;

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
int enter=0;

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
    double seat_time;
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
double time_now1()
{
    return (clock()-start_time)/(double)CLOCKS_PER_SEC;
}
int time_now()
{
    double num=(clock()-start_time)/(double)CLOCKS_PER_SEC;
    return (int)(num+0.5);
}



void spec_arrived(int i)
{
    int aval, hval, nval;
    printf("time =%d, %s has reached the staidum\n", (int)(time_now1()+0.5), sp[i].name);
    sp[i].wait_start = sp[i].time_entered;
    if (strcmp(sp[i].zone, "H") == 0)
    {
        entry_H(i);
    }
    if (strcmp(sp[i].zone, "A") == 0)
    {
        entry_A(i);
    }
    if (strcmp(sp[i].zone, "N") == 0)
    {
        entry_N(i);
    }
    spec_exit += 1;

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
    sem_init(&zone_h, 0, ch);
    sem_init(&zone_a, 0, ca);
    sem_init(&zone_n, 0, cn);
    pthread_mutex_init(&away, NULL);
    pthread_mutex_init(&neutral, NULL);

    pthread_mutex_init(&zones, NULL);
    start_time = clock();
    while (spec_exit != total)
    {
        double nt=time_now1();
        for (int i = 0; i < total; i++)
        {
            //printf("%d %s %d\n",(int)time_now(),sp[i].name,sp[i].time_entered);
            if (fabs(nt- (double)sp[i].time_entered)<=0.1 && sp[i].entered != 1)
            {
                //printf("%f %d\n",nt,sp[i].time_entered);
                enter+=1;
                sp[i].entered = 1;
                pthread_create(&spec[i], NULL, spec_arrived, (i));
            }
        }
        //printf("sasdasdasdasda\n");
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
        double nt = time_now1();
        for (int i = 0; i < goal_scoring; i++)
        {
            if (fabs(chances[i].time_elapsed-nt)<=0.1 && chances[i].done != 1)
            {
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
                    printf("time =%d, Team %s has scored their %d'th goal\n", (int)(nt+0.5), chances[i].team, c);
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
                    printf("time =%d, Team %s has missed the chance to score the %d goal\n", (int)(nt+0.5), chances[i].team, c + 1);
                }
            }
        }
        //printf("asdasda0000000\n");
    }
}

int entry_H(int i)
{
    int s;
    double nt;
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        return -1;
    }

    ts.tv_sec += (sp[i].patience);
    s = sem_timedwait(&zone_h, &ts);
    if (s == -1)
    {
        if (errno == ETIMEDOUT){
            sleep(1);
            printf("%d sem_timedwait() timed out %s\n",(int)(time_now1()),sp[i].name);
            return;
        } 
        else{
            printf("sem time wait succesful %d %s---\n",(int)(time_now1()+0.5),sp[i].name);
        }
    }
    else{
        printf("timed wait not over %d %s---\n",(int)(time_now1()+0.5),sp[i].name);
    }
    //sem_wait(&zone_h);
    nt=time_now1();
    pthread_mutex_lock(&h);
    if (home_num > 0)
    {
        printf("time=%d ,%s got a seat in zone H\n", (int)(time_now1()+0.5), sp[i].name);
        home_num -= 1;
        strcpy(sp[i].got_zone, "N");
        pthread_mutex_unlock(&h);
    }
    else
    {
        pthread_mutex_unlock(&h);
        pthread_mutex_lock(&n);
        if (neutral_num > 0)
        {
            printf("time=%d ,%s got a seat in zone N\n", (int)(time_now1()+0.5), sp[i].name);
            neutral_num -= 1;
            strcpy(sp[i].got_zone, "N");
            pthread_mutex_unlock(&n);
        }
        else
        {
            pthread_mutex_unlock(&n);
        }
    }
    sp[i].seat_time =time_now1();
    while (time_now1() - sp[i].seat_time < spec_time)
    {
    }
    printf("time=%d, %s watched the match for %d seconds and is leaving\n", (int)(time_now1()+0.5), sp[i].name, (int)(time_now1()+0.5) - (int)(sp[i].seat_time+0.5));
    if (strcmp(sp[i].got_zone, "H") == 0)
    {
        pthread_mutex_lock(&h);
        home_num += 1;
        pthread_mutex_unlock(&h);
    }
    if (strcmp(sp[i].got_zone, "N") == 0)
    {
        pthread_mutex_lock(&n);
        neutral_num += 1;
        pthread_mutex_unlock(&n);
    }
    sem_post(&zone_h);
}
int entry_A(int i)
{
    int s;
    double nt=time_now1();
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        return -1;
    }

    ts.tv_sec += (sp[i].patience);
    s = sem_timedwait(&zone_a, &ts);
    if (s == -1)
    {
        if (errno == ETIMEDOUT){
            sleep(1);
            printf("%d sem_timedwait() timed out %s\n",(int)(time_now1()),sp[i].name);
            return;
        }
        else{
            printf("sem time wait succesful %d %s---\n",(int)(time_now1()+0.5),sp[i].name);
        }
    }
    else{
        printf("timed wait not over %d %s---\n",(int)(time_now1()+0.5),sp[i].name);
    }
    //sem_wait(&zone_a);
    pthread_mutex_lock(&a);
    printf("time=%d ,%s got a seat in zone A\n", (int)(time_now1()+0.5), sp[i].name);
    away_num -= 1;
    strcpy(sp[i].got_zone, "A");
    sp[i].seat_time = time_now1();
    pthread_mutex_unlock(&a);
    while (time_now1() - sp[i].seat_time < spec_time)
    {
    }
    //printf("time=%d , %s watched the match for %d seconds and is leaving\n", (int)time_now(), sp[i].name, (int)time_now() - sp[i].seat_time);
    printf("time=%d , %s watched the match for %d seconds and is leaving\n", (int)(time_now1()+0.5), sp[i].name, (int)(time_now1()+0.5) - (int)(sp[i].seat_time+0.5));
    if (strcmp(sp[i].got_zone, "A") == 0)
    {
        pthread_mutex_lock(&a);
        away_num += 1;
        pthread_mutex_unlock(&a);
    }

    sem_post(&zone_a);
}

int entry_N(int i)
{
    double nt=time_now1();
    int s;
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        return -1;
    }

    ts.tv_sec += (sp[i].patience);
    s = sem_timedwait(&zone_n, &ts);
    if (s == -1)
    {
        if (errno == ETIMEDOUT){
            sleep(1);
            printf("%d sem_timedwait() timed out %s\n",(int)(time_now1()),sp[i].name);
            return;
        }
        else{
            printf("sem time wait succesful %d %s---\n",(int)(time_now1()+0.5),sp[i].name);
        }
    }
    else{
        printf("timedwait not over %d %s---\n",(int)(time_now1()+0.5),sp[i].name);
    }
    //sem_wait(&zone_n);
    //printf("%d\n",home_num+away_num+neutral_num);
    pthread_mutex_lock(&n);
    if (neutral_num > 0)
    {
        printf("time=%d ,%s got a seat in zone N\n", (int)(time_now1()+0.5), sp[i].name);
        neutral_num -= 1;
        strcpy(sp[i].got_zone, "N");
        pthread_mutex_unlock(&n);
    }
    else
    {
        pthread_mutex_unlock(&n);
        pthread_mutex_lock(&a);
        if (away_num > 0)
        {
            printf("time=%d ,%s got a seat in zone A\n", (int)(time_now1()+0.5), sp[i].name);
            away_num -= 1;
            strcpy(sp[i].got_zone, "A");
            pthread_mutex_unlock(&a);
        }
        else
        {
            pthread_mutex_unlock(&a);
            pthread_mutex_lock(&h);
            if (home_num > 0)
            {
                printf("time=%d ,%s got a seat in zone H\n", (int)(time_now1()+0.5), sp[i].name);
                home_num -= 1;
                strcpy(sp[i].got_zone, "H");
                pthread_mutex_unlock(&h);
            }
            else
            {
                printf("************* %d %d %d\n",home_num,away_num,neutral_num);
                pthread_mutex_unlock(&h);
            }
        }
    }
    //printf("time=%d ,%s got a seat in zone N\n",(int)time_now(),sp[i].name);
    sp[i].seat_time =time_now1();
    while (time_now1() - sp[i].seat_time < spec_time)
    {
    }

   // printf("time=%d , %s watched the match for %d seconds and is leaving\n", (int)time_now(), sp[i].name, (int)time_now() - sp[i].seat_time);
   printf("time=%d , %s watched the match for %d seconds and is leaving\n", (int)(time_now1()+0.5), sp[i].name, (int)(time_now1()+0.5) - (int)(sp[i].seat_time+0.5));
    if (strcmp(sp[i].got_zone, "H") == 0)
    {
        pthread_mutex_lock(&h);
        home_num += 1;
        pthread_mutex_unlock(&h);
    }
    if (strcmp(sp[i].got_zone, "A") == 0)
    {
        pthread_mutex_lock(&a);
        away_num += 1;
        pthread_mutex_unlock(&a);
    }
    if (strcmp(sp[i].got_zone, "N") == 0)
    {
        pthread_mutex_lock(&n);
        neutral_num += 1;
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
    home_num = ch;
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