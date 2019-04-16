#----------------------------------------------------------------------
#
# Project     : PandaX-III / T2K-II
#
# Sub-project : Client program for TDCM
#
# Author      : D. Calvet,        denis.calvetATcea.fr
#
# History:
#  Jul 2018: created
#  April 2019: added target fdec
#----------------------------------------------------------------------

#----------------------------------------------------------------------
# Common variables
#----------------------------------------------------------------------
# Platform variables
VAR_OS    = linux

ADD_CFLAGS = -pthread
ADD_LIB    = 

# Change this directory: sources are under this 
DIR_ROOT       = ../../../../projects
PROJ = pandax

DIR_PROJ          =  $(DIR_ROOT)/$(PROJ)
DIR_PROJ_OS       =  $(DIR_PROJ)/$(VAR_OS)

# Directory variables for object files
DIR_OBJ                = ../../../../compil
DIR_OBJ_PROJ           = $(DIR_OBJ)/$(PROJ)
DIR_OBJ_PROJ_OS        = $(DIR_OBJ_PROJ)/$(VAR_OS)

DIR_BIN                = ../../../bin
DIR_BIN_PROJ           = $(DIR_BIN)/$(PROJ)
DIR_BIN_PROJ_OS        = $(DIR_BIN_PROJ)/$(VAR_OS)

DIR_LIB                = ../../../lib
DIR_LIB_PROJ           = $(DIR_LIB)/$(PROJ)
DIR_LIB_PROJ_OS        = $(DIR_LIB_PROJ)/$(VAR_OS)

DIR_OBJ_TARGET       = $(DIR_OBJ_PROJ_OS)

DIR_LIB_TARGET       = $(DIR_LIB_PROJ_OS)

DIR_BIN_TARGET       = $(DIR_BIN_PROJ_OS)

#----------------------------------------------------------------------
# Commands and options
#----------------------------------------------------------------------
CC	 =   gcc
DFLAGS = -DLINUX -DJUMBO_POOL
WFLAGS =   -Wuninitialized -Wimplicit -Wreturn-type
CFLAGS =   -O4 $(DFLAGS) $(WFLAGS) $(ADD_CFLAGS)
LD	 =   gcc

#----------------------------------------------------------------------
# Libraries
#----------------------------------------------------------------------
LIBS  =	-lm -lrt -lpthread

LLIBS =	-L$(DIR_LIB_TARGET)

#----------------------------------------------------------------------
# Include files
#----------------------------------------------------------------------
INCLUDE = \
	-I$(DIR_PROJ)/util/$(VAR_OS) \
	-I$(DIR_PROJ)/util \
	-I$(DIR_PROJ)/platforms \
	-I$(DIR_PROJ)/platforms/$(VAR_OS) \
	-I$(DIR_PROJ)/bufmgr \
	-I$(DIR_PROJ)/cmdi \
	-I$(DIR_PROJ)/spiflash \
	-I$(DIR_PROJ)/format \
	-I$(DIR_PROJ)/common

#----------------------------------------------------------------------
# All targets
#----------------------------------------------------------------------
all : pcli prd fdec

#----------------------------------------------------------------------
# Clean
#----------------------------------------------------------------------
clean : clean_obj clean_lib clean_bin

