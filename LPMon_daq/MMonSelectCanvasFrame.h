#ifndef __MMonSelectCanvasFrame_h__
#define __MMonSelectCanvasFrame_h__

#include "RQ_OBJECT.h"
#include "TGButton.h"

class MMonSelectCanvasFrame
{
  RQ_OBJECT("MMonSelectCanvasFrame")

private:
	TGCheckButton *fCheck[20];

public:

	MMonSelectCanvasFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
	virtual ~MMonSelectCanvasFrame();

	void HandleButton(Int_t id=-1);

	ClassDef(MMonSelectCanvasFrame,1)
};

#endif
