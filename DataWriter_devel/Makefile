#
# Module based Makefile: this Makefile compiles and links the files
# configured in subdirectories through the files */module.mk
#
# This Makefile was primarily written for the Atlas Spectrometer
# Alignment Program (ASAP), but can be used for many other compilation
# tasks.
#
# Some cleanup is pending, and some documentation should be
# written. Also some merging with the various flavours of this
# Makefile should be done.
#
#    This Makefile is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
# Author: P.F. Giraud
#

LIBRARYNAME := Soft
VERSION := 0.0
CXXSPECIFIC :=
F90SPECIFIC :=
ADDITIONAL_TARGETS :=
PROG_INSTALL :=
EXTRA :=
PACKAGENAME :=

all: lib prog additional

-include config.mk

ifndef PACKAGENAME
  PACKAGENAME := $(LIBRARYNAME)
endif

### global options
# Architecture
ARCH         := $(shell ./MTools/guess_architecture.sh)

# File suffixes
OBJSUFFIX  := o
DEPSUFFIX  := d
DLLSUFFIX  := so

# File directories
OBJDIR     := .o
DEPDIR     := .d
BINDIR     := ./bin
MODDIR     := .mod

# output directories
SOLIBDIR   := ./lib
SOLIBNAME  := lib$(LIBRARYNAME).$(DLLSUFFIX).$(VERSION)
SOLIBDNAME = lib$(LIBRARYNAME).$(DLLSUFFIX)
SOLIB      = $(SOLIBDIR)/$(SOLIBNAME)
SOLIBD     = $(SOLIBDIR)/$(SOLIBDNAME)

__dummy := $(shell mkdir -p $(OBJDIR) $(DEPDIR) $(BINDIR) $(SOLIBDIR) $(MODDIR) .tmp)

### compilator, linker, flags and options
CXX        := g++
LD         := g++
OUTPUTOPT  := -o
ifndef OPTFLAGS
  OPTFLAGS   := -g -O2 -Wall -pipe
endif
CXXFLAGS   := $(OPTFLAGS) $(CXXSPECIFIC) -D_GLIBCXX_USE_CXX11_ABI=1
LDFLAGS    := -shared -Wl,-soname,$(SOLIBNAME)
LIBS       :=
DEPGENF90  := ./MTools/depgen_f90.pl
GENMAKSH   := ./MTools/genmak.sh
ifndef OPT90FLAGS
  OPT90FLAGS := -g -O2 -pipe
endif
F90FLAGS   := $(OPT90FLAGS) $(F90SPECIFIC)
ADDDEPS :=
SYNC      := cp -a --parents

DIST = $(PACKAGENAME)-$(shell echo $(VERSION) | sed "s;\.;-;g" )



# Architecture dependant options
-include MTools/$(ARCH).mk


### processing of the modules
# find the modules to include and compile
MODULES    := $(subst /module.mk,,$(wildcard */module.mk))
MODULES    += $(subst /pfgct/module.mk,,$(wildcard */pfgct/module.mk))

__dummy := $(shell $(GENMAKSH) $(MODULES))


# the source files to compile (without suffix)
SRC        :=
PROG       :=
PROGSRC    :=
#VPATH      :=
INCLUDEFLAGS :=

-include gen.mk

# Make sure all the output subdirectories are there
TARGETDIRS := $(sort $(dir $(SRC) $(PROGSRC)))
ifneq ($(strip $(TARGETDIRS)),)
	__dummy := $(shell mkdir -p $(patsubst %,$(DEPDIR)/%,$(TARGETDIRS)))
	__dummy := $(shell mkdir -p $(patsubst %,$(OBJDIR)/%,$(TARGETDIRS)))
endif

# Mangle names of PROG_INSTALL
PROG_INSTALL := $(patsubst %, ./bin/%,$(PROG_INSTALL))

### build targets and include path for compilation
OBJS       := $(addprefix ${OBJDIR}/, $(addsuffix .$(OBJSUFFIX), $(SRC)))
DEPS       := $(addprefix ${DEPDIR}/, $(addsuffix .$(DEPSUFFIX), $(SRC)))
DEPS       += $(addprefix ${DEPDIR}/, $(addsuffix .$(DEPSUFFIX), $(PROGSRC)))
DEPS       += ${ADDDEPS}
CXXFLAGS   += -I. $(INCLUDEFLAGS)
F90FLAGS   += -I. $(INCLUDEFLAGS)

