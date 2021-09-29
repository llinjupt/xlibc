# A generic template Makefile
#
# This file is a(part of) free software; you can redistribute it 
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2, or 
# (at your option) any later version.
include ./make.def

.PHONY = all install clean

#global directory defined
GDB          = 0
TOPDIR       = $(shell pwd)
SRCDIR	     = $(TOPDIR)/src
TARGET_DIR   = $(TOPDIR)/
LIB_DIR      = $(TOPDIR)/lib
OBJECTDIR    = $(TOPDIR)/build
INCLUDEDIR   = $(TOPDIR)/include

#cross compile tools defined 
#defined in make.def

#local host tools defined
#defined in make.def

#target name
TARGETMAIN  = xtest
TARGETLIBS  = libx.a
TARGETSLIBS = libx.so

#FILE' INFOMATION COLLECT
VPATH       = $(shell ls -AxR $(SRCDIR)|grep ":"|grep -v "\.svn"|tr -d ':')
SOURCEDIRS  = $(VPATH)

#search source file in the current dir
SOURCES     = $(foreach subdir,$(SOURCEDIRS),$(wildcard $(subdir)/*.c))
OBJS        = $(patsubst %.c,%.o,$(SOURCES))
BUILDOBJS   = $(subst $(SRCDIR),$(OBJECTDIR),$(OBJS))
DEPS        = $(patsubst %.o,%.d,$(BUILDOBJS))

#external include file define
CFLAGS	= -Wall -MD $(foreach dir,$(INCLUDEDIR),-I$(dir))
ARFLAGS = rc

#special parameters for apps
CFLAGS	+= #-DTEST

ifeq ($(GDB),1)
CFLAGS += -ggdb3
endif

#c file compile parameters and linked libraries
CPPFLAGS   = -fPIC
LDFLAGS	   = -lm
XLDFLAGS   = -Xlinker "-(" $(LDFLAGS) -Xlinker "-)"
LDLIBS     += -L $(LIB_DIR) 

#defaut target:compile the currrent dir file and sub dir 
all: $(TARGETLIBS) $(TARGETSLIBS)

#for .h header files dependence
-include $(DEPS)

$(TARGETMAIN) :$(BUILDOBJS) $(TARGETMAIN).c
	$(CC) $(subst $(SRCDIR),$(OBJECTDIR),$^) $(CPPFLAGS) $(CFLAGS) -lpthread $(XLDFLAGS) -o $@ $(LDLIBS)
	@$(STRIP)  --strip-unneeded $(TARGETMAIN)
	@$(RM) -f *.d *.o

$(TARGETLIBS) :$(BUILDOBJS)
	@$(AR) $(ARFLAGS) $@ $(BUILDOBJS)
	@$(RANLIB) $@

$(TARGETSLIBS) :$(BUILDOBJS)
	@$(CC) -shared $(subst $(SRCDIR),$(OBJECTDIR),$^) $(CPPFLAGS) $(CFLAGS) $(XLDFLAGS) -o $@ $(LDLIBS)

$(OBJECTDIR)%.o: $(SRCDIR)%.c
	@[ ! -d $(dir $(subst $(SRCDIR),$(OBJECTDIR),$@)) ] & $(MKDIR) -p $(dir $(subst $(SRCDIR),$(OBJECTDIR),$@))
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $(subst $(SRCDIR),$(OBJECTDIR),$@) -c $<

install:

clean:
	@$(FIND) $(OBJECTDIR) -name "*.o" -o -name "*.d" | $(XARGS) $(RM) -f
	@$(RM) -f *.d *.o
	@$(RM) -f $(TARGETMAIN) $(TARGETLIBS) $(TARGETSLIBS)
