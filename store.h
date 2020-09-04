#pragma once

#include <cstdint>

#define V2K_RATIO 1
#define KEY_SIZE 8
#define POINTER_SIZE 8
#define VALUE_SIZE (V2K_RATIO * KEY_SIZE)
#define RECORD_SIZE (sizeof(Record))
#define KEYPTR_SIZE (sizeof(KeyPointer))


typedef struct {
    uint64_t key;
    char value[VALUE_SIZE];
} Record __attribute__((packed));

typedef struct {
    uint64_t key;
    Record *pr;
} KeyPointer __attribute__((packed));

// In-NVM / In-DRAM
typedef struct BiNode {
    uint64_t key;
    Record *pr;
    BiNode *left;
    BiNode *right;
} BiNode;

