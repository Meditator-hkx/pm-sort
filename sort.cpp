#include "sort.h"
#include <iostream>

using namespace std;

// O(N^2) reads, O(N) writes
// this method requires 2N writes in fact, because data will swap
void selectSort(Record *records, int num) {
    // N rounds, each with the smallest number with the smallest index
    int count = 0;
    int base;
    int index;
    Record trecord;

    while (count < num) {
        // select the base number and base index
        base = records[count].key;
        index = count;

        // select the smallest element
        for (int i = count + 1;i < num;i++) {
            if (records[i].key < base) {
                base = records[i].key;
                index = i;
            }
        }

        // swap the smallest element with [count]
        if (count != index) {
            memcpy(&trecord, &records[count], RECORD_SIZE);
            memcpy(&records[count], &records[index], RECORD_SIZE);
            memcpy(&records[index], &trecord, RECORD_SIZE);
        }
        count++;
    }
}

// difference with selectSort: no need to swap in-place
// only N writes (to new NVM space) are required
void selectSortExtra(Record *records, Record *out, int num) {
    bool *visited = (bool *)malloc(sizeof(bool) * num);
    for (int i = 0;i < num;i++)
        visited[i] = false;
    int count = 0;
    uint64_t base;
    int index;


    while (count < num) {
        int i = 0;
        while (i < num && visited[i]) {
            i++;
        }
        base = records[i].key;
        index = i;

        while (i < num) {
            if (records[i].key < base && !visited[i]) {
                base = records[i].key;
                index = i;
            }
            i++;
        }

        // write the smallest to output
        memcpy(&out[count], &records[index], RECORD_SIZE);
        visited[index] = true;
        count++;
    }
}


// O(NlogN) reads and writes
void quickSort(Record *records, int num) {
    quickSortReal(records, 0, num-1);
}

void quickSortReal(Record *records, int low, int high) {
    if (low < high) {
        int i = low, j = high;
        Record trecord;
        memcpy(&trecord, &records[low], RECORD_SIZE);

        while (i < j) {
            while (i < j && records[j].key >= trecord.key)
                j--;
            if (i < j) {
                memcpy(&records[i], &records[j], RECORD_SIZE);
                i++;
            }

            while(i < j && records[i].key <=  trecord.key)
                i++;
            if (i < j) {
                memcpy(&records[j], &records[i], RECORD_SIZE);
                j--;
            }
        }
        memcpy(&records[i], &trecord, RECORD_SIZE);

        quickSortReal(records, low, i - 1);
        quickSortReal(records, i + 1, high);
    }
}

void quickSortMem(Record *nvm_records, Record *out, Record *dram_records, int num) {
    memcpy(dram_records, nvm_records, num * RECORD_SIZE);
    quickSort(dram_records, num);
    memcpy(out, dram_records, num * RECORD_SIZE);
}

void ptrQuickSort(Record *records, KeyPointer *pointers, KeyPointer *outptr, int num) {
    // load all pointers to DRAM space
    for (int i = 0;i < num;i++) {
        pointers[i].key = records[i].key;
        pointers[i].pr = &records[i];
    }

    // sort for all pointers
    ptrQuickSortReal(pointers, 0, num-1);

    // output pointers to NVM
    memcpy(outptr, pointers, num * KEYPTR_SIZE);
}

void ptrQuickSortReal(KeyPointer *pointers, int low, int high) {
    if (low < high) {
        int i = low, j = high;
        KeyPointer precord;
        memcpy(&precord, &pointers[low], KEYPTR_SIZE);

        while (i < j) {
            while (i < j && pointers[j].key >= precord.key)
                j--;
            if (i < j) {
                memcpy(&pointers[i], &pointers[j], KEYPTR_SIZE);
                i++;
            }

            while(i < j && pointers[i].key <= precord.key)
                i++;
            if (i < j) {
                memcpy(&pointers[j], &pointers[i], KEYPTR_SIZE);
                j--;
            }
        }
        memcpy(&pointers[i], &precord, KEYPTR_SIZE);

        ptrQuickSortReal(pointers, low, i - 1);
        ptrQuickSortReal(pointers, i + 1, high);
    }
}

// O(NlogN) reads and writes
void binarySort(Record *records, KeyPointer *pointers, BiNode *free_nvm, int num) {
    // build a binary tree for all elements
    // BiNode *root = new BiNode;
    BiNode *root = nvm_alloc(free_nvm);
    root->key = records[0].key;
    root->pr = &records[0];
    root->left = root->right = NULL;
    for (int i = 1;i < num;i++) {
        binaryInsert(root, records[i], free_nvm);
    }
    binaryScan(root, pointers, num);
}