# F90 compiler
ifeq "$(NEEDS_F90)" "yes"
  TEST := $(shell which pgf90 2> /dev/null)
  ifdef TEST
    F90 := pgf90
  endif
  TEST := $(shell which g95 2> /dev/null)
  ifdef TEST
    F90 := g95
  endif
  TEST := $(shell which gfortran 2> /dev/null)
  ifdef TEST
    F90 := gfortran
  endif

  ifndef F90
    $(error "No FORTRAN compiler found, although one is requested")
  endif
endif


### External libraries
### Uncomment one of the following sections to link the corresponding libraries

PKGCONFIG = PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config

# ROOT
ifeq "$(NEEDS_ROOT)" "yes"
  TEST := $(shell which root-config 2> /dev/null)
  ifndef TEST
    $(error ROOT installation is not found)
  endif
  ROOTCFLAGS := $(shell root-config --cflags)
  ROOTGLIBS  := $(shell root-config --glibs) -lMinuit -lHtml -lEG -lThread $(ROOT_EXTRA_LIBS)
  ROOTCINT   := rootcint
  CXXFLAGS   += -DHAS_ROOT $(ROOTCFLAGS)
  LIBS       += $(ROOTGLIBS)

  ifeq "$(NEEDS_REFLEX)" "yes"
    TEST := $(shell which genreflex 2> /dev/null)
    ifndef TEST
      $(error REFLEX installation is not found)
    endif
    GENREFLEX  := genreflex
    LIBS       += -lReflex
  endif
endif

# libxml2
ifeq "$(NEEDS_LIBXML2)" "yes"
  CXXFLAGS   += $(shell xml2-config --cflags)
  LIBS       += $(shell xml2-config --libs)
endif

# java (if found on this machine)
ifeq "$(NEEDS_JAVA)" "yes"
  ifdef JAVA_HOME
    CXXFLAGS   += -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(ARCH)
  endif
endif

# corba (omniORB4)
ifeq "$(NEEDS_CORBA)" "yes"
  TEST := $(shell pkg-config --exists omniDynamic4 && echo 1)
  ifdef TEST
    CXXFLAGS +=  $(shell pkg-config --cflags omniDynamic4)
    LIBS += $(shell pkg-config --libs omniDynamic4)
  else
    $(error CORBA installation is not found)
  endif
endif

# FUSE (filesystem in userspace)
ifeq "$(NEEDS_FUSE)" "yes"
  TEST := $(shell pkg-config --exists fuse && echo 1)
  ifdef TEST
    CXXFLAGS +=  $(shell pkg-config --cflags fuse)
    LIBS += $(shell pkg-config --libs fuse)
  else
    $(error FUSE installation is not found)
  endif
endif

# CERNLIB
ifeq "$(NEEDS_CERNLIB)" "yes"
  ifndef CERNLIB
    CERNLIB    := $(shell MTools/mtcernlib graflib grafX11 kernlib packlib lapack3 mathlib blas)
  endif
  LIBS       += $(CERNLIB)
endif

# LAPACK
ifeq "$(NEEDS_LAPACK)" "yes"
  LIBS += -llapack
endif

# BOOST
ifeq "$(NEEDS_BOOST)" "yes"
  CXXFLAGS += $(BOOST_INCLUDES)
  LIBS += $(BOOST_LIBS)
endif

ifdef F90
-include MTools/$(F90).mk
endif

# QT4
ifeq "$(NEEDS_QT4)" "yes"
  HAS_QT4 := $(shell $(PKGCONFIG) --exists QtGui && echo 1)
  ifdef HAS_QT4
    CXXFLAGS += $(shell $(PKGCONFIG) --cflags QtGui QtCore QtTest)
