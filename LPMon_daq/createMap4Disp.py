#! /usr/bin/env python
#
# Read the map file created by David, and create another map.
#
# Map : (row,col) --> sequential index and global channel id
#

import sys

def main():

    fLayout = file("MapLayoutFile.txt")

    # Skip 8 comment lines
    for i in range(8):
        fLayout.readline()

    # Some constants
    kNRows           = 24
    kNCols           = 72
    kPhysChanPerAsic = 72
    kPhysChannelMax  = 1728  # nRows x nCols

    # We need a dictionary to do this.
    # Key is row,col and value is the pad id
    #
    RowCol = [ -1 for i in range(kPhysChannelMax) ]

    missingChanInfo = []

    #
    # Read Map 
    #
    #  asic_fec: Local ASIC id in a FEC         : 0-3
    #  pad     : Local Pad  id in a ASIC sector : 0-71
    #
    for line in fLayout.xreadlines():
        try:
            asic_fec, row, col, pad = line.split()

            asic_fec = int(asic_fec)
            row      = int(row)
            col      = int(col)
            pad      = int(pad)
        except:
            print "Wrong format.  Check number of lines to skip"
            sys.exit(1)

        #
        # Each FEC handles 12 columns sequentially !
        #
        # NOTE: Modify iFEC assignment if we need to change mapping
        #
        if (col>=60)           : fec=0
        if (col>=48 and col<60): fec=1
        if (col>=36 and col<48): fec=2
        if (col>=24 and col<36): fec=3
        if (col>=12 and col<24): fec=4
        if (col<12)            : fec=5

        #
        # The Map : (Row, Col) --> PhysChan
        #
        # NOTE: PhysChan is the most important one,
        #       which is the one that we can get from the ACQ data file, and
        #       which should be used to find (row, col) of the pad from it !
        #
    
        #
        # NOTE : 2 pads denoted by -1 are being used for HV.
        #        This results in two missing channel numbers : 72 and 73 !!!
        #
        if  pad == -1:
            info = '#  pad at %2d,%2d is for HW' % (row, col)
            missingChanInfo.append( info )
            continue

        asic = asic_fec + 4*fec;            # global asic id
        chan = pad + asic*kPhysChanPerAsic; # to be mapped to (row,col) !!!

        index = col + row * kNCols
        if RowCol[index] == -1:   # not occupied yet
            RowCol[index] = chan
        else:
            print 'chan %4d is already occupied by other(s)' % chan
            print '%d,%2d,%2d,%4d' % (asic_fec, row, col, pad)


    # Close file
    fLayout.close()

    #
    # Write a new MAP : chan --> (row, col)
    #
    # To make line parsing simple '#' is prepended to comment lines.
    #
    print '#                                          '
    print '# MAP for Data Channel to Layout (Row,Col) '
    print '#                                          '
    print '# Convention:                              '
    print '#    - Looking at the Pad Surface          '
    print '#    - Lower-Left corner is (0,0)          '
    print '#                                          '
    print '# Create by Yun-Ha using David\'s Map      '
    print '#                                          '
    print '# NOTE:                                    '

    for info in missingChanInfo:
        print info

    print '#                                          '
    print '#-------------------------------           '
    print '# Row Col PhysChan                         '
    print '#-------------------------------           '

    for row in range(kNRows):
        for col in range(kNCols):
            index = col + row * kNCols

            print '%4d %3d %7d %7d' % (row, col, index, RowCol[index])

#
# MAIN
#
if __name__ == '__main__':

    main()
    
