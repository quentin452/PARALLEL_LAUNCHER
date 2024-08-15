#include "src/ui/direct-play-window.hpp"
#include "ui_direct-play-window.h"

#include "src/core/special-groups.hpp"
#include "src/ui/icons.hpp"

DirectPlayWindow::DirectPlayWindow( const RomFile &romFile ) :
	QMainWindow( nullptr ),
	m_romFile( romFile ),
	m_ui( new Ui::DirectPlayWindow )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
}

DirectPlayWindow::~DirectPlayWindow() {
	delete m_ui;
}

RomInfo DirectPlayWindow::getRomInfo() const {
	return RomInfo{
		m_romFile.sha1,
		"",
		RomUtil::getInternalName( m_romFile.path ),
		m_ui->romSettings->getEmulatorCore(),
		m_ui->romSettings->getParallelPlugin(),
		m_ui->romSettings->getMupenPlugin(),
		m_ui->romSettings->upscaleTexRects(),
		m_ui->romSettings->removeBorders(),
		m_ui->romSettings->emulateFramebuffer(),
		m_ui->romSettings->accurateDepthCompare(),
		m_ui->romSettings->glidenRspEmulation(),
		0,
		0,
		std::set<string>(),
		m_ui->romSettings->overclockCpu(),
		m_ui->romSettings->overclockVi(),
		RomUtil::getCrc32( m_romFile.path ),
		m_ui->romSettings->getInputMode(),
		m_ui->romSettings->widescreen(),
		m_ui->romSettings->bigEEPROM(),
		m_ui->romSettings->useInterpreter(),
		RomUtil::readControllerTypes( m_romFile.path ),
		m_ui->romSettings->sdCard()
	};
}

void DirectPlayWindow::accept() {
	hide();
	emit play();
}
