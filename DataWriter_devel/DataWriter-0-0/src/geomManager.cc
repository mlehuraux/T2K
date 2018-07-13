#include "interface/geomManager.hh"

#include <TH2.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

femGeomManager::femGeomManager(void) { _elecSetupDone = false; _geomSetupDone = false; }
femGeomManager::~femGeomManager(void) { }

femGeomManager::femGeomManager( const femGeomManager & geo ) {
  _elecSetupDone = geo._elecSetupDone;
  _geomSetupDone = geo._geomSetupDone;

  _iPadMin = geo._iPadMin;  _jPadMin = geo._jPadMin;
  _iPadMax = geo._iPadMax;  _jPadMax = geo._jPadMax;
  
  
  _iPad  = geo._iPad;
  _jPad  = geo._jPad;
  _iAsic = geo._iAsic;
  _iFec  = geo._iFec;
  _iPadInAsic  = geo._iPadInAsic;

  _x0Pad = geo._x0Pad;   _x1Pad = geo._x1Pad;   _x2Pad = geo._x2Pad;   _x3Pad = geo._x3Pad;
  _y0Pad = geo._y0Pad;   _y1Pad = geo._y1Pad;   _y2Pad = geo._y2Pad;   _y3Pad = geo._y3Pad;

}


femGeomManager& femGeomManager::operator=( const femGeomManager & geo){
  _elecSetupDone = geo._elecSetupDone;
  _geomSetupDone = geo._geomSetupDone;

  _iPadMin = geo._iPadMin;  _jPadMin = geo._jPadMin;
  _iPadMax = geo._iPadMax;  _jPadMax = geo._jPadMax;
  
  
  _iPad  = geo._iPad;
  _jPad  = geo._jPad;
  _iAsic = geo._iAsic;
  _iFec  = geo._iFec;
  _iPadInAsic  = geo._iPadInAsic;

  _x0Pad = geo._x0Pad;   _x1Pad = geo._x1Pad;   _x2Pad = geo._x2Pad;   _x3Pad = geo._x3Pad;
  _y0Pad = geo._y0Pad;   _y1Pad = geo._y1Pad;   _y2Pad = geo._y2Pad;   _y3Pad = geo._y3Pad;

  return *this;
}


