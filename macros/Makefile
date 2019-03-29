# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

RC     := root-config
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
else
ROOTSYS = /local/home/ssuvorov/T2K/work/frozen/ROOT/v5r34p34n00/Linux-x86_64
include $(ROOTSYS)/Makefile.arch
endif
-include ../MyConfig.mk

#------------------------------------------------------------------------------


##CXX           =
ObjSuf        = o
SrcSuf        = C
OutPutOpt     = -o
OutPutSuf   	= exe


##turning on debuging 6/11/13
CXXFLAGS += -g

OBJS= $(SRC:.$(SrcSuf)=.$(ObjSuf)) ilc/mydict.o
MCR=  $(SRC:.$(SrcSuf)=)
RLXX=$(LFLAGS) $(ROOTLIBS) -lMinuit -lTreePlayer

##turning on debuging 9/26/2016
RLXX +=  -g

all: $(MCR)

$(MCR): $(OBJS)
	$(LD) $^ $(OutPutOpt) $@.$(OutPutSuf) $(RLXX)
	rm *.$(ObjSuf)

clean:
	rm -f *.$(ObjSuf) ilc/*.$(ObjSuf) *.exe

