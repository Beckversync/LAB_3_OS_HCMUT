#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];
int count;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex to protect logbuf

void flushlog();

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
   char str[MAX_LOG_LENGTH];
   int id = *(int*) data;

   usleep(20); // Simulate delay (e.g., I/O)

   sprintf(str, "%d", id);

   // Lock the mutex before accessing logbuf
   pthread_mutex_lock(&log_mutex);

   // Only write to logbuf if there's space (count < MAX_BUFFER_SLOT)
   if (count < MAX_BUFFER_SLOT) {
       strcpy(logbuf[count], str);
       count++; // Increment count after writing log
   } else {
       // If buffer is full, overwrite the oldest slot (cyclic buffer)
       count = 0;
       strcpy(logbuf[count], str);
   }

   // Unlock the mutex after accessing logbuf
   pthread_mutex_unlock(&log_mutex);

   return NULL;
}

void flushlog()
{
   int i;
   char nullval[MAX_LOG_LENGTH];

   // Lock the mutex before accessing logbuf
   pthread_mutex_lock(&log_mutex);

   if (count > 0) {
       // Print the current state of logbuf slots
       for (i = 0; i < MAX_BUFFER_SLOT; i++) {
           printf("Slot %i: %s\n", i, logbuf[i]);
       }
   }

   // Unlock the mutex after accessing logbuf
   pthread_mutex_unlock(&log_mutex);

   fflush(stdout); // Ensure all output is printed immediately
}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog(); // Periodically call flushlog
      usleep(((struct _args *) args)->interval); // Wait until the next timeout
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];

   struct _args args;
   args.interval = 1000000; // 1 second interval (1000000 microseconds)

   /* Set up a periodic call to flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /* Asynchronously invoke wrlog() */
   for (i = 0; i < MAX_LOOPS; i++)
   {
      id[i] = i;
      pthread_create(&tid[i], NULL, wrlog, (void*) &id[i]);
   }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL); // Wait for all wrlog threads to finish

   sleep(5); // Allow some time for any remaining logs to be flushed

   return 0;
}
