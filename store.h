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
} __attribute__((packed)) Record;

typedef struct {
    uint64_t key;
    Record *pr;
} __attribute__((packed)) KeyPointer;

// In-NVM / In-DRAM
typedef struct BiNode {
    // uint64_t key;
    // Record *pr;
    Record record;
    BiNode *left;
    BiNode *right;
} BiNode;

