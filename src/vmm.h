#include <stdint.h>
struct PageMap
{
    struct lock l;
    uint64_t *top_level;
    void *mmap_ranges[];
};
extern uint64_t page_size;
extern uint64_t higher_half;
