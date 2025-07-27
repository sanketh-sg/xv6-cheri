#include <stdint.h>

enum procstate { NONE, READY, RUNNING, BLOCKED };

struct pcbentry{
    void * __capability pc;
    void * __capability sp;
    void * __capability phybase;
    void * __capability page_table_base;
    enum procstate state;
    uint64_t wakeuptime;
};