void binaryInsert(BiNode *root, Record &record, BiNode *free_nvm) {
    BiNode *newNode;

    if (record.key >= root->key) {
        if (root->right == NULL) {
            // newNode = new BiNode;
            newNode = nvm_alloc(free_nvm);
            newNode->key = record.key;
            newNode->pr = &record;
            newNode->left = newNode->right = NULL;
            root->right = newNode;
        }
        else {
            binaryInsert(root->right, record, free_nvm);
        }
    }
    else {
        if (root->left == NULL) {
            // newNode = new BiNode;
            newNode = nvm_alloc(free_nvm);
            newNode->key = record.key;
            newNode->pr = &record;
            newNode->left = newNode->right = NULL;
            root->left = newNode;
        }
        else {
           binaryInsert(root->left, record, free_nvm);
        }
    }
}
// write (key, ptr) to NVM as sorted file
void binaryScan(BiNode *root, KeyPointer *out_ptr, int num) {
    vector<KeyPointer> outs(num);
    binaryScanRecur(root, outs);
    for (int i = 0;i < outs.size();i++) {
        memcpy(&out_ptr[i], &outs[i], KEYPTR_SIZE);
    }
}
void binaryScanRecur(BiNode *root, vector<KeyPointer> &outs) {
    // read left
    // read middle
    // read right
    KeyPointer kp;

    if (root == NULL) {
        return;
    }

    if (root->left) {
        binaryScanRecur(root->left, outs);
    }
    kp.key = root->key;
    kp.pr = root->pr;
    outs.push_back(kp);
    if (root->right) {
        binaryScanRecur(root->right, outs);
    }
}

void externalSort(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num) {
    // case 1: DRAM space enough, in fact a quickSort
    if (total_num <= mem_num) {
        memcpy(dram_records, nvm_records, total_num * RECORD_SIZE);
        quickSort(dram_records, total_num);
        memcpy(outs, dram_records, total_num * RECORD_SIZE);
        return;
    }

    // case 2: small DRAM as buffer
    divideSort(nvm_records, dram_records, total_num, mem_num);
    mergeSortReal(nvm_records, dram_records, outs, total_num, mem_num);
}

void divideSort(Record *nvm_records, Record *dram_records, int total_num, int mem_num) {
    int left_num = total_num;
    int grp_num = total_num / mem_num;
    int sort_num;
    for (int i = 0;i <= grp_num;i++) {
        // load into DRAM
        sort_num = min(mem_num, left_num);
        memcpy(dram_records, nvm_records + mem_num*i, sort_num * RECORD_SIZE);
        // quickSort
        quickSort(dram_records, sort_num);
        // store into NVM
        memcpy(nvm_records + mem_num*i, dram_records, sort_num * RECORD_SIZE);
        left_num -= sort_num;
        if (left_num == 0) {
            break;
        }
    }
//    printRecords(nvm_records, total_num);
}

void mergeSortReal(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num) {
    int grp_num = total_num / mem_num;
    bool extra_flag = false;
    int extra_num = 0;
    if (total_num % mem_num != 0) {
        extra_flag = true;
        extra_num = total_num % mem_num;
    }

    int dram_count = 0;
    int out_count = 0;
    int *counts = (int *)malloc(sizeof(int) * (grp_num+1));
    for (int i = 0;i < grp_num+1;i++) {
        counts[i] = 0;
    }

    int deal_num = 0;
    set<int> completed_grp;

    uint64_t min_base;
    int index;

    while (deal_num < total_num) {
        // select a smallest element from nvm records => insert into dram
        int i = 0;
        while (i < grp_num) {
            if (completed_grp.count(i) == 0) {
                break;
            }
            else {
                i++;
            }
        }
        if (i < grp_num) {
            min_base = nvm_records[i*mem_num+counts[i]].key;
            index = i;
        }
        else if (i == grp_num && extra_flag && completed_grp.count(i) == 0) {
            min_base = nvm_records[i*mem_num+counts[i]].key;
            index = i;
        }
        else {
            break;
        }

        while (i < grp_num) {
            if (completed_grp.count(i) == 0 && nvm_records[i*mem_num+counts[i]].key < min_base) {
                min_base = nvm_records[i*mem_num+counts[i]].key;
                index = i;
            }
            i++;
        }
        if (extra_flag && completed_grp.count(i) == 0) {
            if (completed_grp.count(i) == 0 && nvm_records[i*mem_num+counts[i]].key < min_base) {
                min_base = nvm_records[i*mem_num+counts[i]].key;
                index = i;
            }
        }

        if (index < grp_num) {
            memcpy(&dram_records[dram_count], &nvm_records[index*mem_num+counts[index]], RECORD_SIZE);
            counts[index]++;
            if (counts[index] == mem_num) {
                completed_grp.insert(index);
            }
        }
        else {
            memcpy(&dram_records[dram_count], &nvm_records[index*mem_num+counts[index]], RECORD_SIZE);
            counts[index]++;
            if (counts[index] == extra_num) {
                completed_grp.insert(index);
            }
        }

        dram_count++;
        // if dram full, output and reset
        if (dram_count == mem_num) {
            memcpy(&outs[out_count], dram_records, mem_num * RECORD_SIZE);
            out_count += mem_num;
            deal_num += mem_num;
            dram_count = 0;
        }
        // if all sorted, end sorting
        else if (deal_num + dram_count == total_num) {
            memcpy(&outs[out_count], dram_records, dram_count * RECORD_SIZE);
            out_count += dram_count;
            deal_num += dram_count;
            dram_count = 0;
        }
    }
}

