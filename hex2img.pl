#!/usr/bin/perl

use strict;
use warnings;

while (<STDIN>) {
	chomp;
	for (split) {
		print pack("S<", oct($_));
	}
}
