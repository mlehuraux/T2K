#!/usr/bin/perl -w

use strict;

#
# Usage: depgen_f90.pl [-Ddef] [-Ipath] fortran_file.f90
#
# This script writes on standard output the makefile dependencies of
# fortran_file.f90 (include files and modules)
#
# It understands all the C preprocessor directives and also the USE
# and MODULE directives of fortran 90.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
# Author: P.F. Giraud
#

my $file = $ARGV[$#ARGV];

my $ofile = $file;
$ofile =~ s/\.[fF].*/.o/;
$ofile = "\$(OBJDIR)/".$ofile;

my $dfile = $file;
$dfile =~ s/\.[fF].*/.d/;
$dfile = "\$(DEPDIR)/".$dfile;

my @targets;
my @deps;
my @moddeps;

sub pushdep {
    my ($file) = @_;
    foreach my $i (@deps) {
	return if ($i eq $file);
    }
    push @deps, ($file) if ( -e $file );
}

# Run the C preprocessor on the fortran file and parse its output
open (CPP, "cpp ".join(" ", @ARGV)." |") || die "Cannot execute cpp";
while (<CPP>) {
    my $line = $_;
    chomp($line);

    # First step is to look for files included by the C preprocessor
    if ($line =~ m/^# *[0-9]+ *"(.*)"/) {
	pushdep $1;
    }

    # Second step is to parse FORTRAN directives "USE" and "MODULE"
    $line = lc($line);
    if ($line =~ m/^[ \t]*use[ \t]+([^, \t]*)([, \t]+|$)/) {
	my $module = "\$(MODDIR)/".$1.".mod";
	push @moddeps, ($module);
    }
    if ($line =~ m/^[ \t]*module[ \t]+([^ \t]*)[ \t]*($|!)/) {
	my $module = "\$(MODDIR)/".$1.".mod";
	push @targets , ($module);
    }
}

# Clean up the moddeps array (remove duplicates, remove the modules
# that are already defined in the fortran file)

# remove irrelevant modules...
my @moddepsnew;
foreach my $m (@moddeps) {
    my $useit = 1;
    foreach my $mod (@targets) {
	$useit = 0 if ($m eq $mod);
    }
    push @moddepsnew, ($m) if ($useit);
}
@moddeps = @moddepsnew;

# ... and sort and uniq to remove duplicates
my %seen;
@moddeps = grep !$seen{$_}++, @moddeps;
@moddeps = sort @moddeps;

# Finally we write the makefile rules on standard output
print "$dfile : ".join(" ",@deps)."\n\n";
print "$ofile : ".join(" ",@deps)." ".join(" ",@moddeps)."\n\n";
print join(" ",@targets)." : $ofile\n\n" if ($#targets != -1);
