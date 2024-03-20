#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_BUCKETS 5
#define NUM_THREADS 5
#define ARRAY_SIZE 100
#define MAX_RANDOM_VALUE 1000

typedef struct {
    int* elements;
    int size;
} Bucket;

Bucket buckets[NUM_BUCKETS];
int globalArray[ARRAY_SIZE];

void initializeArray(int* array, int size, int maxValue) {
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        array[i] = rand() % maxValue;
    }
}

void initializeBuckets(Bucket* buckets, int numBuckets) {
    for (int i = 0; i < numBuckets; i++) {
        buckets[i].elements = (int*)malloc((ARRAY_SIZE / numBuckets) * sizeof(int));
        buckets[i].size = 0;
    }
}

void distributeToBuckets(int* array, Bucket* buckets, int arraySize, int numBuckets) {
    for (int i = 0; i < arraySize; i++) {
        int bucketIndex = i % numBuckets;
        buckets[bucketIndex].elements[buckets[bucketIndex].size++] = array[i];
    }
}

void* sortBucket(void* arg) {
    Bucket* bucket = (Bucket*)arg;
    // Insertion sort
    for (int i = 1; i < bucket->size; i++) {
        int key = bucket->elements[i];
        int j = i - 1;

        while (j >= 0 && bucket->elements[j] > key) {
            bucket->elements[j + 1] = bucket->elements[j];
            j--;
        }
        bucket->elements[j + 1] = key;
    }
    pthread_exit(NULL);
}

void mergeBuckets(Bucket* buckets, int* array, int numBuckets) {
    int index = 0;
    for (int i = 0; i < numBuckets; i++) {
        for (int j = 0; j < buckets[i].size; j++) {
            array[index++] = buckets[i].elements[j];
        }
        free(buckets[i].elements);
    }
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct timespec start, end;

    initializeArray(globalArray, ARRAY_SIZE, MAX_RANDOM_VALUE);
    initializeBuckets(buckets, NUM_BUCKETS);
    distributeToBuckets(globalArray, buckets, ARRAY_SIZE, NUM_BUCKETS);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, sortBucket, (void*)&buckets[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    mergeBuckets(buckets, globalArray, NUM_BUCKETS);

    double executionTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Total Execution Time: %.6f seconds\n", executionTime);

    return 0;
}
