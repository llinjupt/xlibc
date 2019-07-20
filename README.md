# xlibc

An eXtended C library with lots of common used functions and data structures
as genral String, Memory, DB, Algorithm, Net and etc. operations.

And this tiny lib uses one-for-all Makefile, refet to [a common Makefile](http://blog.chinaunix.net/uid-20608849-id-360294.html),
with it you do not need to update Makefile when you add new .c, .h files and folders.

Not like glibc which is a huge and complex lib, this xlibc is a tiny extened
function groups, used for general Unix-Like OS, especially for Embedded systems. 
It aims to help engineens develop a C program quickly and canonically. Lots of 
functions were enlightened by other open source projects.

There are a lot of parts listed below, and will be extended day after day.

1. Character, String  and file functions with security checking.
2. Very genral and convenient bitmap fucntions.
3. A tiny memory tracking system. 
4. Useful logging and debugging APIs: [xdebug](http://blog.chinaunix.net/uid-20608849-id-365933.html).
5. Data Structure: list, array , heap and queue functions 
6. Net functions with ip/mac address. 
7. TCP/UDP functions encapsulated for quick network operations 
8. CRC, Hash and base64 algrithms 
9. An system-like funtion xexccmd.c help to call other cmd quickly.
10. Some third party opensource libs or .c files: cJSON.c base64.c 
  ...

How to compile, test and use xlibc:

1. Cross compile xlibc 
make.def defined the Cross compile tools' path. Redefine CROSS_COMPILE 
macro with your own toolchain.

2. How to make 
./make will generate libx.a and libx.so, use them with -L and -l directly.

3. How to test xlibc functions 
Enable CFLAGS  += -DTEST in Makefile and do make xtest, then run it to 
traverse all test functions defined in every individual xxx.c files.

4. How to extend the xlibc 
It's easy no bother Makefile. Just put the public .h files to ./include 
directory and .c files to ./src. And remake it. xlibc only with one 
Makefile and do everything well automatically. Enjoy it.

Note: Third party codes collected obey its own licenses.

