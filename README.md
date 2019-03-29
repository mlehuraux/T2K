# T2K TPC  analysis package


The package allow to convert data from ILC DAQ format (.acq) to ROOT format,
saving all the information about the ADC vs. time for each pad.
The included macros can be used for the data analysis.

acq-->root converter (DataWriter_devel) is provided from the ILC team (Boris)

## Requierments:
1) ROOT
2) BOOST for acq-->root

## COMPILATION

To work with this package you need to compile the ROOT dictionary in order to be able to store c++ vectors
and vectors of vectors in the ROOT file.
To do this in the macros/ilc folder run
```
rootcint -f  mydict.C -c LinkDef.h
```

This will generate mydict.C and mydict.h the name can be changed.

1) In the DataWriter_devel folder the RootConverter should be compiled. Simply run
```
make clean
make
```
to build DataWriter_devel/bin/RootConverter.exe

2) To compile the macro you need run make command and specify
the macro code, the path to dictionary and the executable. In the macro folder:
```
make SRC=your_macro.C
```
This will produce executable your_macro.exe



## RUN

1) To run acq-->root converter in the run
```
./bin/acqToRootConverterExe -t 0 -m 1 -f  "run_XXX.acq" --noxml
```
It will unpack the data in run_XXX.acq into run_XXX.root

2) To run the macro call
```
./your_macro.exe -f output_root_file.root -o output_dir
```
This will produce all histos and output root files you requested in the macros code.



## DATA FORMAT

** IMPORTANT! Geometry could be rotated by 90 degrees **

In order to be able to create your own macros here it is the data structure of the RootConverter
output file.

output_root_file.root:

    |-- TTree padData      # contains the pad data
        |-- PadphysChannels # vector<short>          # vector of phys.l channels for the pads
        |-- PadADCvsTime    # vector<vector<short> > # ADC vs. time
    |-- TTree femGeomTree  # contains geometry
        |-- iPad            # vector<int>            # i position of the physical channel
        |-- jPad            # vector<int>            # j position of the physical channel
        |-- xPad            # vector<double>         # x position of the physical channel
        |-- yPad            # vector<double>         # y position of the physical channel
        |-- dxPad           # vector<double>         # x size of a pad
        |-- dyPad           # vector<double>         # y size of a pad

So to look at the data:
```
loop over i in PadphysChannels[i] --> loop over hit pads
loop over j in PadADCvsTime[i][j] --> loop over ADC for pad i
```

Geometry:
```
Physical channels go from 0 to 1727
j corresponds to columns goes from 0 to 47
i corresponds to rows    goes from 0 to 35
x corresponds to colums
y corresponds to rows
```