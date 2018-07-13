LIBRARYNAME := DataWriter


ifeq "$(shell uname -n | head -c6)" "lxplus"
  BOOST = /afs/cern.ch/cms/slc5_amd64_gcc434/external/boost/1.47.0
  BOOST_INCLUDES :=
  BOOST_LIBS :=  -lboost_program_options
else
  BOOST_INCLUDES := -I $(BOOST_LIB)/include/
  BOOST_LIBS := -L$(BOOST_LIB)/lib/ -lboost_program_options -lboost_filesystem -lboost_system
endif

