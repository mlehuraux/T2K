# F90 flags
F90FLAGS   += -fno-second-underscore

# F90 libraries
F90LIBS    := $(shell g95 -print-file-name=libf95.a)
F90LIBS    += $(shell g95 -print-file-name=libgcc.a)

# G77 libraries
G77LIBS    := $(shell g77 -print-file-name=libg2c.a)

FLIBS      := $(F90LIBS) $(G77LIBS)
