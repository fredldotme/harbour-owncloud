# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.27
# 

Name:       harbour-owncloud

# >> macros
%define debug_package %{nil}
%define __requires_exclude ^libqwebdav|libharbourowncloudcommon.*|libnemonotifications-qt5.*$
%define __provides_exclude_from ^%{_datadir}/.*$
# << macros

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    ownCloud
Version:    0.9.5
Release:    1
Group:      Qt/Qt
License:    GPLv2
URL:        https://github.com/fredldotme/harbour-owncloud
Source0:    %{name}-%{version}.tar.bz2
Source100:  harbour-owncloud.yaml
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(nemotransferengine-qt5)
BuildRequires:  desktop-file-utils

%description
Unofficial NextCloud/ownCloud client for SailfishOS

%package daemon
Summary:   NextCloud/ownCloud background task for automatic camera backups
Requires:  harbour-owncloud = %{version}

%description daemon
Nextcloud/ownCloud background task for automatic camera backups

%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
# << build pre

%qtc_qmake5 CONFIG+=sailfish_build

%qtc_make %{?_smp_mflags}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install
rm -r %{buildroot}%{_datadir}/%{name}/qml

# >> install post
mkdir -p %{buildroot}/usr/lib/systemd/user/user-session.target.wants
ln -sf /usr/lib/systemd/user/%{name}-daemon.service %{buildroot}/usr/lib/systemd/user/user-session.target.wants/%{name}-daemon.service
ln -sf /usr/lib/systemd/user/%{name}-permission-agent.service %{buildroot}/usr/lib/systemd/user/user-session.target.wants/%{name}-permission-agent.service
# << install post

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png

%files daemon
%defattr(755,root,root,-)
%{_bindir}/%{name}-daemon
%{_bindir}/%{name}-permission-agent
%defattr(-,root,root,-)
/usr/lib/systemd/user/%{name}-daemon.service
/usr/lib/systemd/user/%{name}-permission-agent.service
/usr/lib/systemd/user/user-session.target.wants/%{name}-daemon.service
/usr/lib/systemd/user/user-session.target.wants/%{name}-permission-agent.service
%{_datadir}/nemo-transferengine/plugins/
%{_libdir}/nemo-transferengine/plugins/libghostcloudshareplugin.so
%{_libdir}/qt5/qml/com/github/beidl/harbourowncloud/libharbourowncloudqmlplugin.so
%{_libdir}/qt5/qml/com/github/beidl/harbourowncloud/qmldir
%{_datadir}/themes/sailfish-default/meegotouch/z1.0/icons/icon-m-share-%{name}.png
%{_datadir}/themes/sailfish-default/meegotouch/z1.25/icons/icon-m-share-%{name}.png
%{_datadir}/themes/sailfish-default/meegotouch/z1.5-large/icons/icon-m-share-%{name}.png
%{_datadir}/themes/sailfish-default/meegotouch/z1.75/icons/icon-m-share-%{name}.png

# >> files
# << files

%pre daemon
/bin/systemctl-user stop harbour-owncloud-permissiond.service >/dev/null 2>&1 || :

%post daemon
rm -f /home/nemo/.config/systemd/user/lipstick.service.wants/harbour-owncloud-daemon.service >/dev/null 2>&1 || :
rm -f /home/nemo/.config/systemd/user/lipstick.service.wants/harbour-owncloud-permission.service >/dev/null 2>&1 || :
/bin/systemctl-user daemon-reload >/dev/null 2>&1 || :
/bin/systemctl-user restart harbour-owncloud-daemon.service >/dev/null 2>&1 || :
/bin/systemctl-user restart harbour-owncloud-permission-agent.service >/dev/null 2>&1 || :
