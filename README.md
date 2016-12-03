# xlib
===
An eXtended C library of collected functions about genral String, Memory, DB,<br> 
Algorithm, Net and etc. operations.<br> 
<br> 
Not like glibc which is a huge and complex lib, this xlib is a tiny extened<br> 
function groups, used for general Unix-Like OS, especially for Embedded systems. <br> 
It aims to help engineens develop a C program quickly and canonically. Lots of<br> 
functions were enlightened by other open source projects.<br> 
<br> 
There are a lot of parts listed below, and will be extended day after day.<br> 
<br> 
1. Character, String  and file functions with security checking.<br> 
2. Very genral and convenient bitmap fucntions.<br> 
3. A tiny memory tracking system. <br> 
4. An useful logging and debugging system: xdebug and xlogxxx.<br> 
5. Data Structure: list, array , heap and queue functions<br> 
6. Net functions with ip/mac address. <br> 
7. TCP/UDP functions encapsulated for quick network operations<br> 
8. CRC, Hash and base64 algrithms<br> 
9. An system-like funtion xexccmd.c help to call other cmd quickly.<br> 
10. Some third party opensource libs or .c files: cJSON.c base64.c<br> 
...<br> 
<br> 
How to compile, test and use xlib:<br> 
---
1. Cross compile xlib<br> 
make.def defined the Cross compile tools' path. Redefine CROSS_COMPILE<br> 
macro with your own toolchain.<br> 
<br> 
2. How to make<br> 
./make will generate libx.a and libx.so, use them with -L and -l directly.<br> 
<br> 
3. How to test xlib functions<br> 
---
Enable CFLAGS  += -DTEST in Makefile and do make xtest, then run it to<br> 
traverse all test functions defined in every individual xxx.c files.<br> 
<br> 
4. How to extend the xlib<br> 
It's easy no bother Makefile. Just put the public .h files to ./include <br> 
directory and .c files to ./src. And remake it. xlib only with one <br> 
Makefile and do everything well automatically. Enjoy it.<br> 
<br> 
Note: Third party codes collected obey its own licenses.<br> 
