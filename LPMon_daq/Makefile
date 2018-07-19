#ROOT librairies : needs root-config
include Makefile.arch

HDRS          = MMonSelectCanvasFrame.h mygblink.h MMonMainFrame.h KeyStonePad.h

SRCS          = MMonitoring.cc \
 MMonSelectCanvasFrame.cc OomonMainFrameDict.cc KeyStonePad.cc

OBJS          = MMonitoring.o \
 MMonSelectCanvasFrame.o OomonMainFrameDict.o KeyStonePad.o

PROGRAM       = LPDisplay

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
	@echo "Linking $(PROGRAM) ..."
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "$@ done"@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
	@echo "Yatta : compilation done ;-)"

clean:;         
	@rm -f $(OBJS) core
	@rm -f *dict.* core

.SUFFIXES: .$(SrcSuf)

###
OomonMainFrame.$(ObjSuf): MMonSelectCanvasFrame.h  mygblink.h MMonMainFrame.h KeyStonePad.h

OomonMainFrameDict.$(SrcSuf): MMonSelectCanvasFrame.h mygblink.h MMonMainFrame.h KeyStonePad.h Handlers.h LinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -g -c $<

#---------------------------------------------------
