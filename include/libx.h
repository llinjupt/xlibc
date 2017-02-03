#ifndef _LIBX_H
#define _LIBX_H

#ifndef bool
#define bool char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include "xalloc.h"
#include "xdebug.h"
#include "xexccmd.h"
#include "xcharstr.h"
#include "xbitmap.h"
#include "xtime.h"
#include "xfile.h"
#include "xarray.h"
#include "xlist.h"
#include "xhash.h"
#include "xrandom.h"

#include "xsocket.h"
#include "xioctl.h"
#include "xnetif.h"
#include "xipaddr.h"
#include "xmac.h"
#include "xsha1.h"
#include "xcrc32.h"

#include "xsystem.h"
#include "xheap.h"
#include "xepoll.h"
#include "xipcmsg.h"
#include "xqueue.h"
#include "cJSON.h"

/* en/decoding */
#include "xurl.h"

/* xfile configure loader */
#include "xfcfg.h"

#endif /* _LIBX_H */
