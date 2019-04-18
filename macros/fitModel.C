// Fit model from Telegraph equations
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooExponential.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooDataHist.h"
#include "RooPolynomial.h"
#include "RooGaussian.h"
#include <iostream>
#include <cmath>

using namespace std;
void fit(string data_file, int order )
{

  TH1 *data = new TH1("data", "", 100, 0, 100);

  TFile *file = TFile::Open(("../data/ILC-TPC/"+data_file).c_str(), "READ");
  cout << "File open" << endl;
  TTree *datatree = (TTree*)file->Get("padData");
  TTree *geomtree = (TTree*)file->Get("femGeomTree");
  cout << "Trees loaded" << endl;





  RooRealVar t(“t”,”t”,0,100);
  RooDataHist datahist("hist", "hist", RooArgList(t), data);

  // Build model

  int nx = 50;
  double dX = 0.3;
  double dY = 0.3;

  RooRealVar *nPadX = new RooRealVar("nPadX", "nPadX", nx);
  RooRealVar *dx = new RooRealVar("dx", "dx", dX);
  RooFormulaVar *s = new RooFormulaVar("s", "nPadX*dx", RooArgList(*nPadX, *dx));

  nPadX->setConstant(true);
  dx->setConstant(true);

  for (int i = 0; i < nx; i++)
  double A = 0;
  { // Loop over pads on the concerned j line
    A += data[i][j]/(dX*dY) * sin(i*order*M_PI/(nx*dx))*dx;
  }
  A = 2*A/(nx*dX);



}
