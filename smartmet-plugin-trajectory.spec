%define DIRNAME trajectory
%define SPECNAME smartmet-plugin-%{DIRNAME}
Summary: SmartMet trajectory plugin
Name: %{SPECNAME}
Version: 21.1.14
Release: 1%{?dist}.fmi
License: FMI
Group: SmartMet/Plugins
URL: https://github.com/fmidev/smartmet-plugin-trajectory
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: rpm-build
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: boost169-devel
BuildRequires: libconfig
BuildRequires: smartmet-library-spine-devel >= 21.1.14
BuildRequires: smartmet-library-trajectory-devel >= 21.1.14
BuildRequires: smartmet-library-macgyver-devel >= 21.1.14
BuildRequires: smartmet-engine-geonames-devel >= 21.1.14
BuildRequires: smartmet-engine-querydata-devel >= 21.1.14
BuildRequires: smartmet-library-smarttools-devel
BuildRequires: bzip2-devel
Requires: libconfig
Requires: smartmet-library-macgyver >= 21.1.14
Requires: smartmet-library-trajectory >= 21.1.14
Requires: smartmet-engine-geonames >= 21.1.14
Requires: smartmet-engine-querydata >= 21.1.14
Requires: smartmet-server >= 21.1.14
Requires: smartmet-trajectory-formats > 16.1.17
Requires: smartmet-library-spine >= 21.1.14
%if 0%{rhel} >= 7
Requires: boost169-date-time
Requires: boost169-iostreams
Requires: boost169-thread
%endif
Provides: %{SPECNAME}
Obsoletes: smartmet-brainstorm-trajectory < 16.11.1
Obsoletes: smartmet-brainstorm-trajectory-debuginfo < 16.11.1

%description
SmartMet trajectory plugin

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}
 
%build -q -n %{SPECNAME}
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0775,root,root,0775)
%{_datadir}/smartmet/plugins/%{DIRNAME}.so

%changelog
* Thu Jan 14 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.14-1.fmi
- Repackaged smartmet to resolve debuginfo issues

* Tue Oct  6 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.10.6-1.fmi
- Enable sensible relative libconfig include paths

* Wed Sep 23 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.23-1.fmi
- Use Fmi::Exception instead of Spine::Exception

* Fri Aug 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.21-1.fmi
- Upgrade to fmt 6.2

* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgraded to Boost 1.69

* Thu Oct 31 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.10.31-1.fmi
- Rebuilt due to newbase API/ABI changes

* Thu Sep 26 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.2.26-1.fmi
- Repackaged due to ABI changes

* Thu Feb 21 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.2.21-1.fmi
- Improved error reporting

* Mon Nov 12 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.11.12-1.fmi
- Use TemplateFactory for thread safety

* Sat Apr  7 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.4.7-1.fmi
- Upgrade to boost 1.66

* Tue Mar 20 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.3.20-1.fmi
- Full recompile of all server plugins

* Mon Aug 28 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.8.28-1.fmi
- Upgrade to boost 1.65

* Wed Mar 15 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.15-1.fmi
- Recompiled since Spine::Exception changed

* Sat Feb 11 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.2.11-1.fmi
- Repackaged due to newbase API change

* Wed Jan  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.4-1.fmi
- Changed to use renamed SmartMet base libraries

* Wed Nov 30 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.30-1.fmi
- No installation for configuration

* Tue Nov  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.1-1.fmi
- Namespace changed

* Tue Sep  6 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.9.6-1.fmi
- New exception handler

* Tue Aug 30 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.8.30-1.fmi
- Base class API change
- Use response code 400 instead of 503

* Mon Aug 15 2016 Markku Koskela <markku.koskela@fmi.fi> - 16.8.15-1.fmi
- The init(),shutdown() and requestHandler() methods are now protected methods
- The requestHandler() method is called from the callRequestHandler() method

* Tue Jun 14 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.14-1.fmi
- Full recompile

* Thu Jun  2 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.2-1.fmi
- Full recompile

* Wed Jun  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.1-1.fmi
- Added graceful shutdown

* Mon Jan 18 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.18-1.fmi
- newbase API changed, full recompile

* Wed Nov 18 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.11.18-1.fmi
- SmartMetPlugin now receives a const HTTP Request

* Mon Oct 26 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.10.26-1.fmi
- Added proper debuginfo packaging

* Wed Aug 26 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.26-1.fmi
- Recompiled with latest newbase with faster parameter changing

* Mon Aug 24 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.24-1.fmi
- Recompiled due to Convenience.h API changes

* Tue Aug 18 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.18-1.fmi
- Recompile forced by brainstorm API changes

* Mon Aug 17 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.17-1.fmi
- Use -fno-omit-frame-pointer to improve perf use

* Fri Aug 14 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.14-1.fmi
- Recompiled due to string formatter changes

* Wed Apr 29 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.29-1.fmi
- Removed optionengine dependency

* Wed Apr 15 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.15-1.fmi
- newbase API changed

* Thu Apr  9 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.9-1.fmi
- newbase API changed

* Wed Apr  8 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.8-1.fmi
- Dynamic linking of smartmet libraries

* Tue Feb 24 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.2.24-1.fmi
- Recompiled due to newbase changes

* Thu Dec 18 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.12.18-1.fmi
- Recompiled due to spine API changes

* Thu Oct 30 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.10.30-1.fmi
- Improved vertical interpolation algorithms (newbase)

* Mon Sep  8 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.9.8-2.fmi
- Recompiled due to newbase API changes

* Mon Sep  8 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.9.8-1.fmi
- Recompiled due to geoengine API changes

* Wed May 14 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.14-1.fmi
- Use shared macgyver and locus libraries

* Tue May  6 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.6-1.fmi
- qengine API changed

* Mon Apr 28 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.4.28-1.fmi
- Full recompile due to large changes in spine etc APIs

* Tue Mar 18 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.3.18-1.fmi
- Choose maximum pressure for trajectory if given height is too low

* Thu Feb 27 2014 Tuomo Lauri <tuomo.lauri@fmi.fi> - 14.2.27-1.fmi
- Fixed the default template directory path

* Fri Feb 14 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.2.14-1.fmi
- Fixed to work near the ground for hybrid level data

* Mon Jan 20 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.1.20-1.fmi
- Added dependency to smartmet-trajectory-formats

* Thu Dec 12 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.12.12-2.fmi
- Fixed a typo in the configuration file (extude --> extrude)

* Thu Dec 12 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.12.12-1.fmi
- Added height and heightrange options

* Mon Nov 25 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.25-1.fmi
- Updated Locus library

* Tue Nov  5 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.5-1.fmi
- Major release

* Tue Oct 29 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.10.29-1.fmi
- Fixed randomization of trajectories on Linux

* Thu Oct 24 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.10.24-1.fmi
- First version based on the qdtrajectory command line tool