void femGeomManager::readElectronic() {
  _iPadMin = -1;
  _iPadMax = -1;
  _jPadMin = -1;
  _jPadMax = -1;

  int kPhysChanPerAsic = 72;

 /// tmp variables
  map<int,int> iPad, jPad;
  map<int,int> iAsic,iFec,iPadInAsic;

  // get mapping
  int iASIC=0;
  int iFEC=0;

  ifstream input(_elecMap.c_str());
  while ( input.good() && !input.eof() ) {
    string line;
    getline(input,line,'\n');

    istringstream parse(line);
    int asic_fec (-1), row(-1), column(-1), pad(-1);   
    int ipad(-1);

    parse >> asic_fec >> row >> column >> pad;
    //    cout << " asic: " << asic_fec  << " row: " << row << " column: " << column << " pad: " << pad << endl;
    if( asic_fec == -1 || row == -1 || column == -1 ) continue;
    
    
    if      (column>=60) iFEC=5;
    else if (column>=48) iFEC=4;
    else if (column>=36) iFEC=3;
    else if (column>=24) iFEC=2;
    else if (column>=12) iFEC=1;
    else                 iFEC=0;
    
    // Comment by YHS
    //
    // The Map : (Row, Col) --> PhysChan
    //
    // NOTE: PhysChan is the most important one,
    //       which is the one that we can get from the ACQ data file, and
    //       which should be used to find (row, col) of the pad from it !
    //
    //
    //
    // NOTE : 2 pads denoted by -1 are being used for HV.
    //        This results in two missing channel numbers : 72 and 73 !!!
    //
    iASIC = asic_fec + 4*iFEC;
    
    if  ( pad != -1 ) {
      ipad = pad + kPhysChanPerAsic*iASIC;
    }
    else {
//       stringstream iss;
//       iss << "  Pad at row,col: " << setw(2) << row << "," << setw(2) << column
//           << " is for HV :  ";

//       // NOTE: Modify this when mapping is being changed !
//       //
// //      if ( column == 65 ) iPAD[i] = 72;
// //      if ( column == 64 ) iPAD[i] = 73;

//       iss << "Channel ID is assinged manually to " << iPAD[i];
      //  missingChannelInfo.push_back( iss.str() );
    }

    if( _iPadMin == -1 ) _iPadMin = row;
    if( _iPadMax == -1 ) _iPadMax = row;
    if( _jPadMin == -1 ) _jPadMin = column;
    if( _jPadMax == -1 ) _jPadMax = column;

    if( row < _iPadMin ) _iPadMin = row;
    if( row > _iPadMax ) _iPadMax = row;

    if( column < _jPadMin ) _jPadMin = column;
    if( column > _jPadMax ) _jPadMax = column;

    //    int kMaxCols = 72;
    iPad.insert(  make_pair(ipad,row)      );
    jPad.insert(  make_pair(ipad,column)   );
    iAsic.insert( make_pair(ipad,iASIC)    );
    iFec .insert( make_pair(ipad,iFEC)     );
    iPadInAsic.insert( make_pair(ipad,pad) );


    //    _chPad.insert( make_pair(ipad,column + row * kMaxCols)   );
  }
  
  /// then copy the ordered map to a vector to make the access faster...
  _iPad.resize(  iPad.size() );
  _jPad.resize(  jPad.size() );
  _iAsic.resize( iAsic.size() );
  _iFec.resize(  iFec.size() );
  _iPadInAsic.resize(  iPadInAsic.size() );

  map<int,int>::const_iterator it;
  for( it = iPad.begin() ; it != iPad.end() ; ++it ) _iPad [it->first] = it->second;
  for( it = jPad.begin() ; it != jPad.end() ; ++it ) _jPad [it->first] = it->second;
  for( it = iAsic.begin(); it != iAsic.end(); ++it ) _iAsic[it->first] = it->second;
  for( it = iFec.begin() ; it != iFec.end() ; ++it ) _iFec [it->first] = it->second;
  for( it = iPadInAsic.begin() ; it != iPadInAsic.end() ; ++it ) _iPadInAsic[it->first] = it->second;


  input.close();

  _elecSetupDone = true;
  cout << " ====> Added " << _iPad.size() << " channels" << endl;

}



int femGeomManager::iPad( int iPhysCh ) const {
  if( iPhysCh >= 0 && iPhysCh < int(_iPad.size()) ) return _iPad[iPhysCh];
  return -1;
}

int femGeomManager::jPad( int iPhysCh ) const {
  if( iPhysCh >= 0 && iPhysCh < int(_iPad.size()) ) return _jPad[iPhysCh];
  return -1;
}


int femGeomManager::asic( int iPhysCh ) const {
  if( iPhysCh >= 0 && iPhysCh < int(_iPad.size()) ) return _iAsic[iPhysCh];
  return -1;
}

int femGeomManager::fec( int iPhysCh ) const {
  if( iPhysCh >= 0 && iPhysCh < int(_iPad.size()) ) return _iFec[iPhysCh];
  return -1;
}

int femGeomManager::padInAsic( int iPhysCh ) const {
  if( iPhysCh >= 0 && iPhysCh < int(_iPad.size()) ) return _iPadInAsic[iPhysCh];
  return -1;
}





////// Geometry file
double femGeomManager::xPad(int i, int j) const {
  return 0.25 *( _x0Pad[i][j] + _x1Pad[i][j] + _x2Pad[i][j] + _x3Pad[i][j] );
}

////// Geometry file
double femGeomManager::yPad(int i, int j) const {
  return 0.25 *( _y0Pad[i][j] + _y1Pad[i][j] + _y2Pad[i][j] + _y3Pad[i][j] );
}

