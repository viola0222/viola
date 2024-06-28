#ifndef SERIAL_H_
#define SERIAL_H_

void initSCI(void);

void put1byte(char c);
void putnbyte(char *buf, int len);
int myprintf(const char *fmt, ...);

#endif