clean_obj :
	rm $(DIR_OBJ_TARGET)/*.o

clean_lib :
	rm $(DIR_LIB_TARGET)/lib*.a

clean_bin :
	rm $(DIR_BIN_TARGET)/pclient \
	rm $(DIR_BIN_TARGET)/preader \
	rm $(DIR_BIN_TARGET)/fdecoder

#----------------------------------------------------------------------
# Client Program for TDCM
#----------------------------------------------------------------------
pcli : $(DIR_BIN_TARGET)/pclient

$(DIR_BIN_TARGET)/pclient : \
	$(DIR_OBJ_TARGET) \
	$(DIR_BIN_TARGET) \
	$(DIR_OBJ_TARGET)/mclientUDP.o \
	$(DIR_OBJ_TARGET)/femproxy.o \
	$(DIR_OBJ_TARGET)/evbuilder.o \
	$(DIR_OBJ_TARGET)/femarray.o \
	$(DIR_OBJ_TARGET)/cmdfetcher.o \
	$(DIR_OBJ_TARGET)/programflash.o \
	$(DIR_OBJ_TARGET)/hexreader.o \
	$(DIR_OBJ_TARGET)/bufpool.o \
	$(DIR_OBJ_TARGET)/frame.o \
	$(DIR_OBJ_TARGET)/remotecopy.o \
	$(DIR_OBJ_TARGET)/os_al.o
		$(LD) $(CFLAGS) \
			$(DIR_OBJ_TARGET)/mclientUDP.o \
			$(DIR_OBJ_TARGET)/femproxy.o \
			$(DIR_OBJ_TARGET)/evbuilder.o \
			$(DIR_OBJ_TARGET)/femarray.o \
			$(DIR_OBJ_TARGET)/cmdfetcher.o \
			$(DIR_OBJ_TARGET)/programflash.o \
			$(DIR_OBJ_TARGET)/hexreader.o \
			$(DIR_OBJ_TARGET)/bufpool.o \
			$(DIR_OBJ_TARGET)/frame.o \
			$(DIR_OBJ_TARGET)/remotecopy.o \
			$(DIR_OBJ_TARGET)/os_al.o \
			$(LLIBS) \
			$(LIBS) \
			-o $(DIR_BIN_TARGET)/pclient

#----------------------------------------------------------------------
# Create Directories
#----------------------------------------------------------------------
$(DIR_OBJ_TARGET) :
	mkdir -p $(DIR_OBJ_TARGET)

$(DIR_BIN_TARGET) :
	mkdir -p $(DIR_BIN_TARGET)

#----------------------------------------------------------------------
# Binary data file reader - more complex version
#----------------------------------------------------------------------
prd : $(DIR_BIN_TARGET)/preader

$(DIR_BIN_TARGET)/preader : \
	$(DIR_OBJ_TARGET)/mreader.o \
	$(DIR_OBJ_TARGET)/histo_int.o \
	$(DIR_OBJ_TARGET)/accu_array.o \
	$(DIR_OBJ_TARGET)/frame_scan.o \
	$(DIR_OBJ_TARGET)/frame.o
		$(LD) $(CFLAGS) \
			$(DIR_OBJ_TARGET)/mreader.o \
			$(DIR_OBJ_TARGET)/histo_int.o \
			$(DIR_OBJ_TARGET)/accu_array.o \
			$(DIR_OBJ_TARGET)/frame_scan.o \
			$(DIR_OBJ_TARGET)/frame.o \
			$(LLIBS) \
			$(LIBS) \
			-o $(DIR_BIN_TARGET)/preader

#----------------------------------------------------------------------
# Binary data file reader - ligther and clearer version
#----------------------------------------------------------------------
fdec : $(DIR_BIN_TARGET)/fdecoder

$(DIR_BIN_TARGET)/fdecoder: \
	$(DIR_OBJ_TARGET)/fdecoder.o \
	$(DIR_OBJ_TARGET)/datum_decoder.o
		$(LD) $(CFLAGS) \
			$(DIR_OBJ_TARGET)/fdecoder.o \
			$(DIR_OBJ_TARGET)/datum_decoder.o \
			$(LLIBS) \
			$(LIBS) \
			-o $(DIR_BIN_TARGET)/fdecoder

#----------------------------------------------------------------------
# Rules for building object files
#----------------------------------------------------------------------
$(DIR_OBJ_TARGET)/mclientUDP.o : \
	$(DIR_PROJ)/mclient/mclientUDP.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/mclientUDP.c \
			-o $(DIR_OBJ_TARGET)/mclientUDP.o

$(DIR_OBJ_TARGET)/femproxy.o : \
	$(DIR_PROJ)/mclient/femproxy.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/femproxy.c \
			-o $(DIR_OBJ_TARGET)/femproxy.o

$(DIR_OBJ_TARGET)/evbuilder.o : \
	$(DIR_PROJ)/mclient/evbuilder.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/evbuilder.c \
			-o $(DIR_OBJ_TARGET)/evbuilder.o

$(DIR_OBJ_TARGET)/femarray.o : \
	$(DIR_PROJ)/mclient/femarray.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/femarray.c \
			-o $(DIR_OBJ_TARGET)/femarray.o

$(DIR_OBJ_TARGET)/cmdfetcher.o : \
	$(DIR_PROJ)/mclient/cmdfetcher.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/cmdfetcher.c \
			-o $(DIR_OBJ_TARGET)/cmdfetcher.o

$(DIR_OBJ_TARGET)/programflash.o : \
	$(DIR_PROJ)/mclient/programflash.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/programflash.c \
			-o $(DIR_OBJ_TARGET)/programflash.o

$(DIR_OBJ_TARGET)/hexreader.o : \
	$(DIR_PROJ)/spiflash/hexreader.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/spiflash/hexreader.c \
			-o $(DIR_OBJ_TARGET)/hexreader.o

$(DIR_OBJ_TARGET)/bufpool.o : \
	$(DIR_PROJ)/bufmgr/bufpool.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/bufmgr/bufpool.c \
			-o $(DIR_OBJ_TARGET)/bufpool.o

$(DIR_OBJ_TARGET)/frame.o : \
	$(DIR_PROJ)/format/frame.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/format/frame.c \
			-o $(DIR_OBJ_TARGET)/frame.o

$(DIR_OBJ_TARGET)/remotecopy.o : \
	$(DIR_PROJ)/mclient/remotecopy.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/remotecopy.c \
			-o $(DIR_OBJ_TARGET)/remotecopy.o

$(DIR_OBJ_TARGET)/os_al.o : \
	$(DIR_PROJ)/platforms/$(VAR_OS)/os_al.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/platforms/$(VAR_OS)/os_al.c \
			-o $(DIR_OBJ_TARGET)/os_al.o

$(DIR_OBJ_TARGET)/mreader.o : \
	$(DIR_PROJ)/mclient/mreader.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/mreader.c \
			-o $(DIR_OBJ_TARGET)/mreader.o
			
$(DIR_OBJ_TARGET)/frame_scan.o : \
	$(DIR_PROJ)/mclient/frame_scan.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/frame_scan.c \
			-o $(DIR_OBJ_TARGET)/frame_scan.o
			
$(DIR_OBJ_TARGET)/histo_int.o : \
	$(DIR_PROJ)/util/histo_int.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/util/histo_int.c \
			-o $(DIR_OBJ_TARGET)/histo_int.o

$(DIR_OBJ_TARGET)/accu_array.o : \
	$(DIR_PROJ)/util/accu_array.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/util/accu_array.c \
			-o $(DIR_OBJ_TARGET)/accu_array.o

$(DIR_OBJ_TARGET)/datum_decoder.o : \
	$(DIR_PROJ)/format/datum_decoder.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/format/datum_decoder.c \
			-o $(DIR_OBJ_TARGET)/datum_decoder.o

$(DIR_OBJ_TARGET)/fdecoder.o : \
	$(DIR_PROJ)/mclient/fdecoder.c
		$(CC) $(CFLAGS) \
		$(INCLUDE) \
			-c $(DIR_PROJ)/mclient/fdecoder.c \
			-o $(DIR_OBJ_TARGET)/fdecoder.o

