Name:       harbour-blocky

# >> macros
%global _missing_build_ids_terminate_build 0
%define blocky_version 0.23
# << macros

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    blocky
Version:    0.0.5
Release:    1
Group:      Qt/Qt
License:    GPLv3
URL:        https://scarpino.dev
Source0:    %{name}-%{version}.tar.bz2
%ifarch %arm
Source1: blocky_v%{blocky_version}_Linux_armv7.tar.gz
%endif
%ifarch aarch64
Source1: blocky_v%{blocky_version}_Linux_arm64.tar.gz
%endif
Source2:    blocky.service
Source3:    blocky.yaml
Source4:    connman.override.conf
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils

%description

A DNS proxy and ad-blocker for the local network

%prep
%setup -q -n %{name}-%{version}

#%setup -a 1 doesn't work in mb2
if [ ! -f %{_sourcedir}/blocky ]; then
  tar xf %{SOURCE1} -C %{_sourcedir}
fi

# >> setup
# << setup

%build
# >> build pre
# << build pre

%qtc_qmake5

%qtc_make %{?_smp_mflags}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install

# >> install post
install -d %{buildroot}%{_unitdir}
install -m644 %{SOURCE2} %{buildroot}%{_unitdir}

install -m755 %{_sourcedir}/blocky %{buildroot}%{_bindir}

install -Dm644 %{SOURCE3} %{buildroot}%{_sysconfdir}/blocky.yaml
install -Dm644 %{SOURCE3} %{buildroot}%{_datadir}/%{name}/blocky.yaml

install -d %{buildroot}%{_sysconfdir}/systemd/system/connman.service.d
install -Dm644 %{SOURCE4} %{buildroot}%{_sysconfdir}/systemd/system/connman.service.d/override.conf

install -d %{buildroot}%{_sysconfdir}/tmpfiles.d
touch %{buildroot}%{_sysconfdir}/tmpfiles.d/connman_resolvconf.conf

# << install post

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%attr(4755,root,root) %{_bindir}/%{name}
%{_bindir}/blocky
%{_datadir}/%{name}
%config(noreplace) %{_sysconfdir}/blocky.yaml
%{_datadir}/%{name}/blocky.yaml
%{_sysconfdir}/systemd/system/connman.service.d/override.conf
%{_sysconfdir}/tmpfiles.d/connman_resolvconf.conf
%{_unitdir}/blocky.service
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
# >> files
# << files

%post
systemctl daemon-reload
if [ $1 -eq 1 ]; then
systemctl enable blocky.service
systemctl restart connman.service
rm /etc/resolv.conf
echo "nameserver 127.0.0.1" > /etc/resolv.conf
systemctl-user restart booster-browser@sailfish-browser booster-browser@jolla-email
fi
systemctl restart blocky.service

%preun
if [ $1 -eq 0 ]; then
  systemctl stop blocky.service
  systemctl disable blocky.service
fi

%postun
if [ $1 -eq 0 ]; then
  systemctl daemon-reload
  rm /etc/resolv.conf
  systemctl restart connman.service
  systemd-tmpfiles --create
fi
