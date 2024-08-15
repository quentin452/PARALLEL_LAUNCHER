#include "src/input/windows/direct-input-driver.hpp"

#undef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <dinput.h>
#include <unordered_map>
#include <QTimer>
#include "src/polyfill/windows/unicode.hpp"
#include "src/polyfill/crash.hpp"

#ifdef _WIN32
	#define CALLBACK __stdcall
#else
	#define CALLBACK
#endif

struct DirectInputDevice {
	GamepadId gamepadId;
	IDirectInputDevice8* device;
	ControllerInfo info;
	GamepadState state;
};

static uint s_loopIteration = 0;
static LPDIRECTINPUT8 s_context = nullptr;
static GamepadId s_nextId = 0;
static HashMap<Uuid,DirectInputDevice> s_devices;
static HashMap<GamepadId,Uuid> s_idMap;
static QTimer s_timer;

DirectInputDriver::DirectInputDriver( const GamepadEventEmitter &eventEmitter ) : IGamepadDriver( eventEmitter ) {
	static bool notInitialized = true;
	pl_assert( notInitialized );
	notInitialized = false;

	QObject::connect( &s_timer, &QTimer::timeout, [this](){
		this->processEvents();
	});
}

DirectInputDriver::~DirectInputDriver() {
	DirectInputDriver::stop();
};

static int CALLBACK configureAxisCallback( const DIDEVICEOBJECTINSTANCE *device, void *acquiredDevice ) {
	DIPROPRANGE range;
	range.diph.dwSize = sizeof( DIPROPRANGE );
	range.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	range.diph.dwHow = DIPH_BYID;
	range.diph.dwObj = device->dwType;
	range.lMin = -0x7FFF;
	range.lMax = 0x7FFF;

	IDirectInputDevice8_SetProperty(
		static_cast<IDirectInputDevice8*>( acquiredDevice ),
		DIPROP_RANGE,
		&range.diph
	);

	return DIENUM_CONTINUE;
}

