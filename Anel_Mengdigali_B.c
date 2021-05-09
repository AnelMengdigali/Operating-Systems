//Anel Mengdigali
//Part B:

/* Generally, this Bounded-Buffer Problem was solved as producer-consumer problem.
The aim is to have alternating letters between 2 strings, to reach that 2 things have to be done:
 1) mutual exclusion done through mutex: to have 1 thread at a time in critical region.
 2) synchronization done through condition variables: to have 1 A in operation by each consumer and producer thread.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define n 10 //buffer size --> must be scalable
#define N 20 //number of operations

pthread_mutex_t mtx; //mutex

pthread_cond_t full; //condition variable
pthread_cond_t empty; //condition variable

int idx = 0; //to control buffer elements

//linked list:
typedef struct node {
    char val;
    struct node *next;
} node_t;

node_t *head = NULL;

void *producer() {
    
    printf("pro thread\n");

    for ( int i = 0; i < N; i ++ ) {
        
        //mutex lock the entrance for critical region:
        pthread_mutex_lock(&mtx);
        
        //wait until buffer is not full = condition, (to produce):
        while( idx == n ) {
            //blocking the pro thread execution until the condition gets satisfied:
            pthread_cond_wait( &empty, &mtx );
        }
        
        //the critical region starts for producing an item:
        
        int counter = 0; //to track number of A
        
        if ( head == NULL ) { //when node is the first one in the list
                       
            //producing an item:
            head = (node_t*)malloc( sizeof(node_t) ); //allocating space
            head -> val = 'A'; //saving A to value field
            head -> next = NULL; //not pointing to new node yet
                             
        } 
        else {
                     
            node_t *curr = head;
            while( curr -> next != NULL ) {
            	curr = curr -> next; //searching for the last node to append
            }
            //producing an item: 
            curr -> next = (node_t*)malloc( sizeof(node_t) ); //allocating space
            curr = curr -> next;
            curr -> val = 'A';  //saving A to value field
            curr -> next = NULL; //not pointing to new node yet
            
        }
        idx += 1; //increasing buffer   
        
        //for printing (going through each node):
        node_t *ins = head;
        while ( ins != NULL ) {
        
            putchar( ins -> val );
            counter += 1;
            ins = ins -> next;
            
        }
        printf(" %i\n", counter);
        
        //:the critical region ends
        
        //sending signal to the waiting con thread to wakeup, and come to execute:
        pthread_cond_signal(&full);
        
        //mutex unlock the entrance for critical region so the upper thread can come:
        pthread_mutex_unlock(&mtx);
        
        //for speed variations:
        usleep(1);
        
    }
    
}

void *consumer() {
    
    printf("con thread\n");

    for ( int i = 0; i < N; i ++ ) {
        
        //mutex lock the entrance for critical region:
        pthread_mutex_lock(&mtx);
        
      	//wait until buffer is not empty = condition, (to consume):
        while( idx == 0 ) {
            //blocking the con thread execution until the condition gets satisfied:
            pthread_cond_wait( &full, &mtx );
        }	
	
        //the critical region starts for consuming an item:
        
        int counter = 0; //to track number of A
        
        //consuming an item: 
        node_t *temp = head -> next; //for LIFO
        free( head ); //deallocating space
        head = temp;       	
        idx -= 1; //decreasing buffer
                    
        //for printing (going through each node):
        node_t *ins = head;
        printf("[");
        while ( ins != NULL ) {
        
            putchar( ins -> val );
            counter += 1;
            ins = ins -> next;
            
        }
        printf("] ");
        printf("[%i]\n", counter);
        
        //:the critical region ends
        
        //sending signal to the waiting con thread to wakeup, and come to execute:
        pthread_cond_signal(&empty);
        
        //mutex unlock the entrance for critical region so the upper thread can come:
        pthread_mutex_unlock(&mtx);
           
        //for speed variations:
        usleep(1);
        
    }
    
}
 
int main() {
    
    //2 threads initialisation:
    pthread_t con;
    pthread_t pro;
    
    //mutex usage (binary semaphore) to obtain mutual exclusion of the 2 threads, as mutex controls entrance in critical region by locking and unlocking operations:
    pthread_mutex_init( &mtx, NULL ); //mutex initialisation
    
    //condition variables usage to obtain synchronization of the 2 threads:
    pthread_cond_init( &full, NULL );
    pthread_cond_init( &empty, NULL );
    
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
    
    //condition variables deletion after threads execution is done:
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    exit(0);
    
}