int femGeomManager::readGeometry() {

  if(  !_elecSetupDone ) {
    cerr << "ERROR femGeomManager::readGeomtry(...): need to decode the electronic map before decoding the geometry! bailing out..." << endl;
    return -1;
  }
  ifstream padGeomFile(_geomMap.c_str());

  if ( !padGeomFile ) {
    cerr << "ERROR femGeomManager::readGeomtry(...): Can't open file : " << _geomMap << endl;
    return -1;
  }

  char   varName[20];
  double width;
  double ymin, ymax;
  
  padGeomFile >> varName >> width;  // half width of the pad panel
  padGeomFile >> varName >> ymin;
  padGeomFile >> varName >> ymax;

  // tpcPanelWidth  = 2. * width;         // total width  of the pad array
  // tpcPanelHeight = ymax - ymin;        // total height of the pad array

  cout << " ===== Decoding geometry ======== " << endl;
  cout << "FEM Pad Panel: w , ymin, ymax = " 
       << width << ", " << ymin << ", " << ymax << endl;

  unsigned nRow = _iPadMax - _iPadMin + 1;
  unsigned nCol = _jPadMax - _jPadMin + 1;
  vector<vector<double> > x0_v(nRow),y0_v(nRow);
  vector<vector<double> > x1_v(nRow),y1_v(nRow);
  vector<vector<double> > x2_v(nRow),y2_v(nRow);
  vector<vector<double> > x3_v(nRow),y3_v(nRow);
  
  for( unsigned ix = 0 ; ix < x0_v.size(); ix++ ) {
    x0_v[ix].resize(nCol,-1.);   y0_v[ix].resize(nCol,-1.);
    x1_v[ix].resize(nCol,-1.);   y1_v[ix].resize(nCol,-1.);
    x2_v[ix].resize(nCol,-1.);   y2_v[ix].resize(nCol,-1.);
    x3_v[ix].resize(nCol,-1.);   y3_v[ix].resize(nCol,-1.);
  }
 
  while ( padGeomFile.good() && !padGeomFile.eof() ) {
    int row, col;
    double x0, y0, x1, y1, x2, y2, x3, y3;
    padGeomFile >> row >> col 
                >> x0 >> y0 >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
    
    x0_v[row][col] = x0;  y0_v[row][col] = y0;
    x1_v[row][col] = x1;  y1_v[row][col] = y1;
    x2_v[row][col] = x2;  y2_v[row][col] = y2;
    x3_v[row][col] = x3;  y3_v[row][col] = y3;

  }

  padGeomFile.close();

  _x0Pad = x0_v;  _y0Pad = y0_v;
  _x1Pad = x1_v;  _y1Pad = y1_v;
  _x2Pad = x2_v;  _y2Pad = y2_v;
  _x3Pad = x3_v;  _y3Pad = y3_v;

  //  setGeo2DVisual();

  _geomSetupDone = true;
  return 0;
}
double femGeomManager::xPadCoord( int i, int j, int k ) const {

  if( k == 0 ) return _x0Pad[i][j];
  if( k == 1 ) return _x1Pad[i][j];
  if( k == 2 ) return _x2Pad[i][j];
  if( k == 3 ) return _x3Pad[i][j];

  return -1;
}

double femGeomManager::yPadCoord( int i, int j, int k) const {

  if( k == 0 ) return _y0Pad[i][j];
  if( k == 1 ) return _y1Pad[i][j];
  if( k == 2 ) return _y2Pad[i][j];
  if( k == 3 ) return _y3Pad[i][j];

  return -1;
}


