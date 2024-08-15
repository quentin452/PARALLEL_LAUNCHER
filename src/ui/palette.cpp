#include "src/ui/palette.hpp"

#ifdef _WIN32
#include <Windows.h>

bool ColourPalette::systemIsDarkMode() {
	DWORD value = 1;
	DWORD size = (DWORD)sizeof( DWORD );
	const LSTATUS status = RegGetValueW(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"AppsUseLightTheme",
		RRF_RT_REG_DWORD,
		nullptr,
		(void*)&value,
		&size
	);

	return (status == ERROR_SUCCESS) && (value == 0);
}
#endif

#ifdef __APPLE__
#include "src/polyfill/process.hpp"
bool ColourPalette::systemIsDarkMode() {
	string output;
	return Process::tryGetOutput( "defaults read -g AppleInterfaceStyle", output ) && output.find( "Dark" ) != string::npos;
}
#endif

#ifdef __linux__
#include <QApplication>
#include <QPalette>

bool ColourPalette::systemIsDarkMode() {
	return QApplication::palette().color( QPalette::Active, QPalette::Base ).toHsv().value() < 128;
}
#endif

#ifndef __linux__
static constexpr QRgb s_lightCP[20][3] = {
	/* WindowText */		{ 0xff232627u, 0xff232627u, 0xffa0a2a2u },
	/* Button */			{ 0xffeff0f1u, 0xffeff0f1u, 0xffe3e5e7u },
	/* Light */				{ 0xffffffffu, 0xffffffffu, 0xffffffffu },
	/* Midlight */			{ 0xfff7f7f8u, 0xfff7f7f8u, 0xffecedeeu },
	/* Dark */				{ 0xff888e93u, 0xff888e93u, 0xff82878cu },
	/* Mid */				{ 0xffc4c9cdu, 0xffc4c9cdu, 0xffbcc0c5u },
	/* Text */				{ 0xff232627u, 0xff232627u, 0xffa8a9a9u },
	/* BrightText */		{ 0xffffffffu, 0xffffffffu, 0xffffffffu },
	/* ButtonText */		{ 0xff232627u, 0xff232627u, 0xffa0a2a2u },
	/* Base */				{ 0xfffcfcfcu, 0xfffcfcfcu, 0xfff1f1f1u },
	/* Window */			{ 0xffeff0f1u, 0xffeff0f1u, 0xffe3e5e7u },
	/* Shadow */			{ 0xff474a4cu, 0xff474a4cu, 0xff47494cu },
	/* Highlight */			{ 0xff3daee9u, 0xffc2e0f5u, 0xffe3e5e7u },
	/* HighlightedText */	{ 0xfffcfcfcu, 0xff232627u, 0xffa0a2a2u },
	/* Link */				{ 0xff2980b9u, 0xff2980b9u, 0xffa2c8e0u },
	/* LinkVisited */		{ 0xff7f8c8du, 0xff7f8c8du, 0xffc7cbcbu },
	/* AlternateBase */		{ 0xffeff0f1u, 0xffeff0f1u, 0xffe3e5e7u },
	/* ToolTipBase */		{ 0xff232627u, 0xff232627u, 0xff232627u },
	/* ToolTipText */		{ 0xfffcfcfcu, 0xfffcfcfcu, 0xfffcfcfcu },
	/* Placeholder */		{ 0x80232627u, 0x80232627u, 0x80232627u }
};

static constexpr QRgb s_darkCP[20][3] = {
	/* WindowText */		{ 0xffeff0f1u, 0xffeff0f1u, 0xff6e7175u },
	/* Button */			{ 0xff31363bu, 0xff31363bu, 0xff2e3338u },
	/* Light */				{ 0xff454c54u, 0xff454c54u, 0xff434a51u },
	/* Midlight */			{ 0xff3c4248u, 0xff3c4248u, 0xff3a4046u },
	/* Dark */				{ 0xff1c1f22u, 0xff1c1f22u, 0xff1b1e21u },
	/* Mid */				{ 0xff2b2f34u, 0xff2b2f34u, 0xff292d31u },
	/* Text */				{ 0xffeff0f1u, 0xffeff0f1u, 0xff65686au },
	/* BrightText */		{ 0xffffffffu, 0xffffffffu, 0xffffffffu },
	/* ButtonText */		{ 0xffeff0f1u, 0xffeff0f1u, 0xff6e7175u },
	/* Base */				{ 0xff232629u, 0xff232629u, 0xff212427u },
	/* Window */			{ 0xff31363bu, 0xff31363bu, 0xff2e3338u },
	/* Shadow */			{ 0xff141719u, 0xff141719u, 0xff131518u },
	/* Highlight */			{ 0xff3daee9u, 0xff224e65u, 0xff2e3338u },
	/* HighlightedText */	{ 0xffeff0f1u, 0xffeff0f1u, 0xff6e7175u },
	/* Link */				{ 0xff2980b9u, 0xff2980b9u, 0xff234257u },
	/* LinkVisited */		{ 0xff7f8c8du, 0xff7f8c8du, 0xff404648u },
	/* AlternateBase */		{ 0xff31363bu, 0xff31363bu, 0xff2e3338u },
	/* ToolTipBase */		{ 0xff31363bu, 0xff31363bu, 0xff31363bu },
	/* ToolTipText */		{ 0xffeff0f1u, 0xffeff0f1u, 0xffeff0f1u },
	/* Placeholder */		{ 0x80eff0f1u, 0x80eff0f1u, 0x80eff0f1u }
};

#define setRole( role, i ) \
	palette.setColor( QPalette::Active, QPalette::role, QColor( colours[i][0] ) ); \
	palette.setColor( QPalette::Inactive, QPalette::role, QColor( colours[i][1] ) ); \
	palette.setColor( QPalette::Disabled, QPalette::role, QColor( colours[i][2] ) );

static inline QPalette makePalette( const QRgb colours[][3] ) {
	QPalette palette;
	setRole( WindowText, 0 );
	setRole( Button, 1 );
	setRole( Light, 2 );
	setRole( Midlight, 3 );
	setRole( Dark, 4 );
	setRole( Mid, 5 );
	setRole( Text, 6 );
	setRole( BrightText, 7 );
	setRole( ButtonText, 8 );
	setRole( Base, 9 );
	setRole( Window, 10 );
	setRole( Shadow, 11 );
	setRole( Highlight, 12 );
	setRole( HighlightedText, 13 );
	setRole( Link, 14 );
	setRole( LinkVisited, 15 );
	setRole( AlternateBase, 16 );
	setRole( ToolTipBase, 17 );
	setRole( ToolTipText, 18 );
	setRole( PlaceholderText, 19 );
	return palette;
}

#undef setRole

const QPalette &ColourPalette::light() {
	static const QPalette s_palette = makePalette( s_lightCP );
	return s_palette;
}

const QPalette &ColourPalette::dark() {
	static const QPalette s_palette = makePalette( s_darkCP );
	return s_palette;
}
#endif
