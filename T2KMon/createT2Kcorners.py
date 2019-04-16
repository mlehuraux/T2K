#! /usr/bin/env python

#
# Create LPTPClayout.txt file which contains coordinates of 4 corners of each
# pad in given (row,col).
#
# Calculates real geometry which will be used in display program
# NOT for FTPC analysis program.
#
# Conventions:
#
#   1. Lower-Left corner when we look at the pad surface is
#              (row,col) = (0,0)
#
#   2. Corner label:  3 2   
#                     0 1
#

#-----------------------------------------------------------------------------
# INPUTS : 
#---------
#
# Dimensions
#
PHI_D = 8.3872          # total angle of the pad panel        ( degree )
Rin   = 1430.5          # inner radius of the active pad area ( mm )
Rout  = 1599.5          # outer radius of the active pad area ( mm )

#
#-----------------------------------------------------------------------------
#
# Rows and Columns
#
nRows = 48
nCols = 36
nPads = nRows * nCols

#
#-----------------------------------------------------------------------------

# Import necessary modules
from math import *
import sys

#-----------------------------------------------------------------------------
# Calculated parameters :
#------------------------

dx   = 7.5
dy   = 9.45
x0   = -360/2
y0   = 480/2

#half = Rout * sin(phi0)
#ymin = Rin  * cos(phi0)

#y0   = (Rout+Rin) / 2      # translate y coord to make this origin
#y0   = (Rout+ymin) / 2      # translate y coord to make this origin

print '#' + '-'*77
print '# nCols=%2d, nRows=%2d'  % (nCols, nRows)
print '#' + '-'*77
print ' Row Col    x0    y0    x1    y1    x2    y2    x3    y3'
print '#' + '-'*77

#-----------------------------------------------------------------------------
# Let's Do It :
#--------------

# Prepare some arrays
x = [ [ 0.0 for i in range(nPads) ] for j in range(4) ]  # 4 x nPads
y = [ [ 0.0 for i in range(nPads) ] for j in range(4) ]  # 4 x nPads

#
# Calculate 4 corners of the pads
#
for row in range(nRows):
    for col in range(nCols):

        id = col + row * nCols  # inverse : row=id/nCols, col=id%nCols

        # 
        rL = x0 + col*dx    # L for Lower
        rU = y0 + row*dy    # U for Upper

      
        # LL
        x[0][id] = rL
        y[0][id] = rU

        # LR
        x[1][id] = rL + dx
        y[1][id] = rU

        # UR
        x[2][id] = rL + dx
        y[2][id] = rU + dy

        # UL
        x[3][id] = rL
        y[3][id] = rU + dy

#
# Print out
#
print
print 'half_width %3f' % x0
print 'ymin       %3f' % 0
print 'ymax       %3F' % 480
for ipad in range(nPads):

    row = ipad / nCols
    col = ipad % nCols

    print '%3d %3d' % (row, col),   # continue for coordinates
    for i in range(4):
        print '%9.4f %9.4f' % (x[i][ipad], y[i][ipad] - y0),  # shift y-coord
    print
    