#include <TFile.h>
#include <TTree.h>
void femGeomManager::dumpToRootFile(TFile *f, const std::string &treeName ) {
  if( f == 0 || !f->IsOpen() || f->IsZombie() ) {
    cerr << " ERROR[ femGeomManager::dumpToRootFile ] output root file is not opened properly... " << endl;
    return;
  }
  
  f->cd();
  TTree *t = new TTree(treeName.c_str(),"fem geometrye" );

  t->Branch("iPad" , &_iPad  );
  t->Branch("jPad" , &_jPad  );
  t->Branch("iAsic", &_iAsic );
  t->Branch("iFec" , &_iFec  );
  t->Branch("iPadInAsic" , &_iPadInAsic  );
  t->Branch("x0Pad" , &_x0Pad  );
  t->Branch("x1Pad" , &_x1Pad  );
  t->Branch("x2Pad" , &_x2Pad  );
  t->Branch("x3Pad" , &_x3Pad  );
  t->Branch("y0Pad" , &_y0Pad  );
  t->Branch("y1Pad" , &_y1Pad  );
  t->Branch("y2Pad" , &_y2Pad  );
  t->Branch("y3Pad" , &_y3Pad  );
  t->Branch("iPadMin", &_iPadMin );
  t->Branch("jPadMin", &_jPadMin );
  t->Branch("iPadMax", &_iPadMax );
  t->Branch("jPadMax", &_jPadMax );
  t->Fill();

  t->Write(treeName.c_str(),TObject::kOverwrite);
}


void femGeomManager::initFromRootFile(std::string rootFileName, const std::string &treeName ) {
  TFile f(rootFileName.c_str(),"read");
  if( !f.IsOpen() || f.IsZombie() ) {
    cerr << " ERROR[ femGeomManager::initFromRootFile ] input root file is not opened properly... " << endl;
    return;
  }
  
  f.cd();
  TTree *t = (TTree*) f.Get(treeName.c_str());
  vector<int> *tmp_iPad(0), *tmp_jPad(0), *tmp_iAsic(0), *tmp_iFec(0), *tmp_iPadInAsic(0);
  std::vector<std::vector<double> > *tmp_x0Pad(0), *tmp_x1Pad(0), *tmp_x2Pad(0), *tmp_x3Pad(0);
  std::vector<std::vector<double> > *tmp_y0Pad(0), *tmp_y1Pad(0), *tmp_y2Pad(0), *tmp_y3Pad(0);
  t->SetBranchAddress("iPad" , &tmp_iPad  );
  t->SetBranchAddress("jPad" , &tmp_jPad  );
  t->SetBranchAddress("iAsic", &tmp_iAsic );
  t->SetBranchAddress("iFec" , &tmp_iFec  );
  t->SetBranchAddress("iPadInAsic" , &tmp_iPadInAsic  );
  t->SetBranchAddress("x0Pad" , &tmp_x0Pad  );
  t->SetBranchAddress("x1Pad" , &tmp_x1Pad  );
  t->SetBranchAddress("x2Pad" , &tmp_x2Pad  );
  t->SetBranchAddress("x3Pad" , &tmp_x3Pad  );
  t->SetBranchAddress("y0Pad" , &tmp_y0Pad  );
  t->SetBranchAddress("y1Pad" , &tmp_y1Pad  );
  t->SetBranchAddress("y2Pad" , &tmp_y2Pad  );
  t->SetBranchAddress("y3Pad" , &tmp_y3Pad  );
  t->SetBranchAddress("iPadMin", &_iPadMin );
  t->SetBranchAddress("jPadMin", &_jPadMin );
  t->SetBranchAddress("iPadMax", &_iPadMax );
  t->SetBranchAddress("jPadMax", &_jPadMax );
  //  t->Print();
  t->GetEntry(0);

  _iPad  = *tmp_iPad;
  _jPad  = *tmp_jPad;
  _iAsic = *tmp_iAsic;
  _iFec  = *tmp_iFec;
  _iPadInAsic  = *tmp_iPadInAsic;

  _x0Pad = *tmp_x0Pad;   _x1Pad = *tmp_x1Pad;   _x2Pad = *tmp_x2Pad;   _x3Pad = *tmp_x3Pad;
  _y0Pad = *tmp_y0Pad;   _y1Pad = *tmp_y1Pad;   _y2Pad = *tmp_y2Pad;   _y3Pad = *tmp_y3Pad;

  _elecSetupDone = true;
  _geomSetupDone = true;
  
  f.Close();
}




