#include "src/ui/controller-config-dialog.hpp"
#include "ui_controller-config-dialog.h"

#include <cstring>
#include <QCoreApplication>
#include <QInputDialog>
#include <QMessageBox>
#include "src/input/gamepad-controller.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/numeric-string.hpp"
#include "src/ui/bind-input-dialog.hpp"
#include "src/ui/neutral-input-dialog.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/polyfill/crash.hpp"

static const char *s_actionNames[] = {
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Analog Up" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Analog Down" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Analog Left" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Analog Right" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "C Up" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "C Down" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "C Left" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "C Right" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "D-Pad Up" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "D-Pad Down" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "D-Pad Left" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "D-Pad Right" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "A Button" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "B Button" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "L Trigger" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Z Trigger" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "R Trigger" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Start Button" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "X Button" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Y Button" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Save State" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Load State" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Toggle Fast Forward" ),
	QT_TRANSLATE_NOOP( "ControllerConfigDialog", "Toggle Slow Motion" )
};

static_assert( sizeof( s_actionNames) == sizeof(char*) * (size_t)ControllerAction::NUM_ACTIONS );

ControllerConfigDialog::ControllerConfigDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::ControllerConfigDialog ),
	m_controllerId( -1 )
{
	m_ui->setupUi( this );
	UiUtil::scaleWindow( this );
	
	m_profile = DefaultProfile::get( FileController::loadAppSettings().inputDriver );
	UiUtil::fixDialogButtonsOnWindows( this );

	setWindowIcon( Icon::appIcon() );
	m_ui->quickConfigureButton->setIcon( Icon::fastForward() );
	m_ui->saveAsButton->setIcon( Icon::saveAs() );
	m_ui->saveButton->setIcon( Icon::save() );
	m_ui->cancelButton->setIcon( Icon::cancel() );

	connect( m_ui->bindingButtonGroup, SIGNAL( buttonClicked(QAbstractButton*) ), this, SLOT( bindSingle(QAbstractButton*) ) );
}

ControllerConfigDialog::~ControllerConfigDialog() {
	delete m_ui;
}

static void updateButtonText( QAbstractButton *button, const ControllerProfile &profile ) {
	const Binding &binding = profile.bindings[button->property( "controllerBinding" ).toInt()];
	switch( binding.type ) {
		case BindingType::None:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Not Bound" ) );
			break;
		case BindingType::Button:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Button %1" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
		case BindingType::AxisNegative:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Axis -%1" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
		case BindingType::AxisPositive:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Axis +%1" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
		case BindingType::HatUp:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Hat %1 Up" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
		case BindingType::HatDown:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Hat %1 Down" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
		case BindingType::HatLeft:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Hat %1 Left" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
		case BindingType::HatRight:
			button->setText( QCoreApplication::translate( "ControllerConfigDialog", "Hat %1 Right" ).arg( Number::toString( binding.buttonOrAxis ).c_str() ) );
			break;
	}
}

void ControllerConfigDialog::setActiveController(
	const string &name,
	const Uuid &uuid,
	int id,
	const ControllerProfile &profile
) {
	m_ui->controllerNameLabel->setText( name.c_str() );
	m_profile = profile;
	m_controllerUuid = uuid;
	m_controllerId = id;

	m_ui->rumbleCheckbox->setChecked( profile.rumble );
	m_ui->sensitivitySpinner->setValue( profile.sensitivity );
	m_ui->deadzoneSpinner->setValue( profile.deadzone );
	m_ui->axisThresholdSpinner->setValue( (int)(profile.axisThreshold * 100.0) );

	for( QAbstractButton *button : m_ui->bindingButtonGroup->buttons() ) {
		updateButtonText( button, profile );
	}

	m_ui->saveButton->setVisible( !DefaultProfile::exists( FileController::loadAppSettings().inputDriver, profile.name ) );
}

void ControllerConfigDialog::save() {
	const InputDriver inputDriver = FileController::loadAppSettings().inputDriver;
	pl_assert( !DefaultProfile::exists( inputDriver, m_profile.name ) );

	m_profile.rumble = m_ui->rumbleCheckbox->isChecked();
	m_profile.sensitivity = m_ui->sensitivitySpinner->value();
	m_profile.deadzone = m_ui->deadzoneSpinner->value();
	m_profile.axisThreshold = (double)m_ui->axisThresholdSpinner->value() / 100.0;

	std::map<string, ControllerProfile> savedProfiles = FileController::loadControllerProfiles( inputDriver );
	savedProfiles[m_profile.name] = m_profile;
	FileController::saveControllerProfiles( inputDriver, savedProfiles );

	accept();
}

void ControllerConfigDialog::saveAs() {
	QString profileName = QInputDialog::getText( this, tr( "Enter Profile Name" ), tr( "Enter a name for your new controller profile." ) );
	if( profileName.isNull() || profileName.isEmpty() ) {
		return;
	}

	if( DefaultProfile::exists( FileController::loadAppSettings().inputDriver, profileName.toStdString() ) ) {
		QMessageBox::critical( this, tr( "Failed to Save Profile" ), tr( "A default controller profile with that name already exists." ) );
		return;
	}

	//TODO: warn if overwriting an existing profile

	m_profile.name = profileName.toStdString();
	save();
}

void ControllerConfigDialog::bindAll() {
	Binding previousBindings[(ubyte)ControllerAction::NUM_ACTIONS];
	std::memcpy( previousBindings, m_profile.bindings, sizeof( m_profile.bindings ) );

	bool cancelled = false;
	for( QAbstractButton *button : m_ui->bindingButtonGroup->buttons() ) {
		const int action = button->property( "controllerBinding" ).toInt();

		BindInputDialog dialog( tr( s_actionNames[action] ), m_controllerId, true );
		m_profile.bindings[action] = dialog.getBinding( cancelled );
		updateButtonText( button, m_profile );

		if( cancelled ) break;

		if( m_profile.bindings[action].type != BindingType::None ) {
			NeutralInputDialog( m_controllerId ).exec();
		}
	}

	if( cancelled ) {
		std::memcpy( m_profile.bindings, previousBindings, sizeof( m_profile.bindings ) );
		for( QAbstractButton *button : m_ui->bindingButtonGroup->buttons() ) {
			updateButtonText( button, m_profile );
		}
	}
}

void ControllerConfigDialog::bindSingle( QAbstractButton *button ) {
	const int action = button->property( "controllerBinding" ).toInt();

	BindInputDialog dialog( tr( s_actionNames[action] ), m_controllerId, false );
	m_profile.bindings[action] = dialog.getBinding();

	updateButtonText( button, m_profile );
}
