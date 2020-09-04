# pm-sort

## How to Run 

First, you should modify the MAP_FILE path in main.cpp, to your dax-enabled persistent file (which should be large enough).

Then you type `make`, a **sort** file, which is executable, will be generated

Type `./sort` to see the help information and you can see the guideline below:

'''
/sort nvm_type total_num mem_num sort_method
nvm_type: 0 = dram_emulated; 1 = optane PM
total_num: 100, 1000, 10000, ... decides NVM space size
mem_num: 10, 100, 1000, ... decides DRAM space size
alpha: 0,1,2,..., 10 tunes segmentSort only
sort_method: 0 = selectSort, 1 = selectSortExtra, 2 = quickSortNVM,  3 = quickSortMem, 4 = ptrQuickSort, 5 = binarySort, 6 = externalSort, 7 = segmentSort, 8 = segmentSortExter
'''

Finally, you can choose the device you want to use, the total number records to be sorted, available DRAM space, and sorting method you want to test.

## Sorting Methods

1. selectSort: swap-based select sorting, which is performed in an in-place update style. The entire records are divided two parts: [sorted, unsotred]. Each turn, a smallest key/record will be chosen from the unsorted part and appended to the sorted part. Finally, the number of unsorted part will be zero and the sorting terminates. Complexity: O(N) NVM writes, O(N^2) NVM reads.

2. selectSortExtra: non-swap-based select sorting, which uses a new region to store the sorted records. Each turn, a smallest key/record, which has not been chosen before, will be chosen from all the records and appended to the new region. A hash set or a bitmap can act as the bookkeepings (to indicate if a record has been chosen before). Finally, all the records will be appended to the new region as sorted ones. Complexity: O(N) NVM writes, O(N^2) NVM reads.

3. quickSortNVM: in-place sorting with dividing the records into two groups in each turn. Complexity: O(NlogN) NVM reads/writes 

4. quickSortMem: load all records into DRAM and make in-place quick sort in DRAM. At last, the sorted records will be persisted back to NVM. Complexity: O(N) NVM reads/writes, O(NlogN) DRAM reads/writes

5. quickSortPtr: load <key,pointer> pairs (where pointer points to the actual record address) of all records into DRAM and make in-place quick sort in DRAM for these pairs. At last, the sorted <key,pointer> pairs will be persisted back to NVM. Sorted records can be read by using the indirection pointers. Complexity: (minimized) O(N) NVM reads/writes, (minimized) O(NlogN) DRAM reads/writes

6. binarySort: build a binary search tree based on the records. Each insert for a new record is a sorting action. This method derives from the paper [B*-sort](https://ieeexplore.ieee.org/document/9031698). Complexity: O(N) NVM writes, O(NlogN) NVM reads

7. segmentSort: a combination of quick sort and select sort based on a tuning parameter - alpha. It can be interpreted as: for N records, N x alpha reocords will be sorted by quick sort and the remained N x (1-alpha) will be sorted by select sort. These two sorted parts will be merged and output to a separate NVM region. Complexity: to be calculated...

8. segmentSortExter: a combination of external sort and select sort based on a tuning parameter - alpha. It can be interpreted as: for N records, N x alpha reocords will be sorted by external sort and the remained N x (1-alpha) will be sorted by select sort. These two sorted parts will be merged and output to a separate NVM region. This methodd derives from the paper [write-limited sort](https://dl.acm.org/doi/10.14778/2732269.2732277). Complexity: to be calculated...

## What's the difference for sorting in NVM

### Compared with DRAM data sorting

Data consistency should be guaranteed. 

In-place sorting is dangerous: may corrupt original data record during system crash.

Better to use a different location (NVM region) to store sorted data.


### Compared with Disk data sorting

Byte-addressable and low read/write latency. 

No need to load data into DRAM. 

Sorting can be performed directly in NVM.