# Workaround for bug 20241 in macports qt4-mac (qmake no longer includes /opt/local/lib as a library path)
    ifeq "$(shell $(PKGCONFIG) --variable=prefix QtGui)" "/opt/local/libexec/qt4-mac"
      LIBS += -L/opt/local/lib
    endif
    LIBS += $(shell $(PKGCONFIG) --libs QtGui QtCore QtTest)
    QTMOC := $(shell $(PKGCONFIG) --variable=moc_location QtGui)
# Workaround for invalid moc and uic locations in .pc files of qt4-devel package on SLC5
    TEST := $(shell which $(QTMOC) 2> /dev/null)
    ifndef TEST
        QTMOC := $(shell $(PKGCONFIG) --variable=prefix QtGui)/bin/moc
    endif
    QTRCC := $(shell $(PKGCONFIG) --variable=prefix QtGui)/bin/rcc
    QTUIC := $(shell $(PKGCONFIG) --variable=uic_location QtGui)
    TEST := $(shell which $(QTUIC) 2> /dev/null)
    ifndef TEST
        QTUIC := $(shell $(PKGCONFIG) --variable=prefix QtGui)/bin/uic
    endif
   else
    $(error The Qt4 framework was not found in the pkg-config search path. You may need to install Qt, or to add the path of the directory containing `QtGui.pc` to your PKG_CONFIG_PATH variable)
   endif
  TEST :=  $(shell $(PKGCONFIG) --exists QtSvg && echo 1)
  ifdef TEST
    CXXFLAGS += -DHAS_QTSVG $(shell $(PKGCONFIG) --cflags QtSvg)
    LIBS += $(shell $(PKGCONFIG) --libs QtSvg)
  endif
  TEST :=  $(shell $(PKGCONFIG) --exists QtOpenGL && echo 1)
  ifdef TEST
    CXXFLAGS += -DHAS_QTOpenGL $(shell $(PKGCONFIG) --cflags QtOpenGL)
    LIBS += $(shell $(PKGCONFIG) --libs QtOpenGL)
  endif
#   TEST :=  $(shell $(PKGCONFIG) --exists Qt3Support && echo 1)
#   ifdef TEST
#     CXXFLAGS += -DHAS_QT3SUPPORT $(shell $(PKGCONFIG) --cflags Qt3Support)
#     LIBS += $(shell $(PKGCONFIG) --libs Qt3Support)
#   endif
  TEST :=  $(shell $(PKGCONFIG) --exists QtXml && echo 1)
  ifdef TEST
    CXXFLAGS += -DHAS_QTXML $(shell $(PKGCONFIG) --cflags QtXml)
    LIBS += $(shell $(PKGCONFIG) --libs QtXml)
  endif
  TEST :=  $(shell $(PKGCONFIG) --exists QtNetwork && echo 1)
  ifdef TEST
    CXXFLAGS += -DHAS_QTNETWORK $(shell $(PKGCONFIG) --cflags QtNetwork)
    LIBS += $(shell $(PKGCONFIG) --libs QtNetwork)
  endif
endif

# QtROOT plugin for Qt4  (no failure if unavailable)
ifeq "$(NEEDS_QT4ROOT)" "yes"
  TEST := $(shell which root-config 2> /dev/null)
  ifdef TEST
    ROOTLIBDIR := $(shell root-config --libdir)
    TEST := $(shell test `find "$(ROOTLIBDIR)" -name libQtRoot.so 2> /dev/null | wc -l` -gt 0 && echo "yes")
    ifeq "$(TEST)" "yes"
      QTROOTCFLAGS := $(shell root-config --cflags)
      # Find path of QtROOT libraries w.r.t. path of ROOT libraries (see Debian bug #519941)
      QTROOTLIBDIR := $(shell find `root-config --libdir` -name libQtRoot.so -print0 | xargs -0 | cut -d' ' -f1 | xargs dirname)
      QTROOTLIBS   := $(shell root-config --glibs) -L$(QTROOTLIBDIR) -lQtRoot -lGQt
      # Hard-code path to QtROOT libraries if they are provided by the root-plugin-qt Debian package
      ifeq "$(QTROOTLIBDIR)" "/usr/lib/root/5.18"
          QTROOTLIBS := $(shell root-config --glibs) -Wl,-rpath -Wl,$(QTROOTLIBDIR) -L$(QTROOTLIBDIR) -lQtRoot -lGQt
      endif
      # Add flags for ROOT and QtROOT libraries
      CXXFLAGS   += -DHAS_QTROOT $(QTROOTCFLAGS) -DHAS_QT3SUPPORT $(shell $(PKGCONFIG) --cflags Qt3Support)
      LIBS += $(QTROOTLIBS) $(shell $(PKGCONFIG) --libs Qt3Support)
    endif
  endif