void mergeSort(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num) {
    // initiate
    int grp_num = total_num / mem_num;
    int deal_num = 0;
//    int tuple_num_per_grp = total_num / grp_num;
    int buf_num_per_grp = mem_num / (grp_num + 1);
    int iter_num_per_grp = mem_num / buf_num_per_grp;
    int *count = (int *)malloc(sizeof(int) * grp_num);
    int *grpiter = (int *)malloc(sizeof(int) * grp_num);

    for (int i = 0;i < grp_num;i++) {
        int nvm_grp_start_off = mem_num * i;
        int dram_grp_start_off = buf_num_per_grp * i;
        memcpy(dram_records + dram_grp_start_off, nvm_records + nvm_grp_start_off, RECORD_SIZE * buf_num_per_grp);
        count[i] = 0;
        grpiter[i] = 1;
    }

    int off_per_out = 0;
    int out_iter = 0;
    Record *out_buf = dram_records + buf_num_per_grp * grp_num;

    // loop until all items are coped
    int min_seq = 0;
    Record *min_key;
    while (deal_num < total_num) {
        // select the minimum number and its order
        for (int i = 0;i < grp_num;i++) {
            if (count[i] < buf_num_per_grp) {
                min_seq = i;
                break;
            }
        }
        min_key = dram_records + buf_num_per_grp * min_seq + count[min_seq];

        // select the current minimum number as next merging one
        for (int i = 0;i < grp_num;i++) {
            if (count[i] >= buf_num_per_grp) {
                continue;
            }
            if (min_key->key > dram_records[buf_num_per_grp*i+count[i]].key) {
                min_seq = i;
                min_key = dram_records + buf_num_per_grp * min_seq + count[i];
            }
        }

        // swap data into nvm when out buffer is full
        memcpy(out_buf + off_per_out, &dram_records[buf_num_per_grp*min_seq+count[min_seq]], RECORD_SIZE);
        deal_num++;
        off_per_out++;
        if (off_per_out >= buf_num_per_grp) {
            // store data into nvm
            memcpy(outs + buf_num_per_grp*out_iter, out_buf, RECORD_SIZE * buf_num_per_grp);
            out_iter++;
            off_per_out = 0;
        }

        // load data from nvm when group buffer is empty
        count[min_seq]++;
        if (count[min_seq] >= buf_num_per_grp) {
            if (grpiter[min_seq] < iter_num_per_grp) {
                memcpy(dram_records + min_seq*buf_num_per_grp, \
                        nvm_records + mem_num*min_seq+buf_num_per_grp*grpiter[min_seq], \
                        RECORD_SIZE * buf_num_per_grp);
                count[min_seq] = 0;
                grpiter[min_seq]++;
            }
        }
    }
}

// segmentSort = alpha * quickSort + (1-alpha) * selectSort, where alpha can be tuned
void segmentSort(Record *records, Record *out, int total_num, float alpha) {
    int quick_num = alpha * total_num;
    int select_num = total_num - quick_num;
    quickSort(records, quick_num);
    selectSort(records+quick_num, select_num);

    // merge two writes to outfile
    int i = 0, j = quick_num;
    int count = 0;
    while (i < quick_num && j < total_num) {
        if (records[i].key <= records[j].key) {
            memcpy(&out[count], &records[i], RECORD_SIZE);
            i++;
        }
        else {
            memcpy(&out[count], &records[j], RECORD_SIZE);
            j++;
        }
        count++;
    }

    while (i < quick_num) {
        memcpy(&out[count], &records[i], RECORD_SIZE);
        i++; count++;
    }
    while (j < total_num) {
        memcpy(&out[count], &records[j], RECORD_SIZE);
        j++; count++;
    }
}

void segmentSortExt(Record *nvm_records, Record *dram_records, Record *outs, int total_num, int mem_num, float alpha) {
    int exter_num = alpha * total_num;
    int select_num = total_num - exter_num;
    
    externalSort(nvm_records, dram_records, outs, exter_num, mem_num);
    memcpy(nvm_records, outs, RECORD_SIZE * exter_num);
    selectSort(nvm_records+exter_num, select_num);

    // merge two writes to outfile
    int i = 0, j = exter_num;
    int count = 0;
    while (i < exter_num && j < total_num) {
        if (nvm_records[i].key <= nvm_records[j].key) {
            memcpy(&outs[count], &nvm_records[i], RECORD_SIZE);
            i++;
        }
        else {
            memcpy(&outs[count], &nvm_records[j], RECORD_SIZE);
            j++;
        }
        count++;
    }

    while (i < exter_num) {
        memcpy(&outs[count], &nvm_records[i], RECORD_SIZE);
        i++; count++;
    }
    while (j < total_num) {
        memcpy(&outs[count], &nvm_records[j], RECORD_SIZE);
        j++; count++;
    }
}
