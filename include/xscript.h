#ifndef XSCRIPT_H
#define XSCRIPT_H

void xscript_remove(char *sfile);
int xscript_create(char *sfile);
void xscript_run(char *sfile);
int xscript_add(char *sfile, const char *format, ...);

#endif /* XSCRIPT_H */
