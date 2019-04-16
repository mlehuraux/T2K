#ROOT librairies : needs root-config
include Makefile.arch

HDRS          = MMonSelectCanvasFrame.h mygblink.h MMonMainFrame.h T2KPadPanel.h

SRCS          = T2KMon.cc \
 MMonSelectCanvasFrame.cc OomonMainFrameDict.cc T2KPadPanel.cc

OBJS          = T2KMon.o \
 MMonSelectCanvasFrame.o OomonMainFrameDict.o T2KPadPanel.o

PROGRAM       = T2KMon

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
	@echo "Linking $(PROGRAM) ..."
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "$@ done"@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
	@echo " Compilation complete!"

clean:;
	@rm -f $(OBJS) core
	@rm -f *dict.* core
	@rm -f OomonMainFrameDict.cpp

.SUFFIXES: .$(SrcSuf)

###
OomonMainFrame.$(ObjSuf): MMonSelectCanvasFrame.h  mygblink.h MMonMainFrame.h T2KPadPanel.h

OomonMainFrameDict.$(SrcSuf): MMonSelectCanvasFrame.h mygblink.h MMonMainFrame.h T2KPadPanel.h Handlers.h LinkDef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -g -c $<

#---------------------------------------------------
