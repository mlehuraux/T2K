//
// Colors
//
#include <vector>
#include "TPad.h"
#include "TCanvas.h"

void testColor()
{
  vector<TPad*> colorPad(50);

  TCanvas *c = new TCanvas("c", "c", 500, 500);

  double xmin = 0.01;
  double xmax = 0.99;
  double ymin = 0.01;
  double ymax = 0.99;
  double dx   = (xmax-xmin) / 10.;
  double dy   = (ymax-ymin) / 5.;

  for (int row=0; row<5; row++) {
    for ( int col=0; col<10; col++ ) {

      Int_t   index = col + row*10;
      Color_t color = index + 51;
      
      char pname[5];
      sprintf(pname, "p%02d", index);
      char cname[5];
      sprintf(cname, "c%02d", color);
        
      double x0 = xmin + col*dx;
      double y0 = ymin + row*dy;
      double x1 = x0 + dx;
      double y1 = y0 + dy;

      colorPad[index] = new TPad(pname, cname, x0,y0, x1,y1);
      colorPad[index]->SetFillColor(color);
      colorPad[index]->Draw();
    }
  }

  c->SetEditable(false);
  c->Modified();
  c->Update();
  c->Draw();
}