static int CALLBACK onDeviceConnected( const DIDEVICEINSTANCE *device, void *events ) {
	const Uuid uuid( (const ubyte*)&device->guidInstance );

	if( s_devices.find( uuid ) != s_devices.end() ) {
		// Device already connected
		return DIENUM_CONTINUE;
	}

	string name = Unicode::toUtf8( device->tszInstanceName );
	if( name.empty() ) {
		name = Unicode::toUtf8( device->tszProductName );
	}

	const ushort vendorId = (ushort)(device->guidProduct.Data1 & 0xFFFF);
	const ushort productId = (ushort)((device->guidProduct.Data1 & 0xFFFF0000u) >> 16);

	constexpr ushort numButtons = 128;
	constexpr ushort numAxes = 8; // There can be more, but RetroArch only cares about 8 of them
	constexpr ushort numHats = 4;

	IDirectInputDevice8 *acquiredDevice;
	IDirectInput8_CreateDevice( s_context, device->guidInstance, &acquiredDevice, nullptr );
	IDirectInputDevice8_SetDataFormat( acquiredDevice, &c_dfDIJoystick2 );
	IDirectInputDevice8_SetCooperativeLevel( acquiredDevice, GetDesktopWindow(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );

	IDirectInputDevice8_EnumObjects(
		acquiredDevice,
		configureAxisCallback,
		(void*)acquiredDevice,
		DIDFT_ABSAXIS
	);

	s_idMap[s_nextId] = uuid;
	s_devices[uuid] = {
		s_nextId++,
		acquiredDevice,
		ControllerInfo{
			name,
			{ vendorId, productId },
			uuid,
			numButtons,
			numAxes,
			numHats
		},
		GamepadState{
			std::vector<bool>( (size_t)128 ),
			std::vector<double>( (size_t)8 ),
			std::vector<HatDirection>( (size_t)4 )
		}
	};

	DirectInputDevice &inputDevice = s_devices[uuid];
	GamepadConnectedEvent event = { inputDevice.gamepadId, inputDevice.info };
	static_cast<const GamepadEventEmitter*>( events )->gamepadConnected( event );
	inputDevice.info = event.info;

	return DIENUM_CONTINUE;
}

void DirectInputDriver::start() {
	stop();

	DirectInput8Create(
		GetModuleHandle( nullptr ),
		0x0800,
		IID_IDirectInput8,
		(void**)&s_context,
		nullptr
	);

	IDirectInput8_EnumDevices(
		s_context,
		DI8DEVCLASS_GAMECTRL,
		onDeviceConnected,
		(void*)&m_events,
		DIEDFL_ATTACHEDONLY
	);

	s_timer.setInterval( 8 );
	s_timer.start();
}

void DirectInputDriver::stop() {
	s_timer.stop();

	if( s_context == nullptr ) return;

	foreach_mvalue( c, s_devices ) {
		IDirectInputDevice8_Unacquire( c.device );
		IDirectInputDevice8_Release( c.device );
	}

	IDirectInput8_Release( s_context );
	s_context = nullptr;

	s_devices.clear();
	s_idMap.clear();
}

bool DirectInputDriver::isRunning() const {
	return s_context != nullptr;
}

static int CALLBACK enumConnectedDevice( const DIDEVICEINSTANCE *device, void *deviceList ) {
	const Uuid uuid( (const ubyte*)&device->guidInstance );

	string name = Unicode::toUtf8( device->tszInstanceName );
	if( name.empty() ) {
		name = Unicode::toUtf8( device->tszProductName );
	}

	const ushort vendorId = (ushort)(device->guidProduct.Data1 & 0xFFFF);
	const ushort productId = (ushort)((device->guidProduct.Data1 & 0xFFFF0000u) >> 16);

	constexpr ushort numButtons = 128;
	constexpr ushort numAxes = 8;
	constexpr ushort numHats = 4;

	GamepadId id = 0;
	static_cast<std::vector<ConnectedGamepad>*>( deviceList )->push_back({
		id++,
		ControllerInfo{
			name,
			{ vendorId, productId },
			uuid,
			numButtons,
			numAxes,
			numHats
		},
	});

	return DIENUM_CONTINUE;
}

std::vector<ConnectedGamepad> DirectInputDriver::getConnected() {
	std::vector<ConnectedGamepad> devices;
	if( s_context == nullptr ) {
		DirectInput8Create(
			GetModuleHandle( nullptr ),
			0x0800,
			IID_IDirectInput8,
			(void**)&s_context,
			nullptr
		);

		if( s_context == nullptr ) return devices;

		IDirectInput8_EnumDevices(
			s_context,
			DI8DEVCLASS_GAMECTRL,
			enumConnectedDevice,
			(void*)&devices,
			DIEDFL_ATTACHEDONLY
		);

		IDirectInput8_Release( s_context );
		s_context = nullptr;

		return devices;
	}

	devices.reserve( s_devices.size() );
	foreach_cvalue( device, s_devices ) {
		devices.push_back({ device.gamepadId, device.info });
	}
	return devices;
}

static int CALLBACK getUuidCallback( const DIDEVICEINSTANCE *device, void *uuids ) {
	static_cast<std::vector<Uuid>*>( uuids )->push_back(
		Uuid( (const ubyte*)&device->guidInstance )
	);
	return DIENUM_CONTINUE;
}

std::vector<Uuid> DirectInputDriver::getDeviceOrder() {
	LPDIRECTINPUT8 context = nullptr;

	DirectInput8Create(
		GetModuleHandle( nullptr ),
		0x0800,
		IID_IDirectInput8,
		(void**)&context,
		nullptr
	);

	std::vector<Uuid> uuids;
	if( context == nullptr ) return uuids;

	IDirectInput8_EnumDevices(
		context,
		DI8DEVCLASS_GAMECTRL,
		getUuidCallback,
		(void*)&uuids,
		DIEDFL_ATTACHEDONLY
	);

	IDirectInput8_Release( context );

	return uuids;
}

GamepadState DirectInputDriver::getState( GamepadId id ) const {
	const auto &i = s_idMap.find( id );
	if( i == s_idMap.end() ) return GamepadState();
	const auto &j = s_devices.find( i->second );
	if( j == s_devices.end() ) return GamepadState();
	return j->second.state;
}

static inline HatDirection getHatPosition( unsigned int pos ) {
	// RetroArch only checks for specific angles even if the "hat" supports full analog
	switch( pos ) {
		case 0: return HatDirection::Up;
		case 4500: return HatDirection::Up | HatDirection::Right;
		case 9000: return HatDirection::Right;
		case 13500: return HatDirection::Right | HatDirection::Down;
		case 18000: return HatDirection::Down;
		case 22500: return HatDirection::Down | HatDirection::Left;
		case 27000: return HatDirection::Left;
		case 31500: return HatDirection::Left | HatDirection::Up;
		default: return HatDirection::Centre;
	}
}

void DirectInputDriver::processEvents() {
	if( s_context == nullptr ) return;

	/* Relying ONLY on polling can result in button presses being missed,
	 * but the interval is short enough for this to be fine for basic
	 * controller binding
	 */

	if( ++s_loopIteration >= 250 ) {
		// Check for newly connected devices every ~2 seconds
		IDirectInput8_EnumDevices(
			s_context,
			DI8DEVCLASS_GAMECTRL,
			onDeviceConnected,
			(void*)&m_events,
			DIEDFL_ATTACHEDONLY
		);
		s_loopIteration = 1;
	}

	for( auto it = s_devices.begin(); it != s_devices.end(); ) {
		DirectInputDevice &c = it->second;

		if( FAILED( IDirectInputDevice8_Poll( c.device ) ) ) {
			IDirectInputDevice8_Acquire( c.device );
			if( FAILED( IDirectInputDevice8_Poll( c.device ) ) ) {
				it++;
				continue;
			}
		}

		DIJOYSTATE2 state;
		const HRESULT status = IDirectInputDevice8_GetDeviceState( c.device, sizeof( DIJOYSTATE2 ), &state );

		if( status == DIERR_INPUTLOST || status == DIERR_NOTACQUIRED ) {
			m_events.gamepadDisconnected({ c.gamepadId });
			IDirectInputDevice8_Unacquire( c.device );
			IDirectInputDevice8_Release( c.device );
			s_idMap.erase( c.gamepadId );
			it = s_devices.erase( it );
			continue;
		}

		long axisValues[8] = {
			state.lX,
			state.lY,
			state.lZ,
			state.lRx,
			state.lRy,
			state.lRz,
			state.rglSlider[0],
			state.rglSlider[1]
		};

		for( int i = 0; i < 8; i++ ) {
			const double value = (double)axisValues[i] / (double)0x7FFF;
			if( value != c.state.axes[i] ) {
				c.state.axes[i] = value;
				m_events.gamepadAxisMoved({
					c.gamepadId,
					(ubyte)i,
					value
				});
			}
		}

		for( int i = 0; i < 128; i++ ) {
			const bool pressed = (state.rgbButtons[i] != 0);
			if( pressed != c.state.buttons[i] ) {
				c.state.buttons[i] = pressed;
				m_events.gamepadButtonPressed({
					c.gamepadId,
					(ubyte)i,
					pressed
				});
			}
		}

		for( int i = 0; i < 4; i++ ) {
			const HatDirection pos = getHatPosition( state.rgdwPOV[i] );
			if( pos != c.state.hats[i] ) {
				c.state.hats[i] = pos;
				m_events.gamepadHatMoved({
					c.gamepadId,
					(ubyte)i,
					pos
				});
			}
		}

		it++;
	}
}