endif

# SOCI C++ Database Acces library (no failure if unavailable)
ifeq "$(NEEDS_SOCI)" "yes"
   ifdef ORACLE_LIBDIR
   ifdef ORACLE_INCDIR
   ifdef SOCI_HOME
      CXXFLAGS += -DHAS_SOCI=yes
      CXXFLAGS += -I$(SOCI_HOME)/include
      LIBS     += -L$(SOCI_HOME)/lib -lsoci_core-3.0.0 -lsoci_oracle-3.0.0
      CXXFLAGS += -I$(ORACLE_INCDIR)
      LIBS     += -L$(ORACLE_LIBDIR)
      CXXFLAGS += -DHAS_SOCI=yes
   endif
   endif
   endif
endif

# OpenGL core libraries
ifeq "$(NEEDS_OPENGL)" "yes"
  TEST := $(shell $(PKGCONFIG) --exists gl && echo yes)
  ifeq "$(TEST)" "yes"
    CXXFLAGS += $(shell $(PKGCONFIG) --cflags gl)
    LIBS += $(shell $(PKGCONFIG) --libs gl) -lGLU
  else
    LIBS += -lGL -lGLU
  endif
endif

# Freetype
ifeq "$(NEEDS_FREETYPE)" "yes"
  CXXFLAGS += $(shell freetype-config --cflags)
  LIBS += $(shell freetype-config --libs)
endif

# CLHEP
ifeq "$(NEEDS_CLHEP)" "yes"
  CXXFLAGS += $(shell clhep-config --include)
  LIBS += $(shell clhep-config --libs)
endif

### Rules
.PHONY: all clean distclean
.SUFFIXES: .ui _ui.h
.PRECIOUS: ${OBJDIR}/%.bin %SK.cc %.hh %_moc.cxx

lib: $(SOLIBD)

prog: $(PROG)

additional: $(ADDITIONAL_TARGETS)

# essential old-style UNIX fun (copied from Kevan's Makefile)
war:
	@echo "make: *** No rule to make target 'war'.  Stop.  Try 'love' instead."

# the main library rules
$(SOLIBD): $(SOLIB)
	@ln -sf $(SOLIBNAME) $(SOLIBD)

$(SOLIB): $(OBJS)
	@echo "Linking $^ to $@"
	@$(LD) $(LDFLAGS) $^ $(LIBS) $(FLIBS) $(OUTPUTOPT) $@
	@echo "Successfully completed $@"

-include dictgen.mk

