#ifndef Globals_h
#define Globals_h

#include "fdecoder.h"
#include "datum_decoder.h"
#include "platform_spec.h"
#include "frame.h"

#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <vector>

typedef struct _Param {
	char inp_dir[120];
	char inp_file[120];
	FILE *fsrc;
	int  has_no_run;
	int  show_run;
	unsigned int vflag;
	int  sample_index_offset_zs;
} Param;
/*
static Param param;
static Features fea;
static DatumContext dc;
static int verbose;
static TH1D *hADCvsTIME[n::pads];
static TH2D *pads;// = new TH2D("pads", "", geom::nPadx, 0, geom::nPadx, geom::nPady, 0, geom::nPady);
static std::vector<int> eventPos;
static int iEvent;
*/
#endif
