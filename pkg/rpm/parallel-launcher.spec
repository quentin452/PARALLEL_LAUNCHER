Name:		parallel-launcher
Version:	GITLAB_CI_VERSION
Release:	0%{?dist}
Summary:	Modern N64 Emulator
Summary(es):	Un emulador moderno de N64
Summary(de):	Moderner N64-Emulator
Summary(fr):	Emulateur N64 Moderne
Summary(it):	Moderno Emulatore di N64
Summary(pt):	Emulador Moderno do N64
Summary(ar):	محاكي N64 حديث
License:	GPLv3
Group:		games
URL:		https://parallel-launcher.ca
Source:		https://gitlab.com/parallel-launcher/parallel-launcher/-/archive/GITLAB_CI_TAG/parallel-launcher-GITLAB_CI_TAG.tar.bz2
Vendor:		Matt Pharoah

BuildRequires: (gcc >= 9 or gcc9 or gcc10 or gcc11 or gcc12 or gcc13 or gcc-toolset-9-gcc or gcc-toolset-10-gcc or gcc-toolset-11-gcc or gcc-toolset-12-gcc or gcc-toolset-13-gcc)
BuildRequires: (gcc-c++ >= 9 or gcc9-c++ or gcc10-c++ or gcc11-c++ or gcc12-c++ or gcc13-c++ or gcc-toolset-9-gcc-c++ or gcc-toolset-10-gcc-c++ or gcc-toolset-11-gcc-c++ or gcc-toolset-12-gcc-c++ or gcc-toolset-13-gcc-c++)
BuildRequires: ((libqt5-qtbase-common-devel and libQt5DBus-devel) or ((qt5-qtbase-devel or qtbase5-common-devel) and lib64qt5dbus-devel) or qt5-qtbase-devel)
BuildRequires: (libqt5-qtdeclarative-devel or qt5-qtdeclarative-devel or lib64qt5declarative-devel or qtdeclarative5)
BuildRequires: (libSDL2-devel or SDL2-devel or lib64SDL2-devel)
BuildRequires: (sqlite3-devel or sqlite-devel or lib64sqlite3-devel)
BuildRequires: libgcrypt-devel
BuildRequires: (((qt5-qtsvg-devel or libqt5-qtsvg-devel) and (qt5-linguist or libqt5-linguist)) or (lib64qt5svg-devel and (qt5-linguist-tools or qttools5) and lib64qt5network-devel and which))

Requires: glibc
Requires: (libstdc++6 or libstdc++)
Requires: (libgcc_s1 or libgcc)
Requires: ((libQt5Core5 and libQt5Widgets5 and libQt5Network5 and libQt5Svg5 and libQt5DBus5) or (lib64qt5core5 and lib64qt5widgets5 and lib64qt5network5 and lib64qt5svg5 and lib64qt5dbus5) or (qt5-qtbase and qt5-qtbase-common and qt5-qtdeclarative and qt5-qtsvg and (qt5-qdbus or qt5-qttools)))
Requires: (libQt5Gui5 or qt5-qtbase-gui or lib64qt5gui5)
Requires: findutils
Requires: xdg-utils
Requires: polkit
Requires: (libsqlite3-0 or sqlite-libs or lib64sqlite3)
Requires: (libgcrypt or libgcrypt20)
Requires: dosfstools
Requires: (coreutils or coreutils-common)
Requires: p7zip

Recommends: (google-noto-sans-fonts or noto-sans-fonts)
Recommends: (google-noto-sans-mono-fonts or noto-mono-fonts)
Recommends: udisks2

%define debug_package %{nil}

%description
Parallel Launcher is an emulator launcher that aims to make playing N64 games,
both retail and homebrew, as simple and as accessible as possible. Parallel
Launcher uses the RetroArch emulator, but replaces its confusing menus and
controller setup with a much simpler user interface. It also features optional
integration with romhacking.com.

%description -l es
Parallel Launcher es un lanzador para un emulador hecho para jugar a juegos de
N64, tanto comerciales como homebrew, de la forma más simple y accesible
posible. Parallel Launcher usa el emulador RetroArch, pero reemplaza sus menús
confusos y su configuración de mandos con una interfaz de usuario mucho más
simple. También tiene integración opcional con romhacking.com.

%description -l de
Parallel Launcher ist ein Emulator-Launcher der es einfach macht, N64-Spiele
(sowohl Original als auch Mods) zu spielen. Parallel Launcher benutzt den
RetroArch-Emulator, jedoch sind dessen verwirrende Menüs mit einem wesentlich
einfacheren Interface ersetzt. Außerdem ist eine Integration mit romhacking.com
möglich.

%description -l fr
Parallel Launcher est un gestionnaire d'émulateur qui vise à rendre le
lancement de jeux N64, en version de base ou en homebrew, de la manière la plus
simple et accessible possible. Parallel Launcher utilise l'émulateur RetroArch,
mais remplace ses menus et son gestionnaire de manettes par une interface
utilisateur plus simple à utiliser. Contient une intégration optionnelle à
Romhacking.com.

%description -l it
Parallel Launcher è un applicazione che ha lo scopo di emulare e quindi
permettere di giocare ai giochi di N64, sia commerciali che homebrew, nella
maniera piú semplice e accessibile. Parallel Launcher usa l'emulatore
Retroarch, ma rimpiazza i  suoi confusi menú e la configurazione dei controller
con un'interfaccia utente piú intuitiva. Include anche un'integrazione
facoltativa con romhacking.com.

%description -l pt
O Parallel Launcher é um inicializador de emuladores que tem como objetivo
tornar a jogabilidade de jogos N64, sejam comerciais ou homebrew, o mais
simples e acessível possível. O Parallel Launcher usa o emulador RetroArch, mas
substitui os seus menus confusos e as suas configurações de comandos por uma
interface de usuário muito mais simples. Também possui integração opcional com
o romhacking.com.

%description -l ar
 برنامج Parallel Launcher عبارة عن مشغل محاكي يهدف إلى جعل ممارسة ألعاب N64، 
 سواء التجزئة أو إلصناعة إلمنزلية، أمرًا بسيطًا وسهل المنال إلي قدر الإمكان.
 يستخدم Parallel Launcher محاكي RetroArch، ولكنه يستبدل إلقوائم وإعدادات وحدة ا
 لتحكم الخاصة به المربكة، بواجهة مستخدم أبسط بكثير. كما يتميز أيضًا بالتكامل
 الاختياري مع romhacking.com.

