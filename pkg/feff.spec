Name:		feff
Version:	1.10.1
Release:	%mkrel 1
Summary:	Front-end for FFmpeg
Group:		Video
License:	GPLv3+
URL:		https://bitbucket.org/admsasha/feff/
Source0:	https://bitbucket.org/admsasha/feff/downloads/%{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Widgets)
Requires:	ffmpeg

%description
FeFF is a front-end for FFmpeg written in Qt.

%prep
%setup -q

%build
%qmake_qt5
%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}

%files
%doc COPYING
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