#include <TH2Poly.h>
bool femGeomViewer::init(string rootName, const femGeomManager &geo )  {
  reset();
  if( _XYviewer.get() ) return false;
  

  double xmin = +9999;
  double xmax = -9999;
  double ymin = +9999;
  double ymax = -9999;

  /// XY view
  _XYviewer.reset( new TH2Poly() );
  _XYviewer->SetName( rootName.c_str() );
  _XYviewer->SetTitle(rootName.c_str() );
  _binNumber2D.resize(geo.nRow());
  for( int ir = 0 ; ir < geo.nRow(); ir++ ) {
    _binNumber2D[ir].resize(geo.nCol());
    for( int ic = 0 ; ic < geo.nCol(); ic++ ) {
      double px[4];
      double py[4];
      for( int i = 0 ; i < 4; i++ ) {  
	  px[i] = geo.xPadCoord(ir,ic, i);
	  py[i] = geo.yPadCoord(ir,ic, i);
	  if( px[i] < xmin ) xmin = px[i];
	  if( px[i] > xmax ) xmax = px[i];
	  if( py[i] < ymin ) ymin = py[i];
	  if( py[i] > ymax ) ymax = py[i];
      }
      _binNumber2D[ir][ic] = _XYviewer->AddBin( 4, px,py);
      
    }
  }
  _XYviewer->GetXaxis()->SetTitle("x [mm]"); _XYviewer->GetXaxis()->CenterTitle();
  _XYviewer->GetYaxis()->SetTitle("y [mm]"); _XYviewer->GetYaxis()->CenterTitle();


  /// XT view
  double tmin = 49.5;
  double tmax = 199.5;
  int nbin = 25;
  _XTviewer.reset( new TH2F( (rootName + "XT").c_str(), (rootName + "XT").c_str(), geo.nCol(),xmin,xmax, nbin,tmin,tmax ) );
  _TYviewer.reset( new TH2F( (rootName + "TY").c_str(), (rootName + "TY").c_str(), nbin,tmin,tmax, geo.nRow(),ymin,ymax ) );
  _XTviewer->GetXaxis()->SetTitle("x [mm]"      ); _XTviewer->GetXaxis()->CenterTitle();
  _XTviewer->GetYaxis()->SetTitle("t [#samples]"); _XTviewer->GetYaxis()->CenterTitle();
  _TYviewer->GetXaxis()->SetTitle("t [#samples]"); _TYviewer->GetXaxis()->CenterTitle();
  _TYviewer->GetYaxis()->SetTitle("y [mm]"      ); _TYviewer->GetYaxis()->CenterTitle();

  return true;
}

void femGeomViewer::reset() {
  if( _XYviewer.get() ) _XYviewer->Reset("");
  if( _XTviewer.get() ) _XTviewer->Reset();
  if( _TYviewer.get() ) _TYviewer->Reset();
}


void femGeomViewer::setBinContent( int iPad, int jPad, int tPad, double val ) { 
  _XYviewer->SetBinContent( _binNumber2D[iPad][jPad],val );
  //  int tbin = _XTviewer->GetYaxis()->FindBin( tPad );
  double x = _XTviewer->GetXaxis()->GetBinCenter( jPad+1 );
  double y = _TYviewer->GetYaxis()->GetBinCenter( iPad+1 );
  _XTviewer->Fill( x, tPad, val );
  _TYviewer->Fill( tPad, y, val );

}

void femGeomViewer::drawXY( string option )     { _XYviewer->Draw(option.c_str() ); }
void femGeomViewer::drawXT( string option )     { _XTviewer->Draw(option.c_str() ); }
void femGeomViewer::drawTY( string option )     { _TYviewer->Draw(option.c_str() ); }

void femGeomViewer::drawCopyXY( string option ) { _XYviewer->DrawCopy(option.c_str() ); }
void femGeomViewer::drawCopyXT( string option ) { _XTviewer->DrawCopy(option.c_str() ); }
void femGeomViewer::drawCopyTY( string option ) { _TYviewer->DrawCopy(option.c_str() ); }

