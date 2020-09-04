//
// Created by HuangKaixin on 2020-01-09.
//

#include "factory.h"
#include <stdio.h>
#include <iostream>

using namespace std;

struct timespec start, endt;
double total_latency = 0;
int total_count = 0;

static int bi_alloc_count = 0;    

void generateRandomKey(Record *TestSet, int num) {
    srand(0);
    for (int i = 0;i < num;i++) {
        TestSet[i].key = random() % INT64_MAX;
        memset(TestSet[i].value, 'e', VALUE_SIZE);
    }

}

void printRecords(Record *records, int num) {
    int linecount = 0;
    for (int i = 0;i < num;i++) {
        cout << records[i].key << " ";
        linecount++;
        if (linecount == 10) {
            cout << endl;
            linecount = 0;
        }
    }
}

void printRecordsPtr(KeyPointer *out, int num) {
    int linecount = 0;
    for (int i = 0;i < num;i++) {
        cout << out[i].pr->key << " ";
        linecount++;
        if (linecount == 10) {
            cout << endl;
            linecount = 0;
        }
    }
}

void readKV(Record *records, Record *readbuf, int num) {
    for (int i = 0;i < num;i++) {
        memcpy(readbuf, &records[i], RECORD_SIZE);
    }
}

void readPtr(KeyPointer *pointers, Record *readbuf, int num) {
    for (int i = 0;i < num;i++) {
        memcpy(readbuf, pointers[i].pr, RECORD_SIZE);
    }
}

BiNode *nvm_alloc(BiNode *nvm_addr) {
    // cout << "bi_alloc_count = " << bi_alloc_count << endl;
    return (nvm_addr + bi_alloc_count++);
}

double singleLatency() {
    double latency = (endt.tv_sec - start.tv_sec) * NS_RATIO + (double)(endt.tv_nsec - start.tv_nsec) / 1000;
    printf("latency (us): %f\n", latency);
    return latency;
}

double addToTotalLatency() {
    total_latency += singleLatency();
    total_count++;
    return total_latency;
}

double printTotalLatency() {
    printf("total latency (us): %f\n", total_latency);
    return total_latency;
}

void resetTotalLatency() {
    total_latency = 0;
    total_count = 0;
}

double printAverageLatency() {
    double average_lat = total_latency / total_count;
    printf("average latency (us): %f\n", average_lat);
    return average_lat;
}

double printThroughput() {
    double throughput = total_count / total_latency;
    printf("throughput (MOps/s): %f\n", throughput);
    return throughput;
}
