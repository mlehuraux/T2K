# Source files to compile
FILES := xmlReader geomManager dataDecoder tpcPadDataFormats tpcDataAnalyzer tpcPedCalc utils MicroMegasChannelDataFormats MutigenStripDataFormats


# Header files to use for dictionary generation
# In this module: is the same as FILES
DICTFILES :=

# Executable files
PROGRAMS := xmlReaderExe acqToRootConverterExe rootAnalyzerExe rootHodoAnalyzerExe
NEEDS_ROOT  := yes
NEEDS_BOOST := yes