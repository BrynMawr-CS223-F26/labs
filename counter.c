#include <pthread.h>
#include <stdio.h>

#define N 1000000

long counter = 0;

// TODO: Declare and initialize a mutex.

void *increment(void *arg) {
  for (int i = 0; i < N; i++) {
    // TODO: Protect this operation.
    counter++;
  }
  return NULL;
}

int main(void) {
  pthread_t t1, t2;

  pthread_create(&t1, NULL, increment, NULL);
  pthread_create(&t2, NULL, increment, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("Final counter: %ld\n", counter);

  // TODO: Destroy the mutex if needed.
  return 0;
}
