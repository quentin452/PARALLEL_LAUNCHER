QT += core gui widgets network svg
TEMPLATE = app

TARGET = "parallel-launcher"

unix {
	# TODO: The Jammy builds set these flags, but they cause a crash on startup. Find out why.
	
	QMAKE_CXXFLAGS -= -flto=auto -ffat-lto-objects
	QMAKE_LFLAGS -= -flto=auto -ffat-lto-objects
	
	QMAKE_CFLAGS -= -flto=auto -ffat-lto-objects
	QMAKE_CFLAGS_RELEASE -= -flto=auto -ffat-lto-objects
	QMAKE_CFLAGS_DEBUG -= -flto=auto -ffat-lto-objects
	QMAKE_CXXFLAGS_RELEASE -= -flto=auto -ffat-lto-objects
	QMAKE_CXXFLAGS_DEBUG -= -flto=auto -ffat-lto-objects
	QMAKE_LFLAGS_RELEASE -= -flto=auto -ffat-lto-objects
	QMAKE_CXXFLAGS_DEBUG -= -flto=auto -ffat-lto-objects
}

CONFIG += qt object_parallel_to_source
CONFIG(debug, debug|release){
	OBJECTS_DIR = "build/debug/obj"
	MOC_DIR = "build/debug/moc"
	UI_DIR = "build/debug/ui"
	DEFINES += DEBUG
}
CONFIG(release, debug|release){
	OBJECTS_DIR = "build/release/obj"
	MOC_DIR = "build/release/moc"
	UI_DIR = "build/release/ui"
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/main.cpp $$files(src/core/*.cpp) $$files(src/db/*.cpp) $$files(src/ui/*.cpp) $$files(src/polyfill/*.cpp) $$files(src/input/*.cpp) $$files(src/plugins/*.cpp) $$files(src/thirdparty/*.cpp) $$files(src/thirdparty/*.c) $$files(src/rhdc/core/*.cpp) $$files(src/rhdc/web/*.cpp) $$files(src/rhdc/ui/*.cpp)
HEADERS += src/types.hpp $$files(src/core/*.hpp) $$files(src/db/*.hpp) $$files(src/ui/*.hpp) $$files(src/polyfill/*.hpp) $$files(src/input/*.hpp) $$files(src/plugins/*.hpp) $$files(src/thirdparty/*.h) $$files(src/rhdc/core/*.hpp) $$files(src/rhdc/web/*.hpp) $$files(src/rhdc/ui/*.hpp)
FORMS += $$files(src/ui/designer/*.ui) $$files(src/rhdc/ui/designer/*.ui)
RESOURCES = data/resources.qrc
TRANSLATIONS = lang/parallel-launcher_en_us.ts lang/parallel-launcher_en_gb.ts lang/parallel-launcher_it.ts lang/parallel-launcher_es.ts lang/parallel-launcher_fr.ts lang/parallel-launcher_fr_ca.ts lang/parallel-launcher_de.ts lang/parallel-launcher_ar.ts lang/parallel-launcher_pt.ts

macx|win32 {
	LIBS += -ldiscord-rpc
	VERSION = 7.7.0
}

macx {
	CONFIG += c++17
	DEFINES += BOOST_FILESYSTEM_DYN_LINK _LIBCPP_DISABLE_AVAILABILITY
	LIBS += -L./lib
	ICON = appicon.icns
	QMAKE_INFO_PLIST = app.plist
	
	SOURCES += $$files(src/input/macos/*.cpp) $$files(src/polyfill/macos/*.cpp)
	HEADERS += $$files(src/input/macos/*.hpp) $$files(src/polyfill/macos/*.hpp)
	FORMS += $$files(src/updaters/*.ui)
	
	lsjsStub.target = data/parallel-launcher-lsjs
	lsjsStub.depends = FORCE
	lsjsStub.commands = touch data/parallel-launcher-lsjs
	
	PRE_TARGETDEPS += data/parallel-launcher-lsjs
	QMAKE_EXTRA_TARGETS += lsjsStub
}

!flatpak-version {
	SOURCES += $$files(src/updaters/*.cpp)
	HEADERS += $$files(src/updaters/*.hpp)
}

win32 {
	win32:contains(QMAKE_HOST.arch, x86_64) {
		WIN_DIR = win64
		WIN_PLATFORM = x64
	} else {
		WIN_DIR = win32
		WIN_PLATFORM = win32
	}
	
	CONFIG(debug, debug|release){
		WIN_BUILD_DIR = debug
	}
	CONFIG(release, debug|release){
		WIN_BUILD_DIR = release
		QMAKE_LFLAGS += /INCREMENTAL:NO
	}

	DEFINES += _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
	SOURCES += $$files(src/polyfill/windows/*.cpp) $$files(src/input/common/*.cpp) $$files(src/input/windows/*.cpp)
	HEADERS += $$files(src/polyfill/windows/*.hpp) $$files(src/input/common/*.hpp) $$files(src/input/windows/*.hpp)
	FORMS += $$files(src/updaters/*.ui)

	CONFIG += embed_manifest_exe no_batch
	QMAKE_CXXFLAGS += /std:c++17 /WX /utf-8 /Zc:preprocessor,externC,inline,lambda,rvalueCast /Wv:18 /Zi /EHs
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE += /Ox /Gy
	QMAKE_LFLAGS += /DEBUG:FULL
	LIBS += -L$${WIN_DIR}/lib -lkernel32 -lOle32 -lAdvapi32 -ldinput8 -ldxguid -lUser32 -lVersion -lsqlite3 -lDbgHelp -lbcrypt
	INCLUDEPATH += $${WIN_DIR}/include $$PWD
	DEPENDPATH += $$PWD
	
	RC_ICONS = "data\\appicon.ico"
	RC_LANG = "EN-CA"
	QMAKE_TARGET_COMPANY = "Matt Pharoah"
	QMAKE_TARGET_COPYRIGHT = "GNU GENERAL PUBLIC LICENSE Version 3"
	QMAKE_TARGET_PRODUCT = "Parallel Launcher"
	
	lsjsTarget.target = parallel-launcher-lsjs.exe
	lsjsTarget.depends = FORCE
	lsjsTarget.commands = cl /WX /Wv:18 /Ox /Gy /I $${WIN_DIR}\\include lsjs.c /link $${WIN_DIR}\\lib\\SDL2.lib /out:$${WIN_BUILD_DIR}\\parallel-launcher-lsjs.exe

	sdlProxyTarget.target = parallel-launcher-sdl-relay.exe
	sdlProxyTarget.depends = FORCE
	sdlProxyTarget.commands = cl /std:c11 /WX /Wv:18 /Ox /Gy /I $${WIN_DIR}\\include sdl-proxy.c /link $${WIN_DIR}\\lib\\SDL2.lib /out:$${WIN_BUILD_DIR}\\parallel-launcher-sdl-relay.exe
	
	mountSdTarget.target = mount-sd-card.exe
	mountSdTarget.depends = FORCE
	mountSdTarget.commands = msbuild windows-mount-sd-card\\mount-sd-card.vcxproj /p:configuration=Release /p:platform=$${WIN_PLATFORM}
	
	PRE_TARGETDEPS += parallel-launcher-lsjs.exe parallel-launcher-sdl-relay.exe mount-sd-card.exe
	QMAKE_EXTRA_TARGETS += lsjsTarget sdlProxyTarget mountSdTarget
}

unix {
	!macx {
		QT += dbus
		SOURCES += $$files(src/input/linux/*.cpp) $$files(src/input/common/*.cpp)
		HEADERS += $$files(src/input/linux/*.hpp) $$files(src/input/common/*.hpp)
		
		flatpak-version {
			DEFINES += FLATPAK_VERSION
			LIBS += -ldiscord-rpc
		} else {
			DEFINES += DISCORD_LAZY_LINKING
			LIBS += -lSDL2
			
			system( [ `$$QMAKE_CXX -dumpversion | cut -d'.' -f1` -lt 10 ] ) {
				LIBS += -lstdc++fs
			}
		}
	}
}

!win32 {
	QMAKE_CXXFLAGS += -std=c++17 -g1
	QMAKE_LFLAGS += -rdynamic
	LIBS += -lsqlite3 -lgcrypt -lpthread -ldl
	
	macx {
		LIBS += -lboost_system -lboost_filesystem -framework CoreFoundation -framework IOKit
		INCLUDEPATH += /usr/local/include
	} else {
		CONFIG += link_pkgconfig
		QMAKE_LFLAGS += -no-pie -fno-pie
		QMAKE_CXXFLAGS += -Werror
		
		lsjsTarget.target = parallel-launcher-lsjs
		lsjsTarget.depends = FORCE
		lsjsTarget.commands = gcc -Werror -std=c99 -O3 lsjs.c -lSDL2 -o parallel-launcher-lsjs

		sdlProxyTarget.target = parallel-launcher-sdl-relay
		sdlProxyTarget.depends = FORCE
		sdlProxyTarget.commands = gcc -Werror -std=c11 -O3 sdl-proxy.c -lSDL2 -o parallel-launcher-sdl-relay
		
		PRE_TARGETDEPS += parallel-launcher-lsjs parallel-launcher-sdl-relay
		QMAKE_EXTRA_TARGETS += lsjsTarget sdlProxyTarget
	}
}

flatpak-version {
	QMAKE_POST_LINK += "lrelease app.pro"

	inst_bin.path = /app/bin/
	inst_bin.files = parallel-launcher
	
	inst_desktop.path = /app/share/applications/
	inst_desktop.files = ca.parallel_launcher.ParallelLauncher.desktop

	INSTALLS = inst_bin inst_desktop
}
