#ifndef SERIAL.h
#define SERIAL.h
int serial_init();
int serial_received();
char read_serial();
void serial_print(char* str);
#endif