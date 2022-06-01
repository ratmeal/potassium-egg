[BITS 64]
global page_fault_asm
global divide_by_zero_asm
global double_fault_asm
global bound_range_asm
extern page_fault
extern divide_error
extern double_fault_handler
extern bound_range_exceeded
page_fault_asm:
    ; get error code from stack
    pop rdi
    call page_fault
    ; return to caller
    iretq
divide_by_zero_asm:
    call divide_error
    iretq
double_fault_asm:
    pop rdi
    call double_fault_handler
    iretq
bound_range_asm:
    call bound_range_exceeded
    iretq