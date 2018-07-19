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
nRows = 24
nCols = 72
nPads = nRows * nCols

#
#-----------------------------------------------------------------------------

# Import necessary modules
from math import *
import sys

#-----------------------------------------------------------------------------
# Calculated parameters :
#------------------------
PHI  = PHI_D * pi / 180.   # angle in radian

dr   = (Rout-Rin) / nRows  # radius diffence spanned by one pad
dphi = PHI / nCols         # angle spanned by one pad

r0   =  Rin                # inner radius           of the pads in first row
phi0 = -PHI/2.             # angle of the left edge of the pads in first column

half = Rout * sin(phi0)
ymin = Rin  * cos(phi0)

#y0   = (Rout+Rin) / 2      # translate y coord to make this origin
y0   = (Rout+ymin) / 2      # translate y coord to make this origin

print '#' + '-'*77
print '# Rin=%.2f, Rout=%.2f'  % (Rin, Rout)
print '# PHI=%.4f, PHI/2=%.4f' % (PHI_D, PHI_D/2.) 
print '# r0=%.4f, dr=%.4f'     % (r0, dr)
print '# phi0=%.4f, dphi=%.4f' % (phi0, dphi)
print '# half width=%.2f, ymin=%.2f, ymax=%.2f' % (abs(half), ymin, Rout)
print '# Origin = (0, %.4f)'   % y0
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

        # Radii and angles
        rL   = r0   + (row * dr)    # L for Lower
        rU   = rL + dr              # U for Upper

        phiL = phi0 + (col * dphi)  # L for Left
        phiR = phiL + dphi          # R for right

        sinL = sin(phiL)
        sinR = sin(phiR)
        cosL = cos(phiL)
        cosR = cos(phiR)
        
        # LL
        x[0][id] = rL * sinL
        y[0][id] = rL * cosL

        # LR
        x[1][id] = rL * sinR
        y[1][id] = rL * cosR

        # UR
        x[2][id] = rU * sinR
        y[2][id] = rU * cosR

        # UL
        x[3][id] = rU * sinL
        y[3][id] = rU * cosL

#
# Print out
#
print
print 'half_width %.2f' % abs(half)
print 'ymin       %.2f' % ymin
print 'ymax       %.2f' % Rout
for ipad in range(nPads):

    row = ipad / nCols
    col = ipad % nCols

    print '%3d %3d' % (row, col),   # continue for coordinates
    for i in range(4):
        print '%9.4f %9.4f' % (x[i][ipad], y[i][ipad] - y0),  # shift y-coord
    print
    
