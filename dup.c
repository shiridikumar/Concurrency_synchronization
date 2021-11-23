#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
pthread_mutex_t simp;
void create2(){

    pthread_mutex_lock(&simp);
    printf("this is second function\n");
    pthread_mutex_unlock(&simp);
}
void create1(){
    pthread_mutex_lock(&simp);
    printf("this is fisrt function\n");
    pthread_mutex_unlock(&simp);
}
int main(){
    pthread_t a[2];
    printf("asdasd");
    pthread_create(&a[0],NULL,create1,NULL);
    pthread_create(&a[1],NULL,create2,NULL);
    pthread_join(a[0],NULL);
    pthread_join(a[1],NULL);
}