%prep
%setup -q

%build
if [ `g++ -dumpversion` -lt 9 ]; then
	if [ `which g++-13` ]; then
		qmake-qt5 app.pro -spec linux-g++ QMAKE_CXX="g++-13"
	elif [ `which g++-12` ]; then
		qmake-qt5 app.pro -spec linux-g++ QMAKE_CXX="g++-12"
	elif [ `which g++-11` ]; then
		qmake-qt5 app.pro -spec linux-g++ QMAKE_CXX="g++-11"
	elif [ `which g++-10` ]; then
		qmake-qt5 app.pro -spec linux-g++ QMAKE_CXX="g++-10"
	elif [ `which g++-9` ]; then
		qmake-qt5 app.pro -spec linux-g++ QMAKE_CXX="g++-9"
	else
		echo "WARNING: g++ is out-of-date. You may need to update."
		qmake-qt5 app.pro -spec linux-g++
	fi
else
	qmake-qt5 app.pro -spec linux-g++
fi
if [ `which lrelease-qt5` ]; then
	lrelease-qt5 app.pro
else
	lrelease app.pro
fi
make -j `nproc`

%install
install -D parallel-launcher %{buildroot}/usr/bin/parallel-launcher
install -D ca.parallel_launcher.ParallelLauncher.desktop %{buildroot}/usr/share/applications/ca.parallel_launcher.ParallelLauncher.desktop
install -D ca.parallel_launcher.ParallelLauncher.metainfo.xml %{buildroot}/usr/share/metainfo/ca.parallel_launcher.ParallelLauncher.metainfo.xml
install -D data/appicon.svg %{buildroot}/usr/share/icons/hicolor/scalable/apps/ca.parallel_launcher.ParallelLauncher.svg
install -D bps-mime.xml %{buildroot}/usr/share/parallel-launcher/bps-mime.xml
install -D lang/parallel-launcher_en_us.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_en_us.qm
install -D lang/parallel-launcher_en_gb.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_en_gb.qm
install -D lang/parallel-launcher_it.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_it.qm
install -D lang/parallel-launcher_es.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_es.qm
install -D lang/parallel-launcher_fr_ca.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_fr_ca.qm
install -D lang/parallel-launcher_fr.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_fr.qm
install -D lang/parallel-launcher_de.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_de.qm
install -D lang/parallel-launcher_ar.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_ar.qm
install -D lang/parallel-launcher_pt.qm %{buildroot}/usr/share/parallel-launcher/translations/parallel-launcher_pt.qm
install -D parallel-launcher-sdl-relay %{buildroot}/usr/share/parallel-launcher/parallel-launcher-sdl-relay
install -D parallel-launcher-lsjs %{buildroot}/usr/share/parallel-launcher/parallel-launcher-lsjs

%files
/usr/bin/parallel-launcher
/usr/share/applications/ca.parallel_launcher.ParallelLauncher.desktop
/usr/share/metainfo/ca.parallel_launcher.ParallelLauncher.metainfo.xml
/usr/share/icons/hicolor/scalable/apps/ca.parallel_launcher.ParallelLauncher.svg
/usr/share/parallel-launcher/bps-mime.xml
/usr/share/parallel-launcher/translations/parallel-launcher_en_us.qm
/usr/share/parallel-launcher/translations/parallel-launcher_en_gb.qm
/usr/share/parallel-launcher/translations/parallel-launcher_it.qm
/usr/share/parallel-launcher/translations/parallel-launcher_es.qm
/usr/share/parallel-launcher/translations/parallel-launcher_fr_ca.qm
/usr/share/parallel-launcher/translations/parallel-launcher_fr.qm
/usr/share/parallel-launcher/translations/parallel-launcher_de.qm
/usr/share/parallel-launcher/translations/parallel-launcher_ar.qm
/usr/share/parallel-launcher/translations/parallel-launcher_pt.qm
/usr/share/parallel-launcher/parallel-launcher-sdl-relay
/usr/share/parallel-launcher/parallel-launcher-lsjs

%post
if [ $1 -eq 1 ] ; then
	xdg-mime install /usr/share/parallel-launcher/bps-mime.xml 2> /dev/null || true
	xdg-mime default ca.parallel_launcher.ParallelLauncher.desktop x-scheme-handler/rhdc
	update-desktop-database /usr/share/applications 2> /dev/null || true
fi
exit 0

%postun
if [ $1 -eq 0 ] ; then
	xdg-mime uninstall /usr/share/parallel-launcher/bps-mime.xml 2> /dev/null || true
	update-desktop-database /usr/share/applications 2> /dev/null || true
fi
if [ $1 -eq 1 ] ; then
	rm -f /etc/flatpak/installations.d/parallel-launcher.conf
fi
exit 0

