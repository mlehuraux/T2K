#!/bin/sh

    # Try to guess the architecture of the host system
    # Note, that there's no attempt at guessing the compiler.
    # The `native' compiler is used in all cases.
    # If a user really want a specifific compiler, s/he better use the
    # first argument to do so.
arch=`uname -s | tr '[A-Z]' '[a-z]'`
chip=`uname -m | tr '[A-Z]' '[a-z]'`
rele=`uname -r`

case "$arch:$chip:$rele" in
    aix*)                  arch=aix5            ;;
    osf1*:alpha:*)         arch=alphacxx6       ;;
    freebsd*:*:6*)         arch=freebsd5        ;;
    freebsd*:*:5*)         arch=freebsd5        ;;
    freebsd*:*:4*)         arch=freebsd4        ;;
    freebsd*:*:*)          arch=freebsd         ;;
    hp-ux:ia64:*)          arch=hpuxia64acc     ;;
    hp-ux:*:*)             arch=hpuxacc         ;;
    hurd*:*:*)             arch=hurddeb         ;;
    linux:ia64:*)          arch=linuxia64gcc    ;;
    linux:x86_64:*)        arch=linuxx8664gcc   ;;
    linux:alpha:*)         arch=linuxalphagcc   ;;
    linux:arm*:*)          arch=linuxarm        ;;
    linux:ppc64*:*)        arch=linuxppc64gcc   ;;
    linux:ppc*:*)          arch=linuxppcgcc     ;;
    linux:i*86:*)          arch=linux           ;;
    openbsd*:*:*)          arch=openbsd         ;;
    lynx:*:*)              arch=lynxos          ;;
    darwin:power*:*)       arch=macosx          ;;
    darwin:*86*:*)         arch=macosx          ;;
    irix*:sgi*:*)          arch=sgicc           ;;
    sunos:sun*:6*)         arch=solarisCC5      ;;
    sunos:sun*:5*)         arch=solarisCC5      ;;
    sunos:sun*:4*)         arch=solaris         ;;
    sunos:i86pc:5*)        arch=solarisCC5      ;;
    cygwin_*:*86:*)        arch=win32           ;;
    cygwin_*:pentium:*)    arch=win32           ;;
    cygwin_*:ia64)         arch=win32           ;;
    *)
	echo "Attempts at guessing your architecture failed."
	echo "Please specify the architecture as the first argument."
	echo "Do '$0 --help' for a list of avaliable architectures."
	exit 1
	;;
esac
#if [ "$arch" = "macosx" ]; then
#    if [ `sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2` = 5 ]; then
#	if `sysctl machdep.cpu.extfeatures | grep "64" > /dev/null  2>&1` ; then
#	    arch=macosx64
#	fi
#    fi
#fi

echo $arch
