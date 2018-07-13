# macosx:
#   to build the .so, use "make"
#   to build the .dylib, use "make DLLSUFFIX=dylib"

MACOSX_MINOR  := $(shell sw_vers -productVersion | cut -d'.' -f2)
PLATFORM      := macosx
DLLSUFFIX     := dylib

# compiler
#CXX           := c++
CXXFLAGS      := -O -pipe -Wall -W -Woverloaded-virtual

# linker
ifeq ($(MACOSX_MINOR),3)
   UNDEFOPT   := dynamic_lookup
   LD         := MACOSX_DEPLOYMENT_TARGET=10.3 c++
else
   UNDEFOPT   := suppress
   LD         := c++
endif


ifeq ($(DLLSUFFIX),dylib)
   SOLIBNAME  := lib$(LIBRARYNAME).$(VERSION).dylib
   LDFLAGS    := -dynamiclib -flat_namespace -undefined $(UNDEFOPT) -current_version $(VERSION)
   ADDITIONAL_TARGETS := $(SOLIBDIR)/lib$(LIBRARYNAME).so

$(SOLIBDIR)/lib$(LIBRARYNAME).so: $(SOLIBDIR)/lib$(LIBRARYNAME).dylib
	@ln -s lib$(LIBRARYNAME).dylib $(SOLIBDIR)/lib$(LIBRARYNAME).so

else
   LDFLAGS    := -bundle -undefined $(UNDEFOPT) -O -Xlinker -bind_at_load -flat_namespace
endif

# Workaround for 'cp' not providing the '--parents' option on Mac OS X
SYNC := rsync -R