%changelog
* Sun May 26 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.7.0
- Enabled error logging in RetroArch
- When RetroArch crashes shortly after starting, the error logs can now be viewed in the error dialog
- Worked around a Windows bug that prevented SD card mounting from working if file compression is enabled
* Wed May 22 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.6.0
- Added a x9 option for window scale
- Fixed the application icon not showing up on Linux when using Wayland
- Fixed the Open Containing Folder option not working on Windows if the rom path contains non-ASCII characters
- Fixed a rare application freeze on Windows if IS Viewer or Parallel Launcher integration fails to connect to the emulator in a specific way
- Updated the minimum allowed version of the ParallelN64 core to ensure you have the latest security updates
* Sun Apr 16 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.5.3
- Fixed a crash on Windows when trying to automatically import a PJ64 save file if the internal rom name is neither valid Shift-JIS nor valid UTF-8
- Improved parsing of internal rom names to ensure that the parsed result is always a valid UTF-8 string even if the internal rom name is not correctly encoded
* Sun Apr 15 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.5.2
- Fixed a major regression that prevented RHDC hacks with star layouts from being downloaded in Parallel Launcher
* Sat Apr 14 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.5.1
- Fixed issues loading roms with non-ASCII characters in their filename on Windows
- Fixed inability to import PJ64 save files on Windows if the internal rom name contains Japanese characters
- Parallel Launcher now checks for star layout updates when launching a rom using the Play Now button on the RHDC website
- Resetting your RetroArch config in Parallel Launcher settings will now also purge the shader cache
- Fixed audio crackling on some systems when playing on a monitor with a refresh rate higher than 60Hz
- Fixed a crash if the stored RHDC credentials somehow become corrupted
* Thu Apr 5 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.5.0
- Added Fixed Shuffle and Random Shuffle sorting methods to the RHDC view
- Fixed a crash when launching roms or going to controller configuration if the SDL driver helper is missing
- Fixed an oversight where Parallel Launcher would always disable video shaders on startup
- Fixed default controller profile for the Gamecube having incorrect mappings on SDL
- Fixed the star layout tester and save file editor not being resizable
* Wed Mar 28 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.4.0
- Added a "secret" debugging feature: pressing F8 will now open a dialog showing previous crash logs (only crashes from this version onwards are logged)
- Fixed a bug where OGRE and GLideN64 with framebuffer emulation disabled would not work correctly on certain GPUs
* Mon Mar 5 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.3.0
- Updated RHDC hack rating from Parallel Launcher to reflect the new Romhacking.com rating system
- Updated RHDC integration to use the latest APIs
- Improved performance of OGRE and GLideN64 cores when not using framebuffer emulation
* Tue Feb 21 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.2.0
- Disabled anti-aliasing in the GLideN64 plugin by default due to graphical bugs that appear when it is enabled. You can re-enable GLideN64 anti-aliasing in your Parallel Launcher settings if you want to
- Updated the SDL2 input driver on Windows (v2.0.10 -> v2.30.0)
- Updated French translations
- Fixed an issue where old crash reports would be repeatedly sent on startup if Parallel Launcher crashed while being run with elevated permissions
* Wed Feb 15 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.1.0
- Added an LLE RSP option to the GLideN64 plugin. This allows homebrew/romhacks with custom microcode to be run on GLideN64 instead of just ParaLLEl and Angrylion; however, due to inaccuracies with GLideN64 when using this option, ParaLLEl is still recommended for these hacks.
* Thu Jan 26 2024 Matt Pharoah <mtpharoah@gmail.com> - 7.0.0
- The non-flatpak Linux version of Parallel Launcher no longer uses a Flatpak installation to install and update an isolated RetroArch. Instead, it now simply uses an appimage.
- Fixed a bug where the Windows and Flatpak versions of Parallel Launcher could sometimes assign the wrong controller to RetroArch when multiple controllers are connected
- Fixed a rare crash on Windows when refreshing your rom list under certain conditions
* Thu Jan 5 2024 Matt Pharoah <mtpharoah@gmail.com> - 6.22.6
- When using the Angrylion RDP (graphics) plugin, the RSP is now emulated with parallel-rsp instead of cxd4
- Fixed a bug where the "(recommended by hack author)" text would not show up beside GLideN64 for RHDC hacks that recommend that plugin
* Mon Dec 12 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.22.5
- Fixed a rare crash when downloading a hack from RHDC
* Mon Dec 5 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.22.3
- Fixed a bug that prevented hacks being downloaded from RHDC if their filename contained certain characters
- Fixed a bug where hack versions that were deleted from RHDC could still show up but could not be downloaded
* Mon Oct 3 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.22.0
- Added an option to browse and edit files on virtual SD cards
- Added an option to clone virtual SD cards
- Fixed SM64 save file editing, star layouts, and star progress not working correctly on romhacking that use SRAM or FlashRAM
- Fixed play time not always syncing correctly between different versions of the same RHDC hack
- Fixed an issue that could result in save files appearing to be lost due to RetroArch putting save files in an unexpected place
- Fixed a bug where SD cards could not be imported if no other SD cards existed yet
- Fixed a bug where hacks supporting N64 mouse input would not be detected as such if they were first discovered by Parallel Launcher in a version earlier than 6.2.0
- Enabled bilinear filtering when playing in fullscreen or using stretched widescreen
- Added an emulator extension to allow romhacks to create or request an SD card
* Fri Aug 12 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.21.0
- You can now set the analog to digital button press threshold on your controller profiles, allowing you to change how far down you need to press a trigger or how far you need to move a control stick for it to be considered a button press
- Changed the default value for the aforementioned threshold from 50% to 70%
- When playing a romhacks that supports the N64 mouse peripheral, the mouse cursor is now automatically captured by default
- Fixed a bug that caused the main Parallel Launcher window to not size itself correctly
* Mon Aug 1 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.20.0
- Added an emulator extension that allows roms to display your username and avatar when romhacking.com integration is enabled
- Fixed a bug where internal rom names containing a dakuten or handakuten are not decoded correctly
* Wed Jun 15 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.19.0
- Added the ability to import SD cards from ISO files
- You can now set the time on the N64 realtime clock, and optionally have it roll back the clock when loading a savestate. (Settings -> System Clock)
- When using Open Containing Folder, the rom file will now be selected in the opened file browser
- Added a Show Save File option to the context menu for roms which opens the save directory in your file browser and selects the save file
- Added a Data Directories submenu to the ≡ main menu where you can open your save directory among other things
- You will no longer be fully signed out of romhacking.com integration if you open Parallel Launcher with no internet connection
- Fixed a crash if the rom list updates after right clicking on a rom but before clicking an option on the opened context menu
* Tue May 24 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.18.2
- Fixed SD cards larger than 2 GiB not showing up
- Fixed a crash when creating an SD card with no filesystem
- Fixed some windows and dialogs being smaller than intended on high DPI displays
* Sat May 21 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.18.1
- Fixed a bug that would cause you to be signed out of RHDC integration when launching Parallel Launcher from the Play Now button on the RHDC website
- Updated Québécois French translations
- Added missing Portuguese translations
* Mon May 16 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.18.0
- Added support for emulating the SD card interface on the SummerCart64 flash cart (thanks to devwizard for implementing this in the ParallelN64 emulator core)
- Fixed a semi-rare bug in the RHDC integration that could cause you to suddenly be signed out of your RHDC account
- Fixed a very rare crash that can happen when refreshing your list of ROMs or starting the program
* Mon May 2 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.17.0
- Added a search bar to both the classic and RHDC views
- Added IS Viewer support to help in developing romhacks or homebrew. It can be enabled under the new Developer section of the Settings dialog.
- Fixed a bug where new thumbnails failed to load in the RHDC view
* Mon Apr 25 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.16.1
- Fixed a bug that would cause users to be signed out of RHDC integration after about a week
* Tue Apr 19 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.16.0
- Added Portuguese translations (thanks to Lila)
- RHDC integration: support accounts with multi-factor authentication
* Fri Mar 18 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.15.2
- Properly handle archived hack versions in the RHDC integration
* Sun Mar 13 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.15.1
- Fixed star layouts failing to download for RHDC hacks
* Sun Mar 13 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.15.0
- Updating Romhacking.com iconography
- Update to latest Romhacking.com API
- Fixed blurriness on MacOS when using a high DPI display
* Thu Jan 20 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.14.1
- Fixed a bug where the star display window could be larger than the desktop, making some content offscreen and inaccessible.
* Thu Jan 20 2023 Matt Pharoah <mtpharoah@gmail.com> - 6.14.0
- When playing on an ARM Mac, the ARM build of ParallelN64 will now be used instead of needing to emulate the x86 build.
* Wed Dec 15 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.13.0
- When playing a hack in fullscreen with Widescreen (Viewport Hack) on, an aspect ratio of 16:10 will now be used instead of 16:9 if that more closely matches the aspect ratio of your monitor.
- Similarly, the SteamDeck version will no longer use a 16:10 aspect ratio if it is connected to an external display with a different aspect ratio.
- Fixed a bug where the Tab key could not be bound to a hotkey or virtual controller input
- Updated the RetroArch build in the Flatpak version of Parallel Launcher
* Sat Oct 23 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.12.3
- Fixed a bug where removing the last group in the Manage Groups dialog and then clicking on Delete or Rename would result in a crash
- Fixed a potential crash when updating emulator cores under certain conditions
- Fixed a potential rare crash when the sqlite database is busy
- Updated the sqlite library in the Windows version
- Include debug symbols in the Linux version so that crash logs can include a backtrace
* Sun Oct 17 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.12.2
- Reverted the previous vsync change in the Windows version as it was causing issues for a few Windows users
* Sun Oct 17 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.12.1
- The vsync setting is now enabled by default
* Tue Aug 10 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.12.0
- The ParaLLEl plugin is now supported on MacOS (if your hardware supports it)
- Updates specific to the MacOS version of Parallel Launcher will no longer require the Windows version to be updated, and vice versa
* Mon Aug 9 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.11.1
- Fixed OGRE modifications being applied when using GLideN64.
* Thu Jul 29 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.11.0
- Changed the default audio driver on Windows to DirectSound because XAudio2 was causing emulation to run muted at uncapped speed for a small percentage of Windows users.
- The audio driver can now be set in Parallel Launcher settings under the Emulation tab in the Windows version.
* Thu Jul 15 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.10.0
- Changed the controller selection dialog when starting a singleplayer game with multiple controllers detected. You now only need to press any button on the controller you wish to use instead of having to select it from a list.
- Added a metainfo file to the Linux packages for better integration with GUI package managers
- Fixed a crash on the controller setup dialog when rapidly connecting and disconnecting a controller while using the JoyDev driver.
* Sun Jun 20 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.9.4
- Fixed misaligned icon and text for hack difficulty in the RHDC view
- Fixed the window sometimes resizing itself vertically even when it doesn't actually need any more space
- Fixed the plugin selection widget not resizing itself correctly when changing emulators
- Fixed Québécois French not showing up the language selection
* Sun Jun 13 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.9.3
- Fixed crashes that can occur when multiple instances of Parallel Launcher are open at the same time
- Fixed a crash when loading roms if the OS returns an error code when fetching the size of a rom file
- Fixed a crash when clicking on the New/Edit Profile button in the controller select dialog if you somehow manage to have no controller profile selected
- Fixed a bug where your selected controller profile for an input device could be reset when changing input drivers
* Tue Jun 8 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.9.2
- Fixed a crash that can occur when changing the controller input driver
- Fixed long author lists causing the RHDC view window to expand very wide
- Fixed the crash reporter not giving useful stack traces on Linux
* Fri Jun 4 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.9.1
- Fixed a crash when changing the RHDC download folder if the previous download location no longer exists
- Fixed incorrect icon scaling for high DPI screens on MacOS
- Fixed a missing maximize button on the ROM Sources window on Windows
- [Telemetry] Fixed Windows 11 reporting itself as Windows 10
- [Telemetry] Fixed the OS name and version not being reported correctly in the Flatpak version
* Mon May 31 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.9.0
- Added Arabic translations (thanks to Elazul for the translations)
- Fixed missing translations for the input select dropdown
- Fixed input binding not working on some newer Linux distros when using the SDL2 driver
* Fri May 28 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.8.0
- Parallel Launcher now submits crash reports and basic telemetry data (no kind of personal information is ever sent)
* Tue May 25 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.7.0
- Save files can now be shared between different versions of the same RHDC hack. (Requires Romhacking.com integration to be enabled)
- Hacks can now be sorted by star count in the RHDC view
- Fixed a bug in the classic view where a group that was collapsed would expand itself again after a refresh if the currently selected rom is in the group
* Mon May 17 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.6.3
- Fixed a crash when launching Parallel Launcher by clicking on a rom file that hasn't yet been seen by Parallel Launcher and isn't from RHDC
* Sat May 15 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.6.2
- Fixed the Now Playing window always appearing in the top left corner
- Fixed the Now Playing window having its title bar offscreen on Windows
* Wed May 12 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.6.1
- Reverted to launching RetroArch with normal priority on Windows
* Sun May 9 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.6.0
- The "Widescreen (Viewport Hack)" option will now use 16:10 widescreen on the Steam Deck instead of 16:9, so it now fills the entire screen.
* Sat May 8 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.5.0
- Parallel Launcher will now always check romhacking.com for graphics plugin recommendations when finding new roms, even if RHDC Integration is disabled.
- Disabled integer scaling for the ParaLLEl plugin. It will now always scale to fill the window.
- Fixed the RHDC icon being tiny on high DPI displays
- RetroArch is now launched with a higher process priority on the Windows version
* Thu Apr 29 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.4.2
- Fixed the GoldenEye input mode mapping the triggers incorrectly
- Fixed icon-only buttons being too small on high dpi monitors
- Removed the warning when starting Parallel Launcher in gaming mode on the Steam Deck
* Sat Apr 24 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.4.1
- Fixed a bug where opening a .bps patch from an external file browser would create a corrupted rom entry in the Parallel Launcher classic view
- Removed the 'Automatically Added' category. Roms added by launching them from a file browser now just go into Uncategorized
- Fixed a bug where the controller deadzone and sensitivity settings were not being applied for users whose locale settings use a comma instead of a period for the decimal separator.
- Slightly changed the buttons in the controller selection and setup dialogs to be less confusing when creating a new profile from a default.
* Mon Apr 19 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.4.0
- Tweaked various things when playing on Steam Deck for a better user experience
- Changed the layout of the RHDC view to be more horizontal
- Enable Discord integration by default in the Flatpak version
- Renamed the ParallelN64 core file to ensure RetroArch will never overwrite it
- The Parallel Launcher window now has two separate sizes that it saves separately for the classic and RHDC views
- Fixed a bug where the RHDC view would not remember your last selected ROM on startup
- Fixed graphical bugs in the RHDC view when horizontally resizing the window
- Fixed the last few controller mappings going to the wrong inputs when using Quick Configure
* Sun Apr 18 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.3.0
- Added a Flatpak version of Parallel Launcher
- Tweaked some settings and behaviour when playing on Steam Deck for better usability
- Added a setting to start the emulator in fullscreen mode
* Sun Apr 4 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.2.1
- Fixed a crash on startup on Windows and MacOS if the UI theme saved in the settings file is no longer installed
* Tue Mar 30 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.2.0
- Added support for emulated gamecube controllers (thanks to Wiseguy for implementing this in the ParallelN64 core)
- Added N64 mouse support
- Fixed incorrect text for save slots in the SM64 save file editor
* Sat Mar 27 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.1.1
- Fixed the Remove Black Borders option showing up everywhere when it should only be visible for the ParaLLEl plugin on ParallelN64
- The Now Playing window now makes the star display scrollable so the window can be made smaller and will not be larger than the screen
- Fixed the display of rom internal names containing dakuten and/or handakuten
- Added an information bubble guiding the user to the RHDC view after RHDC integration is enabled
- Removed the windowsvista style from the Windows themes because it does not get styled correctly
- Removed the macintosh style from the MacOS themes because it does not get styled correctly
* Thu Mar 25 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.1.0
- Added a Dark Mode option to Windows and Mac
- Added missing French and German translations
- Updated ParallelN64 core
-  * Added an option to remove the black borders on the sides when using the ParaLLEl plugin
-  * Fixed a bug where the GLideN64 plugin would stop working for some Windows users when going fullscreen or disabling RetroArch notifications (thanks to aglab2 for this fix)
-  * Fixed incorrect rendering of PAL roms in GLideN64 when framebuffer emulation is enabled
* Mon Mar 22 2022 Matt Pharoah <mtpharoah@gmail.com> - 6.0.0
- Parallel Launcher now uses a custom build of the ParallelN64 core
-  * Fixed inaccuracies with the Taz Express ROM
-  * The EverDrive ROM header will now be respected for determining the save type of romhacks
-  * The GLideN64 plugin is now supported on ParallelN64, and some rendering bugs have been fixed. (Huge thanks to aglab2 for bringing this plugin to ParallelN64)
-  * A new graphics plugin has been added: OGRE (Offshoot GLideN64: Romhack Edition). This plugin is a modification (by aglab2) of the GLideN64 plugin that is designed to be compatible with older romhacks that don't work in the mainline version of GLideN64. This plugin should replace Glide64 and Rice in 95% of use cases.
- Added a setting to show advanced rom options
-  * The Overclock VI option is now an advanced option
-  * Added an option to force the use of 16kB EEPROM save data
-  * Added an option to use an interpreter core instead of the dynamic recompiler
- Disabled the ParaLLEl graphics plugin on MacOS (it never actually worked before and instead just loaded the Glide64 plugin when selected)
- The main window now has a smaller minimum size when in the classic view
- Fixed the Now Playing window sometimes being bigger than it should be
- Separated the French translations into Standard French and Québécois French (thanks to Naeso for the Standard French translations)
* Mon Mar 15 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.12.1
- Fixed rhdc:// links not always download the correct version of the hack
* Sat Mar 13 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.12.0
- Improved the advanced star layout format to support groups on the top and/or bottom that span both the left and right columns
- Fixed star layouts not showing up when testing a star layout for a hack that is not yet on romhacking.com
- Fixed star icons being cut off in the star layout tester window
- Fixed vertical misalignments in the star layout displays
- Fixed missing monospace font on Windows and Mac
* Fri Mar 12 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.11.0
- Added a joydev input driver to the Linux version
- Added a default SDL profile for Switch Pro controllers
- Fixed a bug with the star layout editor window suddenly growing unusably large on the Windows version because reasons
* Wed Mar 10 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.10.3
- Fixed controller names not being detected in the SDL driver on Windows
- Fixed rhdc:// links not launching on MacOS if Parallel Launcher is already open
* Sat Mar 6 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.10.2
- Fixed play time and progress not being submitted to romhacking.com when
		launching Parallel Launcher via an rhdc:// link
