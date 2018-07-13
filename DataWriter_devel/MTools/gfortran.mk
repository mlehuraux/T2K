# F90 flags
F90FLAGS   += -fno-second-underscore

# F90 libraries
ifneq "$(ARCH)" "win32"
	F90LIBS    := $(shell gfortran $(F90FLAGS) -print-file-name=libgfortran.${DLLSUFFIX})
else
	F90LIBS    := $(shell gfortran $(F90FLAGS) -print-file-name=libgfortran.a)
	F90LIBS    += $(shell gfortran $(F90FLAGS) -print-libgcc-file-name)
endif

# G77 libraries
###### G77LIBS    := $(shell g77 $(F90FLAGS) -print-file-name=libg2c.${DLLSUFFIX})

FLIBS      := $(F90LIBS) $(G77LIBS)
