int main(void);
void timerinit(void);
void init_uart0(void);
void interruptinit(void);
void plic_cap_init(void);
void panic(char *s);
void putachar(char c);
void printstring(const char *s);
char getachar(void);
