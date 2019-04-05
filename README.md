# T2K TPC  analysis package


The package allow to convert data from ILC DAQ format (.acq) to ROOT format,
saving all the information about the ADC vs. time for each pad.
The included macros can be used for the data analysis.

acq-->root converter (DataWriter_devel) is provided from the ILC team (Boris)

## Requierments:
* ROOT
* BOOST for acq-->root

## Compilation

To work with this package you need to compile the ROOT dictionary in order to be able to store c++ vectors
and vectors of vectors in the ROOT file.
To do this in the macros/ilc folder run
```bash
rootcint -f  mydict.C -c LinkDef.h
```

This will generate mydict.C and mydict.h the name can NOT be changed as the Makefile uses mydict.C notation. This operation shoud be done once and then you can forgot about the dictionary generation.

1) In the DataWriter_devel folder the RootConverter should be compiled. Simply run
```bash
make clean
make
```
to build DataWriter_devel/bin/RootConverter.exe

2) To compile the macro you need run make command and specify
the macro code, that you are compiling. The app/ folder should exist in the project. In the macro folder:
```bash
make SRC=your_macro.C
```
This will produce executable app/your_macro.exe


## Running

1.  To run acq-->root converter in the run
```bash
./bin/acqToRootConverterExe -t 0 -m 1 -f  run_XXX.acq --noxml
```
It will unpack the data in run_XXX.acq into run_XXX.root. The output will be stored in the same folder where run_XXX.acq is located

2.  To run the macro call
```bash
app/your_macro.exe -f output_root_file.root -o output_dir
```
This will produce all histos and output root files you requested in the macros code.

## Pedestal subtraction

The raw files could be provided w/ and w/o zero suppression. User can evaluate zero level from the pedestal file and then switch on zero subtraction in the acq-->root converter.

To evaluate pedestal level:
1. Convert pedestal file from acq to root with a standard command
```bash
./bin/acqToRootConverterExe -t 0 -m 1 -f  pedestal_run.acq --noxml
```
2. Analise the result to extract mean and sigma of the noise
```bash
app/noise.C -f pedestal_run.root -o data/
```
With this command several files in the data/ folder will be created:

* root file with 2D and 1D distributions of mean and sigma of the noise, histogram for each channel with the Gaussian fit;

* pdf files with 2D and 1D distributions;

* pedestal.txt file with #channel mean sigma zero;

3. To take into account the extracted pedestal value you need to run the acqToRootConverterExe with option 'p'
```bash
./bin/acqToRootConverterExe -t 0 -m 1 -f  physical_run.acq -p data/pedestals.txt --noxml
```

### Notes
1. The noise Gaussian fit could fail sometimes. Thus enormous value of mean/sigma could be observed. In this case user need to check the appropriate histogram in the pedestals.root file, fit it in a proper way and fill mean, sigma and zero values in the pedestals.txt manually.
2. Zero suppression level (value $`N`$ in eq. $`zero = mean + N\times\sigma`$) could be set up in the different places:
* in noise.C macro calculate zero with your rule
* in acqToRootConverterExe.cc we can compute from mean and sigma from pedestals.txt
* modify the last column in pedestals.txt file manually
By default we compute $`4\sigma`$ suppression in noise.C, store it in pedestals.txt and use as input in acqToRootConverterExe
3. In commit from 5.04.19 the pedestals.txt is produced from RUN_00173.00000.acq and $`4\sigma`$ suppression is computed.


## DATA FORMAT

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