void femGeomViewer::setMinMax( double min, double max  ) {
  _XYviewer->SetMinimum(min);
  _XYviewer->SetMaximum(max);
  _XTviewer->SetMinimum(min);
  _XTviewer->SetMaximum(max);
  _TYviewer->SetMinimum(min);
  _TYviewer->SetMaximum(max);

}



int femGeomViewer::getBinNumber( int iPad, int jPad) const { return _binNumber2D[iPad][jPad]; }



////// another way to visualize the geometry... not really useful TH2Poly makes it easier.
// void femGeomManager::setGeo2DVisual(void) {
//   double xMin = _x3Pad[_iPadMax][_jPadMin];
//   double yMin = _y0Pad[_iPadMin][_jPadMin];
//   double xMax = _x2Pad[_iPadMax][_jPadMax];
//   double yMax = _y2Pad[_iPadMax][_jPadMax];

//   _padsGeom.resize(_x0Pad.size());
//   for( unsigned ix = 0 ; ix < _padsGeom.size(); ix++ ) {
//     _padsGeom[ix].resize( _x0Pad[ix].size(), (TPolyLine*) 0 );
//     for( unsigned iy = 0 ; iy < _padsGeom[ix].size(); iy++ ) {
//       _padsGeom[ix][iy] = new TPolyLine(5);
//       _padsGeom[ix][iy]->SetPoint(0,  (_x0Pad[ix][iy] - xMin)/(xMax-xMin)*0.95+0.025, (_y0Pad[ix][iy]-yMin)/(yMax-yMin)*0.925+0.025 );
//       _padsGeom[ix][iy]->SetPoint(1,  (_x1Pad[ix][iy] - xMin)/(xMax-xMin)*0.95+0.025, (_y1Pad[ix][iy]-yMin)/(yMax-yMin)*0.925+0.025 );
//       _padsGeom[ix][iy]->SetPoint(2,  (_x2Pad[ix][iy] - xMin)/(xMax-xMin)*0.95+0.025, (_y2Pad[ix][iy]-yMin)/(yMax-yMin)*0.925+0.025 );
//       _padsGeom[ix][iy]->SetPoint(3,  (_x3Pad[ix][iy] - xMin)/(xMax-xMin)*0.95+0.025, (_y3Pad[ix][iy]-yMin)/(yMax-yMin)*0.925+0.025 );
//       _padsGeom[ix][iy]->SetPoint(4,  (_x0Pad[ix][iy] - xMin)/(xMax-xMin)*0.95+0.025, (_y0Pad[ix][iy]-yMin)/(yMax-yMin)*0.925+0.025 );
//       _padsGeom[ix][iy]->SetLineColor(kBlack);
//       _padsGeom[ix][iy]->SetLineWidth(2);
//       _padsGeom[ix][iy]->SetUniqueID( iy + _padsGeom[ix].size() * ix );
//     }
//   }
//   _geo2DSetupDone = true;
// }


// void femGeomManager::drawGeom(void) {
//  for( unsigned ix = 0 ; ix < _padsGeom    .size(); ix++ ) 
//  for( unsigned iy = 0 ; iy < _padsGeom[ix].size(); iy++ ) {   
//    _padsGeom[ix][iy]->Draw("f");
//  }
// }

// #include <TH2F.h>
// #include <TStyle.h>

// void femGeomManager::drawEvt(TH2F *hEvt) {
  
//   double L = hEvt->GetMaximum()-hEvt->GetMinimum();
//   for( int icol = 1; icol <= hEvt->GetXaxis()->GetNbins(); icol++ ) 
//   for( int irow = 1; irow <= hEvt->GetYaxis()->GetNbins(); irow++ ) { 
//     int ic = double((hEvt->GetBinContent(icol,irow)-hEvt->GetMinimum()) / L) * gStyle->GetNumberOfColors() ;
//     if( ic >=  gStyle->GetNumberOfColors() ) ic = gStyle->GetNumberOfColors()-1 ;
//     if( ic <  0 ) ic = 0;
//     _padsGeom[irow-1][icol-1]->SetFillColor( gStyle->GetColorPalette(ic) );
//     _padsGeom[irow-1][icol-1]->Draw("f");    
//   }
// }
