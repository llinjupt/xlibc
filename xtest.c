#include <libx.h>

/* first you need to enable -DTEST in Makefile */
void test_xipaddr();
void test_xmac();
void test_xscript();
void test_xarray();
void test_xsystem(int argc, char *argv[]);
void test_xgetcmdline(int argc, char *argv[]);
void test_xalloc();
void test_xcharstr();
void test_xfile();
void test_xexccmd();
void test_xtime();
void test_xnetif();
void test_xheap();
void test_xcrc32();
void test_xcrc8();
void test_socket();
void test_xurl();
void test_xfcfg();
void test_sm_func();
void test_eloop();
int main(int argc, char *argv[])
{
#ifdef TEST  
  test_xipaddr();
  test_xscript();
  test_xarray();
  test_xsystem(argc, argv);
  test_xgetcmdline(argc, argv);
  test_xalloc();
  test_xcharstr();
  test_xfile();
  test_xmac();
  test_xexccmd();
  test_xtime();
  test_xnetif();
  test_xheap();
  test_xcrc32();
  test_xcrc8();
  test_socket();
  test_xurl();
  test_xfcfg();
  test_eloop();
#endif

  return 0;
}
