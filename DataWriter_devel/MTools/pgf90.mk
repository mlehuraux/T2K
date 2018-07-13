# F90 flags
F90FLAGS   += -Mnosecond_underscore -g77libs

PGI := $(shell which pgf90 | xargs dirname | xargs dirname)
TEST := $(wildcard $(PGI)/lib/libpgf90.a)
ifndef TEST
	$(error Could not find libpgf90.a)
endif

# F90 libraries
F90LIBS    := $(PGI)/lib/libpgf90.a $(PGI)/lib/libpgf90_rpm1.a $(PGI)/lib/libpgf902.a
F90LIBS    += $(PGI)/lib/libpgf90rtl.a $(PGI)/lib/libpgftnrtl.a $(PGI)/lib/libnspgc.a $(PGI)/lib/libpgc.a

# G77 libraries
G77LIBS    := $(shell g77 -print-file-name=libg2c.a)

FLIBS      := $(F90LIBS) $(G77LIBS)
