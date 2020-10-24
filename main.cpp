#include <iostream>
#include <string.h>
#include "store.h"
#include "sort.h"
#include "factory.h"
#include "trxn.h"

#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
    // debug
    int nvm_type, total_num, mem_num, sort_method;
    float alpha;

    if (argc < 6) {
        cout << "./sort_test nvm_type total_num mem_num sort_method" << endl;
        cout << "nvm_type: 0 = dram_emulated; 1 = optane PM" << endl;
        cout << "total_num: 100, 1000, 10000, ... decides NVM space size" << endl;
        cout << "mem_num: 10, 100, 1000, ... decides DRAM space size" << endl;
        cout << "alpha: 0,1,2,..., 10 tunes segmentSort only" << endl;
        cout << "sort_method: 0 = selectSort, 1 = selectSortExtra, 2 = quickSortNVM, "
                     " 3 = quickSortMem, 4 = ptrQuickSort, 5 = binarySort, 6 = externalSort, 7 = segmentSort, 8 = segmentSortExter" << endl;
        return 0;
    }

    nvm_type = atoi(argv[1]);
    total_num = atoi(argv[2]);
    mem_num = atoi(argv[3]);
    alpha = (float)atoi(argv[4]) / 10;
    sort_method = atoi(argv[5]);

//    nvm_type = 0;
//    total_num =1000;
//    mem_num = 500;
//    alpha = 0;
//    sort_method = 7;

    char *dram_base = (char *)malloc(RECORD_SIZE * mem_num);
    char *nvm_base;
    char *out_base;

    if (nvm_type == 0) {
        nvm_base = (char *)mmap(NULL, RECORD_SIZE * total_num * 4, \
                PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
        out_base = nvm_base + RECORD_SIZE * total_num;
    }
    else {
        int fd = open("/home/kaixin/pmdir/pmfile", O_CREAT | O_RDWR);
        nvm_base = (char *)mmap(0, RECORD_SIZE * total_num * 4, \
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        // cout << "nvm address: " << (uint64_t)nvm_base << endl;            
        out_base = nvm_base + RECORD_SIZE * total_num;
    }

    Record *records = (Record *)nvm_base;
    Record *buffer = (Record *)dram_base;
    Record *outfile = (Record *)out_base;
    BiNode *bi_base = (BiNode *)(outfile + total_num);
    KeyPointer *outptr = (KeyPointer *)out_base;
    KeyPointer *ptrbuffer = (KeyPointer *)malloc(KEYPTR_SIZE * total_num);
    Record *readbuf = (Record *)malloc(RECORD_SIZE);

    generateRandomKey(records, total_num);
    clear_cache((char *)records, total_num * RECORD_SIZE);
//    printRecords(records, total_num);

    if (nvm_type == 0) {
        cout << "dram ";
    }
    else {
        cout << "nvm ";
    }
    startTimer(start);
    if (sort_method == 0) {
        cout << "selectSort: ";
        if (nvm_type == 0) {
            selectSort(records, total_num);
        }
        else {
            memcpy(outfile, records, total_num * RECORD_SIZE);
            selectSort(outfile, total_num);
        }
    }
    else if (sort_method == 1) {
        cout << "selectSortExtra: ";
        selectSortExtra(records, outfile, total_num);
    }
    else if (sort_method == 2) {
        cout << "quickSort: ";
        if (nvm_type == 0) {
            quickSort(records, total_num);
        }
        else {
            memcpy(outfile, records, total_num * RECORD_SIZE);
            quickSort(outfile, total_num);
        }
    }
    else if (sort_method == 3) {
        cout << "quickSortMem: ";
        quickSortMem(records, outfile, buffer, total_num);
    }
    else if (sort_method == 4) {
        cout << "ptrQuickSort: ";
        ptrQuickSort(records, ptrbuffer, outptr, total_num);
    }
    else if (sort_method == 5) {
        cout << "binarySort: ";
        binarySort(records, outfile, bi_base, total_num);
    }
    else if (sort_method == 6) {
        cout << "externalSort: ";
        externalSort(records, buffer, outfile, total_num, mem_num);
    }
    else if (sort_method == 7) {
        cout << "segmentSort: ";
        if (nvm_type == 1) {
            memcpy(outfile, records, total_num * RECORD_SIZE);
        }
        segmentSort(records, outfile, total_num, alpha);
    }
    else if (sort_method == 8) {
        cout << "segmentSortExt: ";
        segmentSortExt(records, buffer, outfile, total_num, mem_num, alpha);
    }
    // add more methods below
    else {

    }
    endTimer(endt);
    singleLatency();

    cout << "Read Results: ";
    // flush all results
    clear_cache((char *)records, total_num * RECORD_SIZE);
    clear_cache((char *)outfile, total_num * RECORD_SIZE);
    clear_cache((char *)outptr, total_num * KEYPTR_SIZE);
    startTimer(start);
    if (nvm_type == 0) {
        switch(sort_method) {
        case 0:
        case 2:
            readKV(records, readbuf, total_num);
            break;
        case 1:
        case 3:
        case 6:
        case 7:
        case 8:
            readKV(outfile, readbuf, total_num);
            break;
        case 4:
        case 5:
            readPtr(outptr, readbuf, total_num);
            break;
        default:
            readKV(records, readbuf, total_num);
        }
    }
    else {
        if (sort_method == 4) {
            readPtr(outptr, readbuf, total_num);
        }
        else {
            readKV(outfile, readbuf, total_num);
        }
    }
    endTimer(endt);
    singleLatency();

    free(dram_base);
    munmap(nvm_base, RECORD_SIZE * total_num * 4);

    return 0;

}
