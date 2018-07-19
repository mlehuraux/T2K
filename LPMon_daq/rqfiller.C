//   rqfiller example.
//
//    - Two histograms (1-dim and 2-dim ) can be filled by hand
//      - click left button or move mouse with button pressed to
//        fill histos.
//      - click right button to reset the histogram.
//
//   This example demonstrates:
//
//   - how object communication mechanism can be used for handling
//     pad's mouse/key events by interpreted class.
//
//*******************************************************************
//
//  1. Run ROOT
//  2. Type .L rqfiller.C  (or rqfiller.C++)
//  3. Type rqfiller()
//
//*******************************************************************

#include <TROOT.h>
#include <TPad.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <RQ_OBJECT.h>


/////////////////////////////////////////////////////////////////////
class HFiller {

RQ_OBJECT("HFiller")  // for classes not derived from TQObject
                      // RQ_OBJECT macro should be used
private:
   TH1   *fHist;   // histogram to fill
   TPad  *fPad;    // pad in which histogram is drawn

public:
   HFiller() { fHist = 0; fPad = 0; }
   ~HFiller() { }
   TH1  *GetHist() const { return fHist; }
   void  SetHist(TH1 *hist);
   TPad *GetPad() const { return fPad; }
   void  SetPad(TPad *pad);
   void  ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel);
   void  Draw(Option_t *opt = "") { fHist->AppendPad(opt); }
};

//___________________________________________________________________
void HFiller::SetHist(TH1 *hist)
{
   // Sets new hist for manual filling.

   if (!hist || (hist == fHist)) return;
   fHist = hist;
}

//___________________________________________________________________
void HFiller::SetPad(TPad *pad)
{
   // Sets new pad containing histogram.

   if (!pad || (pad == fPad)) return;
   fPad = pad;
}

//___________________________________________________________________
void HFiller::ExecuteEvent(Int_t event, Int_t px, Int_t py, TObject *sel)
{
   // Actions in reponse to mouse button events.

   TCanvas *c = (TCanvas *) gTQSender;
   TPad *pad = (TPad *) c->GetSelectedPad();
   if (!pad || !fHist) return;
   if (pad != fPad) return;

   if ((event == kButton1Down) ||
       (event == kButton1Double) ||
      ((event == kButton1Motion) && (sel == fHist))) {

      Float_t x = pad->AbsPixeltoX(px);
      Float_t y = pad->AbsPixeltoY(py);
      x = pad->PadtoX(x);
      y = pad->PadtoY(y);

      if (fHist->InheritsFrom(TH2::Class()))
         fHist->Fill(x,y);
      else
         fHist->Fill(x);
      
      pad->Modified();
      //if (event == kButton1Motion) pad->Update(); //force update
      // pad->Modified() --> SIGNAL 
      //                 --> Q: who will catch this signal ?
      //                        when and where ?
      // pad->Update()   --> Repaint all modified pad's


   }

   if (event == kButton3Down) {
      fHist->Reset();
      pad->Modified();
      pad->Update();
   }
}

/////////////////////// functions ///////////////////////////////////
//___________________________________________________________________
void printEvent(Int_t event, Int_t px, Int_t py, TObject *)
{
   //  print event type and current cursor position

   TCanvas *c = (TCanvas *) gTQSender;
   TPad *pad = (TPad *) c->GetSelectedPad();
   if (!pad) return;
   printf("event=%d, px=%d, py=%d, ", event, px, py);
   Float_t x = pad->AbsPixeltoX(px);
   Float_t y = pad->AbsPixeltoY(py);
   x = pad->PadtoX(x);
   y = pad->PadtoY(y);
   printf("x=%.3g, y=%.3g\n",x,y);
}


//___________________________________________________________________
void rqfiller()
{
   // simple test of Pad Event Dispatcher

   TCanvas *c1 = new TCanvas("c1", "Test of Canvas Event Signal",
                              900, 500);

   TPad *p1 = new TPad("p1","",0.01,0.05,0.49,0.95);
   TPad *p2 = new TPad("p2","",0.51,0.05,0.99,0.95);

   p1->Draw();
   p2->Draw();

   ////// create 1-dim histogram filler

   p1->cd();

   HFiller *h1 = new HFiller();

   TH1F *hist1 = new TH1F("test1", "1-Dim hist filled manually",
                          20, 1, 100);
   h1->SetPad(p1);
   h1->SetHist(hist1);
   h1->GetHist()->SetFillColor(16);
   h1->Draw();

   // connect signal to histogam filling method
   //
   // YHS: It is the TCanvas who handles the mouse event and emits signals !!!
   //      It does not have info which graphical object is under the mouse.
   //      Other graphic object such as TPad has no ability to handle mouse
   //        event !!!
   //
   c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "HFiller",
               h1, "ExecuteEvent(Int_t,Int_t,Int_t,TObject*)");

   ////// create 2-dim histogram filler

   p2->cd();

   HFiller *h2 = new HFiller();

   TH2F *hist2 = new TH2F("test2", "2-Dim hist filled manually",
                          20, 1, 100, 20, 1, 100);
   h2->SetPad(p2);
   h2->SetHist(hist2);
   h2->Draw("col");

   // connect signal to histogam filling method
   c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "HFiller",
               h2, "ExecuteEvent(Int_t,Int_t,Int_t,TObject*)");

   // print mouse coordinates
   c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", 0,
               0, "printEvent(Int_t,Int_t,Int_t,TObject*)");

   p1->cd();
}

