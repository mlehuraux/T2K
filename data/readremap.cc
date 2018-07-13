// read TPClayout.txt and redo the mapping for LPMon
void readremap() {
#include <iostream>
FILE *fp;
FILE *foutput;

  // read TPC layout
  // format
// Pad   Group    Row         x       y     x       y
 int igood = 0;
 int ipad, igroup, irow;
 float x,y,dx,dy;
 int icoltemp;
 int iasicLP[1728];
 int irowLP[1728];
 int icolLP[1728];
 int ipadLP[1728];
char buffer[100];
 int i;

 Int_t imaxrowtemp = -1;
 float maxx = -1.;
 float maxy = -1.;
 float minx = 1e6;
 float miny = 1e6;

fp = fopen("./TPClayout.txt", "r");//open file for reading
if(fp == NULL){
printf("error reading file\n");
}else{
int i1;

//  for (i1=0; i1<9;i1++) {
//    //fscanf(fp, "%*[^\n]\n", NULL);
// fgets(buffer, 100, fp);
//  }



for(i = 0; i < 1728; i++){
  int iret = fscanf(fp, "%d %d %d %f %f %f %f", &ipad, &igroup, &irow, &x,&y,&dx,&dy );
  //  cout << " i " << i << " ipad " << ipad  << " irow  " << irow << endl;
  cout << " i " << i << " x " << x << " y " << y << " dx " << dx << " dy " << dy << endl;

  //  cout << " iret " << iret << endl;
  //  if ( i< 100 ) std::cout << " iret " << iret << std::endl;
  //if (iret == 4 ) continue;
  if (iret == 7) {
  igood++;
  // fill tables
  irowLP[i] = irow;
  float coltemp = (y-0.5*dy)/dy;
  icoltemp = TMath::Nint(coltemp)+18;
  float rowtemp = (x-0.5*dx)/dx;
  int irowtemp = TMath::Nint(rowtemp)+24;
  if (irowtemp > imaxrowtemp) imaxrowtemp = irowtemp;
  //  cout << " i " << i << " row " << irow << " irowtemp " << irowtemp << " col " << icoltemp << endl;

  irowLP[i] = irowtemp;
  icolLP[i] = icoltemp;
  ipadLP[i] = ipad;

  if (maxy < y)
    maxy = y;
  if (maxx < x)
    maxx = x;
  if (miny > y)
    miny = y;
  if (minx > x)
    minx = x;

  }

 }

 }

 cout << "imaxrowtemp " << imaxrowtemp << endl;
 cout << minx << "   " << maxx << endl;
 cout << miny << "   " << maxy << endl;

 fclose(fp);

  // print out map in format
  // asic row col pad

foutput = fopen("./TPCLPlayout1.txt", "w");//open new file for writing

 int iasic=0;
 int ifec = 0;
 ipad = 0;
 int imaxrow = -1000;
 int imaxcol = -1000;
 int imincol = 1000;
 int iminrow = 1000;

  map<int,int> iPad, jPad;

for(i = 0; i < 1728; i++){
  ifec = i/288;
  iasic = (i%288)/72;
  ipad = (i%288)%72;
  if (irowLP[i]<iminrow) iminrow = irowLP[i];
  if (irowLP[i]>imaxrow) imaxrow = irowLP[i];
  if (icolLP[i]<imincol) imincol = icolLP[i];
  if (icolLP[i]>imaxcol) imaxcol = icolLP[i];

  int column = irowLP[i];

  int iFEC = 0;

  if      (column>=40) iFEC=5;
    else if (column>=32) iFEC=4;
    else if (column>=24) iFEC=3;
    else if (column>=16) iFEC=2;
    else if (column>=8)  iFEC=1;
    else                 iFEC=0;

    int iASIC = iasic + 4*iFEC;
  int ipad2 = ipad + 72*iASIC;

  if (ipad2 != i) {
    cout << "ALARM" << endl;
    cout << irowLP[i] << endl;
    cout << ipad2 << "  " << iASIC << "   " << iFEC << endl;
    cout << i <<  "  " << iasic  << "     " << ifec<< endl;
  }

  /*if ( icolLP[i]== 11 && irowLP[i] == 11 ) {
    printf("line i %d %d %d %d \n",i,iasic,icolLP[i],irowLP[i],ipad);
  }
  if ( icolLP[i]== 5 && irowLP[i] == 11 ) {
    printf("line i %d %d %d %d \n",i,iasic,icolLP[i],irowLP[i],ipad);
  }*/

  //  fprintf(foutput,"%d %d %d %d \n",iasic,irowLP[i],icolLP[i],ipad);
  // finally invert row colum
  fprintf(foutput,"%d %d %d %d \n",iasic,icolLP[i],irowLP[i],ipad);
 }

 fclose(foutput);
 cout << " min row " << iminrow << " max " << imaxrow << endl;
 cout << " min col " << imincol << " max " << imaxcol << endl;

}
