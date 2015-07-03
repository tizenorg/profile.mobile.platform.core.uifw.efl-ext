Name:       efl-extension
Summary:    EFL extension library
Version:    0.2.8
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        http://www.tizen.org/
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xi)
BuildRequires:  pkgconfig(inputproto)
BuildRequires:  pkgconfig(fontconfig)
BuildRequires:  gettext
BuildRequires:  cmake
BuildRequires:  pkgconfig(cairo)
BuildRequires:  eolian-devel
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
EFL extension library


%package devel
Summary:    EFL extensiona library (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}


%description devel
EFL extension library providing small utility functions (devel)


%prep
%setup -q


%build
export CFLAGS+=" -fvisibility=hidden -fPIC -Wall"
export LDFLAGS+=" -fvisibility=hidden -Wl,-z,defs -Wl,--hash-style=both -Wl,--as-needed"

%define PREFIX /opt/usr/apps/org.tizen.indicator_shm

%if "%{?tizen_profile_name}" == "wearable"
cmake \
. -DCMAKE_INSTALL_PREFIX=/usr -DTIZEN_PROFILE_NAME="wearable"
%else
  %if "%{?tizen_profile_name}" == "tv"
    cmake \
    . -DCMAKE_INSTALL_PREFIX=/usr -DTIZEN_PROFILE_NAME="tv"
  %else
    cmake \
    . -DCMAKE_INSTALL_PREFIX=/usr -DTIZEN_PROFILE_NAME="default"
  %endif
%endif

make %{?jobs:-j%jobs}

%install
%make_install

mkdir -p %{buildroot}/%{_datadir}/license
cp %{_builddir}/%{buildsubdir}/LICENSE %{buildroot}/%{_datadir}/license/%{name}

## systemd
mkdir -p %{buildroot}/usr/lib/systemd/user
mkdir -p %{buildroot}/usr/lib/systemd/user/core-efl.target.wants
mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants
ln -s ../indicator_shm.service %{buildroot}%{_libdir}/systemd/user/core-efl.target.wants/indicator_shm.service
ln -s ../indicator_shm.service %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants/indicator_shm.service

%post -p /sbin/ldconfig
echo "INFO: System should be restarted or execute: systemctl --user daemon-reload from user session to finish service installation."
%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_libdir}/libefl-extension.so.*
%{_datadir}/efl-extension/themes/*.edj
%{_datadir}/locale/*
%{_datadir}/license/%{name}
%manifest %{name}.manifest
%if "%{?tizen_profile_name}" == "wearable"
    %{_datadir}/efl-extension/images/*
%endif

%defattr(-,root,root,-)
%{PREFIX}/bin/indicator_shm
## systemd
%{_libdir}/systemd/user/indicator_shm.service
%{_libdir}/systemd/user/core-efl.target.wants/indicator_shm.service
%{_libdir}/systemd/system/indicator_shm.service
%{_libdir}/systemd/system/multi-user.target.wants/indicator_shm.service

%files devel
%defattr(-,root,root,-)
%{_includedir}/efl-extension/*.h
%{_libdir}/*.so
%{_libdir}/pkgconfig/efl-extension.pc
%if "%{?tizen_profile_name}" == "wearable"
    %{_includedir}/efl-extension/circle/*.h
    %{_includedir}/efl-extension/default/*.h
%else
  %if "%{?tizen_profile_name}" != "tv"
    %{_datadir}/eolian/include/efl-extension/*.eo
  %endif
%endif
