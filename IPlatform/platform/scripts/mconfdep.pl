#!/usr/bin/env perl
use strict;
use warnings;

#
# get the Kernel-Config optinos that package_kmod* depends on
#
sub gen_kconfig_overrides() {
	my %config;
	my %kconfig;
	my $package;
	my $pkginfo = shift @ARGV;
	my $cfgfile = shift @ARGV;

	open FILE, "<$cfgfile" or return;
	while (<FILE>) {
		/^(kmod)-(.+)$/ and $config{"CONFIG_PACKAGE_kmod-$2"} = 1;
	}
	close FILE;

	open FILE, "<$pkginfo" or return;
	while (<FILE>) {
		/^Package:\s*(.+?)\s*$/ and $package = $1;
		/^Kernel-Config:\s*(.+?)\s*$/ and do {
			my @config = split /\s+/, $1;
			foreach my $config (@config) {
				my $val = 'm';
				my $override;
				if ($config =~ /^(.+?)=(.+)$/) {
					$config = $1;
					$override = 1;
					$val = $2;
				}
				if ($config{"CONFIG_PACKAGE_$package"} and ($config ne 'n')) {
					next if (($kconfig{$config}) and ($kconfig{$config} eq 'y'));
					$kconfig{$config} = $val;
				} elsif (!$override) {
					$kconfig{$config} or $kconfig{$config} = 'n';
				}
			}
		};
	};
	close FILE;

	foreach my $kconfig (sort keys %kconfig) {
		if ($kconfig{$kconfig} eq 'n') {
#			print "# $kconfig is not set\n";
		} else {
			print "$kconfig=$kconfig{$kconfig}\n";
		}
	}
}

#
# first get the CONFIG_PACKAGE_* options from file .config
# then get all the kmod-* that CONFIG_PACKAGE_* options depends on
#
sub gen_package_kmods() {
	my %config;
	my %depends;
	my $package;
	my $pkginfo = shift @ARGV;
	my $cfgfile = shift @ARGV;

	open FILE, "<$cfgfile" or return;
	while (<FILE>) {
		/^(CONFIG_PACKAGE_.+?)=(.+)$/ and $config{$1} = 1;
	}
	close FILE;

	open FILE, "<$pkginfo" or return;
	while (<FILE>) {
		/^Package:\s*(.+?)\s*$/ and $package = $1;
		/^Depends:\s*(.+)\s*$/ and do {
			my @depends = split /\s+/, $1;
			foreach my $config (@depends) {
				if ($config =~ /^(\+*)kmod-(.+)$/) {
					$config = "kmod-$2";
						if ($config{"CONFIG_PACKAGE_$package"}) {
							next if ($depends{$config});
							$depends{$config} = 1;
						}
				}
			}
		};
	};
	close FILE;

	foreach my $depend (sort keys %depends) {
		if ($depends{$depend} == 1) {
			print "$depend\n";
		}
	}
}

sub parse_command() {
	my $cmd = shift @ARGV;
	for ($cmd) {
		/^depend/ and return gen_package_kmods();
		/^kconfig/ and return gen_kconfig_overrides();
	}
	print <<EOF
Available Commands:
	$0 depend [file] [config] get kmod-*
	$0 kconfig [file] [config] get kernel config

EOF
}

parse_command();
