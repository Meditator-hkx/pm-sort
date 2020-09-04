#pragma once

#include<time.h>
#include<iostream>
#include<string>
#include <string.h>
#include "store.h"

extern struct timespec start, endt;
extern double total_latency;
extern int total_count;

#define startTimer(start) (clock_gettime(CLOCK_REALTIME, &start))
#define endTimer(endt) (clock_gettime(CLOCK_REALTIME, &endt))
#define NS_RATIO (1000UL * 1000)


void generateRandomKey(Record *TestSet, int num);
void printRecords(Record *records, int num);
void printRecordsPtr(KeyPointer *out, int num);
void readKV(Record *records, Record *readbuf, int num);
void readPtr(KeyPointer *pointers, Record *readbuf, int num);
BiNode *nvm_alloc(BiNode *nvm_addr);

double printLatency();
double singleLatency();
double addToTotalLatency();
double printTotalLatency();
void resetTotalLatency();
double printAverageLatency();
double printThroughput();

