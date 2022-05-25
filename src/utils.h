void dec_to_str(uint64_t dec, char* str);
void hex_to_str(uint64_t hex, char* str);
void set_limine_terminal(struct limine_terminal_request volatile request, struct limine_terminal* volatile terminal);
void kprintf_limine(char* str, unsigned long length);
uint64_t align_up(uint64_t value, uint64_t alignment);
void *memset(void *dest, int c, uint64_t size);
void bitreset(void *bitmap, uint64_t index);