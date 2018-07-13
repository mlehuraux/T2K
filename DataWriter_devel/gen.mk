
# module.mk appends to FILES and DICTFILES
FILES :=
DICTFILES :=
REFLEXFILES :=
USEWILDCARD := 
PROGRAMS :=

THISMODULE := src

# including the module.mk file
-include $(patsubst %, %/module.mk,src)
-include $(patsubst %, %/pfgct/module.mk,src)

ifdef USEWILDCARD
  FILES := $(patsubst src/%,%,$(basename $(wildcard $(patsubst %,src/%,$(FILES)))))
endif

# appending the values found in FILES to the variable SRC
SRC += $(patsubst %,src/%,$(FILES))

# all subdirectories for this given set of FILES
SUBDIRS := $(sort src $(dir $(patsubst %,src/%,$(FILES))))

# put these subdirectories in the INCLUDEFLAGS
INCLUDEFLAGS += $(patsubst %, -I%, $(SUBDIRS))
#INCLUDEFLAGS += $(patsubst %, -I$(MODDIR)/%, $(SUBDIRS))

# appending the values found in DICTFILES to DICTH_modulename
DICTH_src := $(foreach i, $(patsubst %,src/%,$(DICTFILES)), $(wildcard $(i).h) $(wildcard $(i).hh) $(wildcard $(i)))
# if dict header files exist, append to variable SRC
ifneq ($(DICTH_src),)
SRC += src/dict_src
endif

# appending the values found in REFLEXFILES to REFLEXH_modulename
REFLEXH_src := $(foreach i, $(patsubst %,src/%,$(REFLEXFILES)), $(wildcard $(i).h) $(wildcard $(i).hh) $(wildcard $(i)))
SELECTIONXML_src := $(filter %selection.xml,$(REFLEXH_src))
REFLEXH_src := $(filter-out %selection.xml,$(REFLEXH_src))
# if reflex header files exist, append to variable SRC
ifneq ($(REFLEXH_src),)
ifeq ($(SELECTIONXML_src),)
$(error No selection.xml file specified in the REFLEXFILES of module src)
endif
SRC += src/reflexdict_src
ADDDEPS += ${DEPDIR}/src/reflexdict_src.gendict.d
endif

PROG += $(patsubst %,$(BINDIR)/%,$(notdir $(PROGRAMS)))

# appending the values found in FILES to the variable SRC
PROGSRC += $(patsubst %,src/%,$(PROGRAMS))

src_PROGDIRS := $(sort $(dir $(PROGRAMS)))

# a couple of rules to copy executable files correctly
$(BINDIR)/%: ${OBJDIR}/src/%.bin
	cp $^ $@

$(BINDIR)/%: src/bin/%
	cp $^ $@

define src_PROGRAM_template
$(BINDIR)/%: ${OBJDIR}/src/$(1)/%.bin
	cp $$^ $$@
endef

$(foreach sd,$(src_PROGDIRS),$(eval $(call src_PROGRAM_template,$(sd))))

src/%SK.cc src/%.hh: src/idl/%.idl
	omniidl $(OMNICORBAFLAGS) -Csrc -bcxx $^


# module.mk appends to FILES and DICTFILES
FILES :=
DICTFILES :=
REFLEXFILES :=
USEWILDCARD := 
PROGRAMS :=

THISMODULE := interface

# including the module.mk file
-include $(patsubst %, %/module.mk,interface)
-include $(patsubst %, %/pfgct/module.mk,interface)

ifdef USEWILDCARD
  FILES := $(patsubst interface/%,%,$(basename $(wildcard $(patsubst %,interface/%,$(FILES)))))
endif

# appending the values found in FILES to the variable SRC
SRC += $(patsubst %,interface/%,$(FILES))

# all subdirectories for this given set of FILES
SUBDIRS := $(sort interface $(dir $(patsubst %,interface/%,$(FILES))))

# put these subdirectories in the INCLUDEFLAGS
INCLUDEFLAGS += $(patsubst %, -I%, $(SUBDIRS))
#INCLUDEFLAGS += $(patsubst %, -I$(MODDIR)/%, $(SUBDIRS))

# appending the values found in DICTFILES to DICTH_modulename
DICTH_interface := $(foreach i, $(patsubst %,interface/%,$(DICTFILES)), $(wildcard $(i).h) $(wildcard $(i).hh) $(wildcard $(i)))
# if dict header files exist, append to variable SRC
ifneq ($(DICTH_interface),)
SRC += interface/dict_interface
endif

# appending the values found in REFLEXFILES to REFLEXH_modulename
REFLEXH_interface := $(foreach i, $(patsubst %,interface/%,$(REFLEXFILES)), $(wildcard $(i).h) $(wildcard $(i).hh) $(wildcard $(i)))
SELECTIONXML_interface := $(filter %selection.xml,$(REFLEXH_interface))
REFLEXH_interface := $(filter-out %selection.xml,$(REFLEXH_interface))
# if reflex header files exist, append to variable SRC
ifneq ($(REFLEXH_interface),)
ifeq ($(SELECTIONXML_interface),)
$(error No selection.xml file specified in the REFLEXFILES of module interface)
endif
SRC += interface/reflexdict_interface
ADDDEPS += ${DEPDIR}/interface/reflexdict_interface.gendict.d
endif

PROG += $(patsubst %,$(BINDIR)/%,$(notdir $(PROGRAMS)))

# appending the values found in FILES to the variable SRC
PROGSRC += $(patsubst %,interface/%,$(PROGRAMS))

interface_PROGDIRS := $(sort $(dir $(PROGRAMS)))

# a couple of rules to copy executable files correctly
$(BINDIR)/%: ${OBJDIR}/interface/%.bin
	cp $^ $@

$(BINDIR)/%: interface/bin/%
	cp $^ $@

define interface_PROGRAM_template
$(BINDIR)/%: ${OBJDIR}/interface/$(1)/%.bin
	cp $$^ $$@
endef

$(foreach sd,$(interface_PROGDIRS),$(eval $(call interface_PROGRAM_template,$(sd))))

interface/%SK.cc interface/%.hh: interface/idl/%.idl
	omniidl $(OMNICORBAFLAGS) -Cinterface -bcxx $^

