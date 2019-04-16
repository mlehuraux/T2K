#! /usr/bin/env python

#
# Calculate lower-left and upper-right corners of approximated rectangle
#

from math import *

def main():

    nRow = 24;
    nCol = 72;
    nPad = nRow * nCol;

    PI   = pi
    PHI  = 4.1936 * PI / 180.  # Half angle (radian)
    Rin  = 1430.5              # inner radius of the pad array (mm)
    Rout = 1599.5              # outer radius of the pad array (mm)

    r0   = Rin
    phi0  = -PHI

    dr   = (Rout-Rin) / nRow;
    dphi = PHI / (nCol/2); 

    half = Rout * sin(phi0)
    ymin = Rin  * cos(phi0)

    #y0   = (Rout+Rin) / 2      # translate y coord to make this origin
    y0   = (Rout+ymin) / 2      # translate y coord to make this origin

    print '#' + '-'*77
    print '# Rin=%.2f, Rout=%.2f'  % (Rin, Rout)
    print '# PHI/2=%.4f'           % (PHI,) 
    print '# r0=%.4f, dr=%.4f'     % (r0, dr)
    print '# phi0=%.4f, dphi=%.4f' % (phi0, dphi)
    print '# half width=%.2f, ymin=%.2f, ymax=%.2f' % (abs(half), ymin, Rout)
    print '# Origin = (0, %.4f)'   % y0
    print '#' + '-'*77
    print ' Index Row  Col    x0    y0    x1    y1    '
    print '#' + '-'*77


    height = dr  # height is the same for all pads
    width  = 0   # width is changing according to the radius

    # Calculate (r,phi) of the pad centers
    for irow in range(nRow):
        r = r0 + (irow * dr)

        for icol in range(nCol):

            id = icol + irow * nCol
            phi   = phi0 + (icol * dphi)
            width = 2 * r * sin(dphi/2.)

            xc = r * sin(phi);
            yc = r * cos(phi);
            yc -= y0;

            xLL = xc - width/2.
            yLL = yc - height/2.
            
            xUR = xc + width/2.
            yUR = yc + height/2.

            #print "%5d %4d %4d %10.4f %10.4f %10.4f %10.4f" % \
            #      (id, irow, icol, xc, yc, width, height)
            print "%5d %4d %4d %10.4f %10.4f %10.4f %10.4f" % \
                  (id, irow, icol, xLL, yLL, xUR, yUR)


if __name__ == '__main__':

    main()

