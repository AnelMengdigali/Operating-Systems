//Anel Mengdigali
//Part A:

/* Generally, this Bouned-Buffer Problem was solved as producer-consumer problem
The aim is to have alternating operations between 2 threads:
 1) mutual exclusion done through mutex: to have 1 thread at a time in critical region.
 2) synchronization done through 2 semaphores: to have 1 A in operation by each consumer and producer thread.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <semaphore.h> //for semaphore + pthread usage
#define n 10 //buffer size --> must be scalable
#define N 20 //number of operations

sem_t empty; //semaphore
sem_t full; //semaphore

pthread_mutex_t mtx; //mutex

int idx = 0; //to control buffer elements
char arr[n]; //to have A chars

void *producer() {
    
    printf("pro thread\n");

    for ( int i = 0; i < N; i ++ ) {

        //decreasing value from 1 to 0, so to prevent the lower thread execution for the 2nd time: 
        sem_wait(&empty); 
            
        //mutex lock the entrance for critical region:
        pthread_mutex_lock(&mtx);
        
        //the critical region starts:
        
        //production of A:
        arr[idx] = 'A';
        idx += 1; //increasing buffer
        
        //for printing:
        int size = sizeof(arr) / sizeof(arr[0]);
        for ( int j = 0; j < size; j ++ ) {
            putchar(arr[j]);
        }
        printf(" %i\n", idx);
        
        //:the critical region ends
        
        //mutex unlock the entrance for critical region so the upper thread can come:
        pthread_mutex_unlock(&mtx);
            
        //increasing value from 0 to 1 as this thread done its job, so sending signal to waiting queue containig the upper thread to wakeup, and come to execute:	
        sem_post(&full);
        
        //for speed variations:
        usleep(1);
        
    }
         
}

void *consumer() {
    
    printf("con thread\n");

    for ( int i = 0; i < N; i ++ ) {

        //decreasing value from 1 to 0, so to prevent the lower thread execution for the 2nd time:
        sem_wait(&full);
            
        //mutex lock the entrance for critical region:
        pthread_mutex_lock(&mtx);
        
        //the critical region starts:
        
        //consumption of A:
        arr[idx-1] = '\0';
        idx -= 1; //decreasing buffer
        
        //for printing:
        int size = sizeof(arr) / sizeof(arr[0]);
        printf("[");
        for ( int j = 0; j < size; j ++ ) {
            putchar(arr[j]);
        }
        printf("] ");
        printf("[%i]\n", idx);
        
        //:the critical region ends
        
        //mutex unlock the entrance for critical region so the upper thread can come:
        pthread_mutex_unlock(&mtx);
            
        //increasing value from 0 to 1 as this thread done its job, so sending signal to waiting queue containig the upper thread to wakeup, and come to execute:
        sem_post(&empty);
        
        //for speed variations:
        usleep(1);
        
    }
         
}

int main() {

    //2 threads initialisation:
    pthread_t con;
    pthread_t pro;
    
    //semaphore usage to obtain synchronization (only 1 item added and consumed) of the 2 threads:
    sem_init( &empty, 0, n ); //semaphore (controlling empty buffer, space is 1) initialisation for threads
    sem_init( &full, 0, 0 );  //semaphore (controlling full buffer) initialisation for threads
    
    //mutex usage (binary semaphore) to obtain mutual exclusion of the 2 threads, as mutex controls entrance in critical region by locking and unlocking operations:
    pthread_mutex_init( &mtx, NULL ); //mutex initialisation
    
    //creation of threads to do segment of code of the flush function:
    int pid1 = pthread_create( &con, NULL, (void*)consumer, NULL );
    sleep(1);
    int pid2 = pthread_create( &pro, NULL, (void*)producer, NULL );
    
    //the main process waits for those 2 treads to terminate before it finishes by using basic function of Pthread, pthread_join():
    pthread_join( con, NULL );
    pthread_join( pro, NULL );
    
    printf("\n");
    
    //mutex deletion after threads execution is done:
    pthread_mutex_destroy(&mtx); 
    
    //semaphores deletion after threads execution is done:
    sem_destroy(&empty);
    sem_destroy(&full);
    
    exit(0);
    
}
