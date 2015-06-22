Name:       efl-extension
Summary:    EFL extension library
Version:    0.2.4
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        http://www.tizen.org/
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-x)
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
BuildRequires:  eo-devel
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

%if "%{?tizen_profile_name}" == "wearable"
cmake \
   . -DCMAKE_INSTALL_PREFIX=/usr -DTIZEN_PROFILE_NAME="wearable"
%else
cmake \
   . -DCMAKE_INSTALL_PREFIX=/usr -DTIZEN_PROFILE_NAME="default"
%endif

make %{?jobs:-j%jobs}


%install
%make_install

mkdir -p %{buildroot}/%{_datadir}/license
cp %{_builddir}/%{buildsubdir}/LICENSE %{buildroot}/%{_datadir}/license/%{name}


%post -p /sbin/ldconfig


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

%files devel
%defattr(-,root,root,-)
%{_includedir}/efl-extension/*.h
%{_libdir}/*.so
%{_libdir}/pkgconfig/efl-extension.pc
%if "%{?tizen_profile_name}" == "wearable"
    %{_includedir}/efl-extension/circle/*.h
    %{_includedir}/efl-extension/default/*.h
%else
    %{_datadir}/eolian/include/efl-extension/*.eo
%endif
