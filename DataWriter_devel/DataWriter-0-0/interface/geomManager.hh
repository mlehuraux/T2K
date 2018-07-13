#ifndef geomManager_hh__
#define geomManager_hh__

#include <vector>
#include <map>

#include <string>

class TFile;

class femGeomManager {
public:
  femGeomManager(void); 
  ~femGeomManager(void); 

  femGeomManager( const femGeomManager & geo );
  femGeomManager& operator=( const femGeomManager & );

  /// init geometry from flat ascii files
  void setElectronicMap( std::string map ) { _elecMap = map; readElectronic(); }
  void setGeometryMap(   std::string map ) { _geomMap = map; readGeometry()  ; }

  int nPad( void ) const { return _iPad.size(); }
  int nRow(void) const { return _iPadMax - _iPadMin + 1; }
  int nCol(void) const { return _jPadMax - _jPadMin + 1; }


  int iPad( int iPhysCh ) const; /// pad row    from phys channel
  int jPad( int iPhysCh ) const; /// pad column from phys channel
  int asic( int iPhysCh ) const;
  int fec( int iPhysCh  ) const;
  int padInAsic( int iPhysCh ) const;


  int iPadMin(void) const { return _iPadMin;}
  int iPadMax(void) const { return _iPadMax;}
  int jPadMin(void) const { return _jPadMin;}
  int jPadMax(void) const { return _jPadMax;}



  //// isobarycenter of the pad from the 4 corners 
  double xPad(int i, int j) const;
  double yPad(int i, int j) const;
  double xPad(int iPhysCh) const { return xPad(iPad(iPhysCh),jPad(iPhysCh)); }
  double yPad(int iPhysCh) const { return xPad(iPad(iPhysCh),jPad(iPhysCh)); }

  double xPadCoord( int i, int j, int k  ) const;  /// return double[] for root
  double yPadCoord( int i, int j, int k  ) const;  /// return double[] for root

  /// i/o to store geom in a root file
  void dumpToRootFile( TFile *f, const std::string & geoTreeName = "femGeomTree" );
  void initFromRootFile(std::string rootFileName, const std::string &treeName = "femGeomTree" );
  

private:
  void readElectronic(void);
  int  readGeometry(void);

  std::string _elecMap;
  std::string _geomMap;
  int _iPadMin, _iPadMax, _jPadMin, _jPadMax;

  std::vector<int>      _iPad, _jPad;
  std::vector<int>      _iAsic, _iFec, _iPadInAsic; /// electronic path

  std::vector<std::vector<double> > _x0Pad, _x1Pad, _x2Pad, _x3Pad;
  std::vector<std::vector<double> > _y0Pad, _y1Pad, _y2Pad, _y3Pad;

  bool _elecSetupDone;
  bool _geomSetupDone;

};



/// use as a 2D histogram
#include <memory>
class TH2Poly;
class TH2F;

class femGeomViewer {
public:
  femGeomViewer(std::string rootName, const femGeomManager &geo ) { init(rootName,geo); }
  femGeomViewer(void ) : _XYviewer(0), _XTviewer(0), _TYviewer(0) {}  

  /// init is actually done only if the viewer is not setup otherwise this is really long
  bool init(std::string rootName, const femGeomManager &geo );  
  void reset();

  void setBinContent( int iPad, int jPad, int tPad, double val );
  void drawXY(std::string options = "" );
  void drawXT(std::string options = "" );
  void drawTY(std::string options = "" );
  void drawCopyXY(std::string options = "" );
  void drawCopyXT(std::string options = "" );
  void drawCopyTY(std::string options = "" );
  void setMinMax( double min, double max  );
  
  
  /// accessors if the user want to fill the histo himself...
  // no need to worry about memory
  std::auto_ptr<TH2Poly> getHistoXY(void) { return _XYviewer; }
  int  getBinNumber( int iPad, int jPad) const;
  
private:
  /// use smart pointer so it owns the pointer;
  std::auto_ptr<TH2Poly>         _XYviewer;
  std::auto_ptr<TH2F>            _XTviewer;
  std::auto_ptr<TH2F>            _TYviewer;

  std::vector<std::vector<int> > _binNumber2D;
  
};




#endif