# clean rule
clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJDIR) $(DEPDIR) $(BINDIR) $(SOLIBDIR) $(MODDIR) $(foreach i,$(MODULES),$(i)/dict_$(i).* $(i)/reflexdict_$(i).*) gen.mk dictgen.mk *.mod .tmp
	@rm -rf $(foreach i,$(MODULES),$(i)/*_ui.h) $(foreach i,$(MODULES),$(i)/*_moc.cxx) $(foreach i,$(MODULES),$(i)/*_qrc.cxx)
# empty distclean rule
distclean: clean ;

# dist rule (create a .tgz with all the relevant files)
dist: clean
	@rm -rf $(DIST) $(DIST).tgz
	@mkdir $(DIST)
	@for m in $(MODULES); do \
	  cp -a -L $${m} $(DIST)/$${m}; \
	  find $(DIST)/$${m} -name "*~" | xargs --no-run-if-empty rm -f; \
	  find $(DIST)/$${m} -name "semantic.cache" | xargs --no-run-if-empty rm -f; \
	  find $(DIST)/$${m} -type d -name "CVS" | xargs --no-run-if-empty rm -rf; \
	  find $(DIST)/$${m} -type d -name ".svn" | xargs --no-run-if-empty rm -rf; \
	done
	@cp -a -L MTools $(DIST)/
	@cp -a Makefile *.mk *.sh $(DIST)/
	@$(SYNC) $(EXTRA) $(DIST)/
	@tar -cvzf $(DIST).tgz $(DIST)
	@rm -rf $(DIST)/

dist-bootstrap:
	@rm -rf $(LIBRARYNAME) $(LIBRARYNAME).tgz
	@mkdir $(LIBRARYNAME)
	@for m in $(MODULES); do \
	  if [ ! -d $$m/CVS ]; then \
	    cp -a $${m} $(LIBRARYNAME)/$${m}; \
	    find $(LIBRARYNAME)/$${m} -name "*~" | xargs --no-run-if-empty rm -f; \
	    find $(LIBRARYNAME)/$${m} -name "semantic.cache" | xargs --no-run-if-empty rm -f; \
	    find $(LIBRARYNAME)/$${m} -type d -name "CVS" | xargs --no-run-if-empty rm -rf; \
	    find $(LIBRARYNAME)/$${m} -type d -name ".svn" | xargs --no-run-if-empty rm -rf; \
	  fi; \
	done
	@cp -a Makefile *.mk *.sh MTools $(LIBRARYNAME)/
	@tar -cvzf $(LIBRARYNAME).tgz $(LIBRARYNAME)
	@rm -rf $(LIBRARYNAME)/

install: all $(PROG_INSTALL)
ifndef PREFIX
	@echo "Error: Variable PREFIX is undefined"
	@exit 1
else
	@mkdir -p $(PREFIX)/lib/
	@cp -a $(SOLIB) $(PREFIX)/lib/$(SOLIBNAME)
ifdef PROG_INSTALL
	@mkdir -p $(PREFIX)/bin/
	@cp -a $(PROG_INSTALL) $(PREFIX)/bin
endif
ifdef EXTRA
	@mkdir -p $(PREFIX)/share/$(PACKAGENAME)/
	@$(SYNC) $(EXTRA) $(PREFIX)/share/$(PACKAGENAME)/
endif
endif

printincludes:
	@echo $(filter -I% -D%,$(CXXFLAGS))

######

# .o building rules
${OBJDIR}/%.$(OBJSUFFIX) : %.cxx ${DEPDIR}/%.$(DEPSUFFIX)
	@echo "Compiling $< to $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

${OBJDIR}/%.$(OBJSUFFIX) : %.cpp ${DEPDIR}/%.$(DEPSUFFIX)
	@echo "Compiling $< to $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

${OBJDIR}/%.$(OBJSUFFIX) : %.cc ${DEPDIR}/%.$(DEPSUFFIX)
	@echo "Compiling $< to $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

${OBJDIR}/%.$(OBJSUFFIX) : %.C ${DEPDIR}/%.$(DEPSUFFIX)
	@echo "Compiling $< to $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

ifdef F90
$(OBJDIR)/%.$(OBJSUFFIX) : %.f
	@echo "Compiling (Fortran 90) $< to $@"
	@$(F90) $(F90FLAGS) -J$(MODDIR) -I$(MODDIR) -c $< -o $@

$(OBJDIR)/%.$(OBJSUFFIX) : %.f90
	@echo "Compiling (Fortran 90) $< to $@"
	@$(F90) $(F90FLAGS) -J$(MODDIR) -I$(MODDIR) -c $< -o $@

$(OBJDIR)/%.$(OBJSUFFIX) : %.F90
	@echo "Compiling (Fortran 90) $< to $@"
	@$(F90) $(F90FLAGS) -J$(MODDIR) -I$(MODDIR) -c $< -o $@

$(OBJDIR)/%.$(OBJSUFFIX) : %.FF90
	@echo "Compiling (Fortran 90) $< to $@"
	@$(F90) -xf95-cpp-input -ffixed-form $(F90FLAGS) -J$(MODDIR) -I$(MODDIR) -c $< -o $@

$(OBJDIR)/%.$(OBJSUFFIX) : %.F
	@echo "Compiling (Fortran 90) $< to $@"
	@$(F90) $(F90FLAGS) -J$(MODDIR) -I$(MODDIR) -c $< -o $@
endif

# .d building rules
${DEPDIR}/%.$(DEPSUFFIX): %.cxx
	@echo "Making dependency file $@"
	@$(CXX) -MM $(CXXFLAGS) $< \
	| sed 's|\('`basename $*`'\)\.o[ :]*|'$(OBJDIR)'/'`dirname $*`'/\1.o '$(DEPDIR)'/'`dirname $*`'/\1.d : |g' \
	> $@

${DEPDIR}/%.$(DEPSUFFIX): %.cpp
	@echo "Making dependency file $@"
	@$(CXX) -MM $(CXXFLAGS) $< \
	| sed 's|\('`basename $*`'\)\.o[ :]*|'$(OBJDIR)'/'`dirname $*`'/\1.o '$(DEPDIR)'/'`dirname $*`'/\1.d : |g' \
	> $@

${DEPDIR}/%.$(DEPSUFFIX): %.cc
	@echo "Making dependency file $@"
	@$(CXX) -MM $(CXXFLAGS) $< \
	| sed 's|\('`basename $*`'\)\.o[ :]*|'$(OBJDIR)'/'`dirname $*`'/\1.o '$(DEPDIR)'/'`dirname $*`'/\1.d : |g' \
	> $@

${DEPDIR}/%.$(DEPSUFFIX): %.C
	@echo "Making dependency file $@"
	@$(CXX) -MM $(CXXFLAGS) $< \
	| sed 's|\('`basename $*`'\)\.o[ :]*|'$(OBJDIR)'/'`dirname $*`'/\1.o '$(DEPDIR)'/'`dirname $*`'/\1.d : |g' \
	> $@

ifdef F90
${DEPDIR}/%.$(DEPSUFFIX): %.f
	@echo "Making dependency file $@"
	@$(DEPGENF90) $(filter -I% -D%,$(F90FLAGS)) $< > $@

${DEPDIR}/%.$(DEPSUFFIX): %.f90
	@echo "Making dependency file $@"
	@$(DEPGENF90) $(filter -I% -D%,$(F90FLAGS)) $< > $@

${DEPDIR}/%.$(DEPSUFFIX): %.F90
	@echo "Making dependency file $@"
	@$(DEPGENF90) $(filter -I% -D%,$(F90FLAGS)) $< > $@

${DEPDIR}/%.$(DEPSUFFIX): %.FF90
	@echo "Making dependency file $@"
	@$(DEPGENF90) $(filter -I% -D%,$(F90FLAGS)) $< > $@

${DEPDIR}/%.$(DEPSUFFIX): %.F
	@echo "Making dependency file $@"
	@$(DEPGENF90) $(filter -I% -D%,$(F90FLAGS)) $< > $@
endif
${DEPDIR}/%.gendict.d: %.hh
	@echo "Making dependency file $@"
	@$(CXX) -MM $(CXXFLAGS) $< \
	| sed 's|\('`basename $*`'\)\.o[ :]*|'`dirname $*`'/\1.cc '$(DEPDIR)'/'`dirname $*`'/\1.gendict.d : |g' \
	> $@


# Rules for generating built sources form Qt special files
%_ui.h : %.ui
	@echo "Processing (Qt UIC) $< to $@"
	@$(QTUIC) $< -o $@
%_moc.cxx: %.h
	@echo "Processing (Qt MOC) $< to $@"
	@$(QTMOC) $(filter -I% -D%,$(CXXFLAGS)) -o $@ $<
%_moc.cxx: %.hh
	@echo "Processing (Qt MOC) $< to $@"
	@$(QTMOC) $(filter -I% -D%,$(CXXFLAGS)) -o $@ $<
%_qrc.cxx : %.qrc
	@echo "Processing (Qt RCC) $< to $@"
	@$(QTRCC) $< -o $@

# .bin building rules
${OBJDIR}/%.bin: ${OBJDIR}/%.$(OBJSUFFIX) | $(SOLIBD)
	$(CXX) $(CXXFLAGS) $< -L$(SOLIBDIR) -l$(LIBRARYNAME) $(LIBS) $(FLIBS) -o $@

ifeq (,$(filter clean dist,$(MAKECMDGOALS)))
-include $(DEPS)
endif
