void dec_to_str(uint64_t dec, char* str);
void hex_to_str(uint64_t hex, char* str);
void set_limine_terminal(struct limine_terminal_request volatile request, struct limine_terminal* volatile terminal);
void kprintf_limine(char* str, unsigned long length);