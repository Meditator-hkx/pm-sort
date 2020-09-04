#pragma once

#include <vector>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <sys/mman.h>
#include <zconf.h>
#include <set>
#include <vector>
#include "store.h"
#include "factory.h"

using namespace std;

void selectSort(Record *records, int num);
void selectSortExtra(Record *records, Record *out, int num);

void quickSort(Record *records, int num);
void quickSortReal(Record *records, int low, int high);
void quickSortMem(Record *nvm_records, Record *out, Record *dram_records, int num);
void ptrQuickSort(Record *records, KeyPointer *pointers, KeyPointer *outptr, int num);
void ptrQuickSortReal(KeyPointer *pointers, int low, int high);

void externalSort(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num);
void divideSort(Record *nvm_records, Record *dram_records, int total_num, int mem_num);
void mergeSort(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num);
void mergeSortReal(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num);


// void binarySort(Record *records, int num);
void binarySort(Record *records, KeyPointer *pointers, BiNode *free_nvm, int num);
// void binaryInsert(BiNode *root, Record &record);
void binaryInsert(BiNode *root, Record &record, BiNode *free_nvm);
// void binaryScan(BiNode *root, Record *records, int num);
void binaryScan(BiNode *root, KeyPointer *pointers, int num);
// void binaryScanRecur(BiNode *root, vector<uint64_t> &outs);
void binaryScanRecur(BiNode *root, vector<KeyPointer> &outs);

void segmentSort(Record *records, Record *out, int num, float alpha);
void segmentSortExt(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num, float alpha);
