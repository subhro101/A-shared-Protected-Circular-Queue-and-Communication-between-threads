/*******************************************************************************************************
Name :-Subhrajyoti Pradhan
Professor :- Dr. Korzhova 
UID :- U79333962
Project Name :- A shared Protected Circular Queue and Communication between threads
File Name :- Proj3.c
*******************************************************************************************************/
/* Disclaimer - In this code, I have occassionally used code snippets from the examples given 
during class by Dr. Korzhova
I hva also taken excerpts from books
I have tried to mention as much of the snippts I have used wherevefr I can 
Other works are purely my onw
*/

//THE FOLLOWING HEADERS WHERE GIVEN IN THE PROJECT FILE

#define _REENTRANT
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
/* We will now do the following things - 
 buff (size 15 and make it circular) for shared memccpy
 
*/
#define SHMKEY ((key_t) 9870)
#define BUFFER_SIZE 15
#define BUFFER_WRAP(x) x%BUFFER_SIZE
/* we shall now define two \
Structures
*/
typedef struct
{
  char *value;
} SHAREDMEMORY;			//SHAREDMEMORY (memory)
typedef struct
{
  int value;
} SHAREDDATA;			//shared_data (data)


sem_t empty;			//this is the empty semaphore which should be initialized to n
sem_t full;			//this is the full semaphore which should be innitiazlied to -
sem_t mutex;			//this is the mutex which should be initialized to 1
int beg = 0;
int fin = 0;
SHAREDMEMORY *buff;
char newChar;
FILE *files;
SHAREDDATA *counter;

/*we shall now define the function prototype
*/
void *ProducerFunction ();
void *ConsumerFunctions ();

/*we shall now start the main progream
*/
int
main ()
{
  files = fopen ("mytest.dat", "r");
  //*****************init everything: threads, sems, shmem********///
  int r = 0;

  // define the shared memory ID
  int shmid;

  // define the process ids for threads 1 and 2
  pthread_t producer[1];
  pthread_t consumer[1];

  // define the  att pointers array 
  pthread_attr_t attr;
  char *shmadd;
  shmadd = (char *) 0;
  /*
     we shall now create a
     shared mem segment, 
     if returns -1 
     if an error occures */
  if ((shmid = shmget (SHMKEY, sizeof (char), IPC_CREAT | 0666)) < 0)
    {
      perror ("shmget");
      return (1);
    }
  /*
     we shall now connect proc
     shared mem segment, 
     if returns -1 
     if an error occures */
  if ((buff =
       (SHAREDMEMORY *) shmat (shmid, shmadd, 0)) == (SHAREDMEMORY *) - 1)
    {
      perror ("shmat");
      return (0);
    }

  char barray[15];
  buff->value = barray;
  counter = (SHAREDDATA *) malloc (sizeof (SHAREDDATA));

  counter->value = 0;

  /* we shall now
     initialisese the sem
   */
  sem_init (&empty, 0, BUFFER_SIZE);
  sem_init (&full, 0, 0);
  sem_init (&mutex, 0, 1);

  fflush (stdout);
  pthread_attr_init (&attr);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);

  /* we shall now
     create the
     threads
   */
  pthread_create (&producer[0], &attr, ProducerFunction, 0);
  pthread_create (&consumer[0], &attr, ConsumerFunctions, 0);

  pthread_join (producer[0], 0);
  pthread_join (consumer[0], 0);
  sem_destroy (&empty);
  sem_destroy (&full);
  sem_destroy (&mutex);

  if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
    {
      perror ("shmctl");
      exit (-1);
    }
  fclose (files);
  /*
     The next parthappens when the thread is over
   */
  printf ("\n\n value of parent counter is  =  %d\n", counter->value);
  printf ("\n");
  printf ("\t\t    THE END\n");
  printf ("\n");
  printf ("\t\t    THANK YOU\n");
  return 0;
}

/* we shall now 
define the
 producer function
 */
void *
ProducerFunction ()
{
  bool over = false;
  while (!over)
    {
      sem_wait (&empty);
      sem_wait (&mutex);
      fin++;
      if (fscanf (files, "%c", &newChar) != EOF)
	{
	  buff->value[BUFFER_WRAP (fin)] = newChar;
	}
      else
	{
	  buff->value[BUFFER_WRAP (fin)] = '*';
	  over = true;
	}
      sem_post (&mutex);
      sem_post (&full);
    }
  return (void *) over;
}

/* we shall now 
define the
 consumer function
 */
void *
ConsumerFunctions ()
{
  bool over = false;
  char VALTEMP;
  while (!over)
    {
      sem_wait (&full);
      sem_wait (&mutex);
      beg++;
      //*sleep for one s *//
      sleep (1);
      if ((VALTEMP = buff->value[BUFFER_WRAP (beg)]) != '*')
	{
	  printf ("\nNow consuming: %c", VALTEMP);
	  counter->value++;
	}
      else
	{
	  over = true;
	}
      sem_post (&mutex);
      sem_post (&empty);
    }
  return (void *) over;
}

//*****************************************************************************THE END**************************************************************************//