- Fixed core info files being redownloaded more often than they need to be
- Fixed the internal resolution being lower than it should be when using
		widescreen on the Mupen64plus-Next core
- Fixed the SDL input driver not working on the Windows version
- Fixed the MacOS version not being able to launch rhdc:// links
* Fri Mar 5 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.10.1
- Fixed some rhdc:// links not launching correctly
- Don't show the content loaded RetroArch notification
- Fixed savestates not working on RetroArch 1.10.1 on Linux
* Thu Mar 4 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.10.0
- Changed the 'Reload ROM' button text to 'Hard Reset' and made the button always visible
- Added an option to hide RetroArch notifications
- Added an 'Add Single ROM' action to the ≡ menu that allows for a rom or patch to be quickly added without going through the ROM Sources dialog
- Added a URL scheme handler that will allow for Parallel Launcher to be launched directly from romhacking.com (coming soon)
- Fixed savestates not working on RetroArch 1.10.1
- Fixed controller setup not working on Arch Linux if Parallel Launcher was installed via the AUR repo
* Mon Mar 1 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.9.2
- Fixed an error when attempting to install or update an emulator core on Windows if your Windows username contains non-ASCII characters
* Thu Feb 25 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.9.1
- Fixed RHDC romhacks failing to patch on Windows if the vanilla SM64 rom is located on an external drive
- Fixed the font on Windows and Mac to be consistent with the Linux version
- Fixed misaligned text in star layouts on the Windows version
- Fixed the font size on the MacOS version
- Fixed the font colour on selected hacks in the RHDC view on MacOS
- Fixed missing icons on dialog buttons in the MacOS version
- Made the MacOS version themeable and set the default theme to Fusion
* Sun Feb 21 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.9.0
- Added an option to use DirectInput8 as the controller input driver on Windows. If you have a controller that was not working correctly before, try switching to the DirectInput8 driver.
- Fixed SVG images in star layouts not loading on MacOS
- Ensure that the temporary directory is always cleared on shutdown on Linux
- Corrected some German and Italian translations
* Mon Feb 15 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.8.0
- Added an option in settings that, when enabled, will add a Reload ROM button to the Now Playing window, allowing romhack creators to quickly reload their changes.
- Fixed keyboard controls not being mapped correctly
* Fri Feb 12 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.7.2
- Fixed crash when unselecting a ROM or when launching the program without a rom having been previously selected
* Thu Feb 11 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.7.1
- The plugin previously listed as "GLideN64 [Old]" is actually gln64, the predecessor to GLideN64. This name has been corrected.
- Altered controller binding method to allow opposite C buttons to be pressed at the same time
- Fixed Gamecube controller mapping on Linux
- The SDL library used by RetroArch no longer needs to be replaced on Linux
* Sat Feb 6 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.7.0
- Added a Widescreen (Viewport Hack) option when using GLideN64
- Fixed a bug where the emulator could launch with a larger window size than it should
- For RHDC hacks with a recommended plugin, show which plugin was recommended by the hack uploader
- Added British English localization
* Sun Jan 31 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.6.4
- Fixed some toast messages not being displayed when they should
- Fixed some issues handling ROMs that have a non-ASCII character in their file path on Windows
- Fixed the application freezing when attempting to install the ParallelN64 core on MacOS
* Sat Jan 23 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.6.3
- Changed how the Now Playing / Star Display window is managed to make it behave better in screen capturing software such as OBS
* Fri Jan 15 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.6.2
- Locks the ParallelN64 emulator core to a specific version on MacOS, as newer builds are broken on this operating system. On MacOS, you will receive a prompt to update ParallelN64 regardless of your current update settings and should accept it. Other operating systems are not affected and will continue to update as usual.
* Sun Jan 10 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.6.1
- Changed the default star layout to have generic course names
- Improved detection of MOP2 patch for plugin recommendations
- Fixed one incorrect warning text in the Spanish translation
- Fixed minor inconsistency in Spanish and German translations
* Sat Jan 9 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.6.0
- Added an option to display a default star layout for RHDC hacks if none is provided by the hack author. This is enabled by default, but can be disabled in the romhacking.com options if you prefer.
* Thu Jan 7 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.5.3
- Fixed a crash on startup when following a hack on RHDC that has no authors
- Search in more places for Project64 save files
* Tue Jan 5 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.5.2
- Fixed a bug where downloading an RHDC hack from the classic view would install the oldest version instead of the latest version of the hack
- Fixed a bug where the RHDC view would default to the oldest version of a hack instead of the latest version
- Fixed warning text being unreadable on dark themes
* Tue Jan 5 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.5.1
- Fixed a bug where removing a hack from a list on the romhacking.com site then starting Parallel Launcher would re-add the hack to the list instead of removing it from the group in Parallel Launcher
- Parallel Launcher will no longer automatically submit star progress for kaizo hacks that require video proof.
* Sun Jan 3 2022 Matt Pharoah <mtpharoah@gmail.com> - 5.5.0
- Updated RHDC integration to use a newer more efficient API.
* Thu Nov 26 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.4.4
- Fixed a bug in the conversion of the legacy star layout format that would cause star layouts to not work for some hacks.
* Fri Nov 20 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.4.3
- Force both Parallel Launcher and RetroArch to use a specific version of the SDL library (fixes controller mapping issues).
* Thu Nov 5 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.4.2
- Fixed a bug in the Windows version when multiple controllers are connected that would cause the first controller in the list to be used instead of the one actually selected.
* Tue Aug 25 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.4.1
- Fixed a missing dependency in the Windows version that would cause all web requests to fail on certain Windows installations that don't have the expected C runtime.
* Wed Aug 19 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.4.0
- The Windows version will now automatically import your Project64 saves when a new rom is found if Project64 is installed to a standard location.
- Added a way to test star layout files in Parallel Launcher
- When downloading an RHDC hack with no recommended plugin, Rice will be selected by default if the hack uses the MOP2 patch.
- Decreased the scroll speed of the hack list in the RHDC view
- Fixed a bug where star layouts without a missing star icon set would not show uncollected stars at all
* Tue Aug 18 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.3.0
- Use a star display based save editor for RHDC hacks with a layout file
- Added a Project64 save importer. To import your save data from Project64, right-click on a rom in the classic view (or click the ≡ button in the RHDC view) and select 'Import Project64 Save File'
* Sat Aug 15 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.2.2
- Fixed a bug where layouts for existing romhacks would be not automatically downloaded until another rom with a layout was downloaded.
- Fixed a bug in the conversion of old layouts that could result in an extra star being added to every course
* Tue Aug 11 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.2.1
- Fixed Discord integration not working on Windows. It will still initially show RetroArch as the active game, but should now switch to Parallel Launcher after several seconds. The Linux and Mac versions will still work immediately.
* Tue Aug 11 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.2.0
- Added optional Discord integration to set your game status when playing roms. You can enable/disable this feature in Settings.
- Controller binding is now fully functional on MacOS
- Fixed unwanted padding in the UI on MacOS
* Sun Aug 9 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.1.2
- Fixed errors installing emulator cores and downloading hacks on the 64-bit Windows version when your username contains non-ASCII characters
* Thu Aug 6 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.1.1
- Fixed alignment of text in star display
- Fixed the RHDC view button having a toggle state when it should not
- Fixed the text in the RHDC view not changing colour when selected
- The icons in the RHDC view are now coloured to match the text colour
- Fixed incorrect spacing and font size in the RHDC view on MacOS
* Thu Aug 6 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.1.0
- Added support for a new star layout format
- Fixed incorrect star display on hacks with the 8 star per level patch
- Fixed misalignments in star display ui
* Tue Aug 4 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.0.1
- Fixed a bug that could cause some roms to not appear in the correct group/list after syncing with romhacking.com
- Fixed a crash when clicking 'Open Containing Folder' in the classic view
* Tue Aug 4 2021 Matt Pharoah <mtpharoah@gmail.com> - 5.0.0
- Added an alternate view to show only romhacking.com roms in a card list layout. You can switch between the classic view and the RHDC view by clicking on the rainbow star button.
- When playing an RHDC hack that has a layout file, a live star display is shown in the Now Playing window tracking the stars you have collected. This view updates when your save file does.
- You can now override the application language in Settings
- Added German translations (thanks to Mathis "CommandMC" Dröge)
- Added French translations (thanks to max954)
- You can now bind the left and right modifier keys (Shift, Ctrl, etc.) separately on Windows and Linux.
- Fixed a bug where keyboard bindings would not be set correctly when using a non-English keyboard layout
- Fixed an error when double-clicking a rom in the classic view that hasn't been downloaded yet
- Removes the 'Sync Groups' option in RHDC settings. Your Parallel Launcher groups are now always synced with your hack lists on romhacking.com.
* Fri Jul 17 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.6.3
- Fixed a crash when selecting an RHDC rom that has Glide64 or Rice as its recommended plugin
- Changed sorting of roms to be case insensitive
- Show more information in the error message when a download fails
* Tue Jun 30 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.6.2
- Fixed a bug where the internal rom name would be empty for hacks downloaded from romhacking.com
- Fixed a bug where names of already existing roms would not update when linked with romhacking.com
- Fixed a bug where some translations were not being used on the Windows version of Parallel Launcher
* Mon Jun 29 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.6.1
- Fixed a bug where hacks that require user authentication to access could not be downloaded
- Fixed missing translations on the Windows and Mac versions
- Fixed missing button icons on the Windows and Mac versions
* Mon Jun 29 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.6.0
- RHDC Integration no longer requires all hacks to be immediately downloaded. All followed hacks now show up and can be downloaded on demand.
- Fixed translations not working on the Windows version
- Updated some of the Spanish translations
* Sat Jun 27 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.5.0
- Added a checkbox for whether or not the emulator should pause when it loses focus (defaults to no pause)
- Added Spanish translations (Thanks to Daniel "Reonu" León)
- Fixed a bug where the 'Emulate N64 Depth Compare' checkbox for GLideN64 didn't correctly save your choice
* Fri Jun 26 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.4.2
- Fixed the quality slider in the RHDC rating dialog incorrectly altering the difficulty rating descriptions
- Fixed missing Italian translations in the Debian version
- Fixed missing RetroArch assets in the MacOS version
- Fixed the default XBox360 controller profile on MacOS
* Thu Jun 25 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.4.1
- Fixed radio buttons for plugin selection becoming squished and cut off when the window is too small
- Fixed text being cut off in the RHDC integration settings if the window is too small
* Tue Jun 23 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.4.0
- Your play time is now synced with RHDC if you have romhacking.com integration enabled
* Sun Jun 21 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.3.0
- Released a MacOS version!
- Added Italian translations (thanks to Luigi "JakeDower" Orabona)
- Fixed a bug that caused the automatic plugin detection to suggest the wrong plugin
- Fixed a bug where a previously empty rom list would remain empty after downloading hacks from romhacking.com until the program was restarted or a rom was added via another source
* Sat Jun 13 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.2.0
- When downloading a hack from romhacking.com, automatically select the recommended plugin if it has one
- Added an error log viewer accessible by pressing F7 on the main window
* Tue Jun 9 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.1.2
- Fixed a plugin setting being configured incorrectly (fixes the bowser/peach painting in Super Mario 64 not transitioning correctly when using the ParaLLEl plugin)
* Fri Jun 5 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.1.1
- Fixed major graphical errors when disabling GLideN64 framebuffer emulation when anti-aliasing is enabled
- Prevent GLideN64 N64 Depth Compare from being enabled when framebuffer emulation is disabled
* Fri Jun 5 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.1.0
- The widescreen checkbox will now be on the same line as the others if the window is wide enough
- Changed the broken GLideN64 anti-aliasing settings to an FXAA toggle
- Correctly stylized GlideN64 as GLideN64
- Added an option for whether ParaLLEl should upscale TEXRECTs or not
- Added on option for whether or not to enable framebuffer emulation in GLideN64
- The two above options as well as the existing N64 Depth Compare option for GLideN64 can now be set per rom instead of just globally
* Mon Jun 1 2021 Matt Pharoah <mtpharoah@gmail.com> - 4.0.0
- If no controller is connected when you launch a rom, the controller bindings from the last used controller profile are used
- Added preliminary integration with romhacking.com:
-  * You can enable it by clicking 'Enable romhacking.com integration' in the ≡ menu
-  * Your followed hacks are automatically downloaded and patched
-  * You can optionally sync your groups in Parallel Launcher with your lists on romhacking.com
-  * Star progress is automatically submitted to romhacking.com when you finish playing a rom
-  * Parallel Launcher attempts to guess the appropriate graphics plugin to use when downloading a hack from romhacking.com
-  * You can rate a hack from Parallel Launcher by right-clicking on it in your rom list and selecting 'Rate Hack'
-  * Hacks that do not have stars can be marked as complete from the right-click menu
* Sun May 24 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.2.1
- Fixed a potential crash on startup if the database gets into a bad state
- Fixed groups being lost when a rom is updated
- Fixed unnecessary recalculation of rom checksums (rom scanning should be faster now)
- Fixed group renaming not always working correctly
- Fixed sorting by name not working
* Sat May 23 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.2.0
- Added a right-click option to rename a rom in Parallel Launcher
- Fixed the 'Delete ROM' right-click action not working
- Fixed all groups collapsing after playing a rom when sorting by play time
- Fixed empty groups continuing to show up after they are deleted
* Fri May 22 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.1.1
- Fixed infinite loading bar when roms are removed and no new or changed roms are found
- Fixed crash on Windows when looking for a rom that is on a network drive or WSL environment that no longer exists
- Fixed rom names not updating when a rom file is renamed
- Fixed a crash when multiple rom files have the same checksum and one of them is updated but the other is not
* Thu May 21 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.1.0
- Switched rom storage from a json file to a sqlite database
- The application should now be more responsive when you have a large number of roms
- Parallel Launcher is now smarter at tracking roms:
-  * Moving a ROM from one location to another will no longer result in your play time and plugin settings being lost
-  * If a ROM is deleted, but later restored, your play time and settings are restored
-  * Manually added rom paths are automatically cleaned up when the rom is moved or deleted
-  * If two rom files are identical, their settings and play time are synced
- Fixed a bug where resetting your RetroArch config could result in the assets directory being incorrect
- Your controllers with rumble disabled now emulate having a memory pak inserted instead of nothing
* Mon May 18 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.0.3
- Fixed a regression in version 3.0.0 that could cause controllers to bind incorrectly or not at all on Linux when multiple controllers are connected
- The Windows version now supports Windows 7 as well
* Fri May 8 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.0.2
- Prevent accidentally created game overrides in RetroArch from overriding your Parallel Launcher settings
- Suppress the double controller binding notifications when launching RetroArch
* Fri May 8 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.0.1
- Fixed a bug on the Linux version where a successful first installation of RetroArch would incorrectly be reported as a failure
- Fixed a bug where launching a ROM or patch file from your file browser rather than the Parallel Launcher UI would result in your total play time being reset
* Thu May 7 2021 Matt Pharoah <mtpharoah@gmail.com> - 3.0.0
- RetroArch is now automatically installed and updated by Parallel Launcher. This RetroArch install is separate from your system-wide or user installation, so if you previously installed RetroArch and only use it for Parallel Launcher, you can now safely uninstall it.
- Double-clicking a ROM will now launch it
* Fri May 1 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.10.1
- Fixed an oddly specific bug on Windows where if an XBox One controller connected to a dongle with 5GHz WiFi was turned off (either manually or by inactivity) while Parallel Launcher is running, it would turn itself off again whenever launching a ROM or entering the controller configuration screen.
* Sun Apr 26 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.10.0
- Allow binding controller inputs to Fast Forward and Slow Motion
- Added an automatic core updater
- Added an 'Open Containing Folder' option to the rom list right-click menu
- Added a 'Delete ROM' option to the rom list right-click menu
* Sat Apr 11 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.9.0
- Exposed more graphics plugin settings through Parallel Launcher
- Improved the default settings for some graphics plugins
- Added a button in settings to reset your RetroArch config
- Slightly improved performance when the list of ROMs is very large
* Wed Apr 8 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.8.0
- The settings dialog is now categorized and more compact
- You can now set a folder to put patched roms in when loading a bps patch
- In future updates, the Windows installer will no longer require manually clicking through it when doing an update.
- When opening a new rom or patch file from your file browser, it is no longer added to your explicit rom list if it is already covered by an existing rom search path.
- Fixed a bug in the Windows version where the file dialog to add a specific rom or to apply a BPS patch would not work when selecting (a) file(s) in a WSL directory.
- Fixed a bug in the Windows version where roms could not be added if their file path contains characters not in the Latin1 character set.
* Fri Apr 3 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.7.2
- Fixed inputs not working in RetroArch version 1.9.1
- Fixed the D-Pad not working on certain controllers
- Fixed broken rom names on Windows when a ROM contains non-ASCII characters
- Fixed explicit x4 and x8 ParaLLEl scaling not working
- Added a default controller profile for USB N64 controllers
* Mon Mar 30 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.7.1
- Fixed a bug where the Windows version of Parallel Launcher would sometimes fail to detect RetroArch even though it is installed in the correct location
- Parallel Launcher now searches in more locations for RetroArch before asking the user to provide the path
* Sun Mar 29 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.7.0
- Added support for widescreen mode
* Sun Mar 22 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.6.0
- Parallel Launcher will now offer to install emulator cores for you, removing the need to manually install them in RetroArch first
* Sat Mar 21 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.5.0
- Fixed a bug where launching Parallel Launcher by clicking on a ROM file would not work correctly in some file browsers on Linux
- Show a warning when using the outdated GlideN64 plugin on ParallelN64
- Changed the behaviour of Parallel Launcher when launched by clicking on a ROM in the file browser to now exit when the ROM is closed
- Fixed RetroArch crashing when using the mupen64plus-next core
* Wed Mar 18 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.4.0
- Added an Input Mode option to allow for dual analog input that maps to two N64 controllers
* Sun Mar 15 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.3.1
- Fixed HTML characters not rendering correctly in file paths or internal ROM names
* Sun Mar 15 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.3.0
- Fixed .bps files not being associated with Parallel Launcher on Linux
- Added an automatic updater to the Windows version
* Fri Mar 13 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.2.0
- Parallel Launcher now supports automatically patching .bps files onto known ROMs
- Fixed the 'Last Played' field giving incorrect dates on the Windows version
- Fixed an error when loading the ROM list if a ROM has a nul character in its internal name
* Mon Mar 9 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.1.3
- Fixed a bug where the internal rom name would be scrambled for roms not in big-endian format
- Internal rom names now update when the rom file is modified
* Mon Mar 9 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.1.1
- Fixed a bug where the internal ROM name could have the last character cut off
* Mon Mar 9 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.1.0
- Added an optional column to display the internal name of each ROM
- Support adding individual ROMs instead of just search directories
- When opening a new ROM in Parallel Launcher, add it to the list of known ROMs
- When opening a known ROM in Parallel Launcher, immediately play the ROM in the preferred emulator
- Prevent a controller input from being inadvertantly mapped to C Button Mode
- Fixed potential issue with file paths containing non-ASCII characters
* Sat Feb 7 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.0.2
- Fixed controller profiles not saving when using a locale that uses a comma as the decimal separator
* Tue Feb 3 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.0.1
- Fixed Kill Emulator button not working on Linux when using the Flatpak install of RetroArch
- The Windows version is now compiled with MSVC
- Fixed potential crash when searching for ROMs in the Windows version
* Sun Jan 18 2021 Matt Pharoah <mtpharoah@gmail.com> - 2.0.0
- Split 'Optimize for Performance' checkbox into two separate options
- Added support for the Mupen64Plus-Next emulator!
* Fri Jan 16 2021 Matt Pharoah <mtpharoah@gmail.com> - 1.2.1
- Fixed a bug where certain controller types could have their control scheme changed and attempting to bind to an axis would result in a crash
* Wed Nov 26 2020 Matt Pharoah <mtpharoah@gmail.com> - 1.2.0
- Improved performance of recursive ROM searches, especially on Linux
- Don't force integer scaling on HLE plugins
- Fixed potential crash when refreshing the ROM list while it is already in the process of refreshing
- Show a loading bar when refreshing the ROM list
* Tue Nov 11 2020 Matt Pharoah <mtpharoah@gmail.com> - 1.1.0
- Support configuration of hotkeys and keyboard controls from within Parallel Launcher
- Removed the warning when launching a ROM with no controllers plugged in
* Thu Nov 6 2020 Matt Pharoah <mtpharoah@gmail.com> - 1.0.2
- Fixed the ROM Source manager not handling WSL directories on Windows
* Sun Nov 2 2020 Matt Pharoah <mtpharoah@gmail.com> - 1.0.1
- Fixed controller binds for saving and loading states not working
- Fixed the text entry in the Auto option for ParaLLEl upscaling not updating correctly
* Fri Oct 31 2020 Matt Pharoah <mtpharoah@gmail.com> - 1.0.0
- Initial Release
