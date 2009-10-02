#include "../liblcd/lcd.h"

typedef struct term_stat
{
  char * lines[16];
  char index[16];
} term_stat;

void term_init(term_stat * ts);

void term_print(term_stat * ts);

void term_pushstr(term_stat * ts, char * str);
