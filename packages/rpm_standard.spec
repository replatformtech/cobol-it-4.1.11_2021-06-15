Name:           cobol-it
# List of additional build dependencies
#BuildRequires:  gcc-c++ libxml2-devel
Version:        citversion
Distribution:   citdistribution
Release:        1
License:        GPL v2 or later
Source:         %{name}-%{version}-%{distribution}.tar.gz
Group:          Development/Libraries/Other
Summary:        COBOL-IT  COBOL-COMPILER. 32 and 64 Bits		

BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
This package is a clone of the CIsam data base
It is used by COBOL-IT as base VSAM file system

%prep
%setup -q -n %{name}-%{version}-%{distribution}

%build

# Assume that the package is built by plain 'make' if there's no ./configure.
# This test is there only because the wizard doesn't know much about the
# package, feel free to clean it up
# a simple "sh ./configure --prefix=/usr" should be enought
export RPMBUILDCOBOLITDIR=%buildroot
sh builddist.sh allpublicrpm 32
sh builddist.sh allpublicrpm 64

%install

# Write a proper %%files section and remove these two commands and
# the '-f filelist' option to %%files
echo '%%defattr(-,root,root)' >filelist
find %buildroot -printf '/%%P*\n' >>filelist


%clean
rm -rf %buildroot

%files -f filelist
%defattr(-,root,root)

# This is a place for a proper filelist:
# /usr/bin/vbisam
# You can also use shell wildcards:
# /usr/share/vbisam/*
# This installs documentation files from the top build directory
# into /usr/share/doc/...
# %doc README COPYING
# The advantage of using a real filelist instead of the '-f filelist' trick is
# that rpmbuild will detect if the install section forgets to install
# something that is listed here



        
	