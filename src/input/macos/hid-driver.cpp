#include "src/input/macos/hid-driver.hpp"

#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include "src/core/qthread.hpp"
#include "src/polyfill/crash.hpp"

/* Mimic RetroArch's Mac input driver so that the inputs map as expected in RetroArch */

static IOHIDManagerRef s_hidManager;
static GamepadEventEmitter s_events;
static bool s_active = false;

struct HidInputMap {
	HashMap<uint, ushort> button;
	HashMap<uint, ushort> axis;
	HashMap<uint, ushort> hat;
};

struct HidDevice {
	HidInputMap inputMap;
	ControllerInfo info;
	GamepadState state;
	uint portId;
};

static HashMap<GamepadId, HidDevice> s_devices;

static constexpr ushort NOT_FOUND = 0xFFFF;
static inline ushort tryGetIndex( const HashMap<uint, ushort> &inputMap, uint cookie ) {
	const auto &i = inputMap.find( cookie );
	if( i == inputMap.end() ) return NOT_FOUND;
	return i->second;
}

HidDriver::HidDriver( const GamepadEventEmitter &eventEmitter ) : IGamepadDriver( eventEmitter ) {
	static bool notInitialized = true;
	pl_assert( notInitialized );
	notInitialized = false;
	s_events = eventEmitter;
}

HidDriver::~HidDriver() {
	stop();
}

static uint getIntProperty( IOHIDDeviceRef &device, const CFStringRef &key ) {
	CFNumberRef ref = (CFNumberRef)IOHIDDeviceGetProperty(device, key);
	if( !ref || CFGetTypeID( ref ) != CFNumberGetTypeID() ) return 0;
	uint value = 0;
	CFNumberGetValue( (CFNumberRef)ref, kCFNumberIntType, &value );
	return value;
}

static inline ushort getShortProperty( IOHIDDeviceRef &device, const CFStringRef &key ) {
	return (ushort)getIntProperty( device, key );
}

static string getStringProperty( IOHIDDeviceRef &device, const CFStringRef &key ) {
	char buffer[1024];
	buffer[0] = '\0';
	CFStringRef ref = (CFStringRef)IOHIDDeviceGetProperty( device, key );
	if( ref ) CFStringGetCString( ref, buffer, 1024, kCFStringEncodingUTF8 );
	buffer[1023] = '\0';
	return string( buffer );
}

static void handleButtonEvent( GamepadId gamepadId, HidDevice &device, IOHIDValueRef &value, uint cookie ) {
	const ushort index = tryGetIndex( device.inputMap.button, cookie );
	if( index == NOT_FOUND || index > 0xFF ) return;

	const bool pressed = (bool)IOHIDValueGetIntegerValue( value );
	if( index < device.state.buttons.size() ) {
		device.state.buttons[index] = pressed;
	}

	QtThread::safeAsync( [=](){
		s_events.gamepadButtonPressed({
			gamepadId,
			(ubyte)index,
			pressed
		});
	});
}

static void handleAxisOrButtonEvent( GamepadId gamepadId, HidDevice &device, IOHIDValueRef &value, IOHIDElementRef &element, uint cookie ) {
	const ushort index = tryGetIndex( device.inputMap.axis, cookie );
	if( index == NOT_FOUND || index > 0xFF ) {
		handleButtonEvent( gamepadId, device, value, cookie );
		return;
	}

	const CFIndex min = IOHIDElementGetPhysicalMin( element );
	const CFIndex current = IOHIDValueGetIntegerValue( value );
	const CFIndex max = IOHIDElementGetPhysicalMax( element );

	if( min == max ) return;

	const double normalizedValue = (2.0 * (double)(current - min) / (double)(max - min)) - 1.0;
	if( index < device.state.axes.size() ) {
		device.state.axes[index] = normalizedValue;
	}

	QtThread::safeAsync( [=](){
		s_events.gamepadAxisMoved({
			gamepadId,
			(ubyte)index,
			normalizedValue
		});
	});
}

static void handleHatEvent( GamepadId gamepadId, HidDevice &device, IOHIDValueRef &value, IOHIDElementRef &element, uint cookie ) {
	const ushort index = tryGetIndex( device.inputMap.hat, cookie );
	if( index == NOT_FOUND || index > 0xFF ) return;

	CFIndex i = IOHIDValueGetIntegerValue( value );
	const CFIndex min = IOHIDElementGetLogicalMin( element );
	const CFIndex max = IOHIDElementGetLogicalMax( element );

	if( max == min + 3 ) i *= 2;
	if( min == 1 ) i--;

	HatDirection direction;
	switch( i ) {
		case 0: direction = HatDirection::Up; break;
		case 1: direction = HatDirection::Up | HatDirection::Right; break;
		case 2: direction = HatDirection::Right; break;
		case 3: direction = HatDirection::Down | HatDirection::Right; break;
		case 4: direction = HatDirection::Down; break;
		case 5: direction = HatDirection::Down | HatDirection::Left; break;
		case 6: direction = HatDirection::Left; break;
		case 7: direction = HatDirection::Up | HatDirection::Left; break;
		default: direction = HatDirection::Centre; break;
	}

	if( index < device.state.hats.size() ) {
		device.state.hats[index] = direction;
	}

	QtThread::safeAsync( [=](){
		s_events.gamepadHatMoved({
			gamepadId,
			(ubyte)index,
			direction
		});
	});
}

static void inputReceived( void *context, IOReturn, void*, IOHIDValueRef value ) {
	const GamepadId gamepadId = (GamepadId)reinterpret_cast<std::intptr_t>( context );
	if( s_devices.count( gamepadId ) <= 0 ) return;

	HidDevice &device = s_devices.at( gamepadId );
	IOHIDElementRef element = IOHIDValueGetElement( value );
	const uint type = (uint)IOHIDElementGetType( element );
	const uint page = (uint)IOHIDElementGetUsagePage( element );
	const uint usage = (uint)IOHIDElementGetUsage( element );
	const uint cookie = (uint)IOHIDElementGetCookie( element );

	switch( page ) {
		case kHIDPage_GenericDesktop:
			if( type != kIOHIDElementTypeInput_Misc ) break;
			if( usage == kHIDUsage_GD_Hatswitch ) {
				handleHatEvent( gamepadId, device, value, element, cookie );
			} else {
				handleAxisOrButtonEvent( gamepadId, device, value, element, cookie );
			}
			break;
		case kHIDPage_Consumer:
		case kHIDPage_Button:
			if( type == kIOHIDElementTypeInput_Misc || type == kIOHIDElementTypeInput_Button ) {
				handleButtonEvent( gamepadId, device, value, cookie );
			}
			break;
		case kHIDPage_Simulation:
			if( type != kIOHIDElementTypeInput_Misc ) break;
			handleAxisOrButtonEvent( gamepadId, device, value, element, cookie );
			break;
		default: break;
	}
}

static CFComparisonResult portCompare( const void *x1, const void *x2, void* ) {
	IOHIDElementRef a = (IOHIDElementRef)x1;
	IOHIDElementRef b = (IOHIDElementRef)x2;

	const uint pageA = (uint)IOHIDElementGetUsagePage( a );
	const uint pageB = (uint)IOHIDElementGetUsagePage( b );
	if( pageA != pageB ) {
		return (CFComparisonResult)( pageA > pageB );
	}

	const uint usageA = (uint)IOHIDElementGetUsage( a );
	const uint usageB = (uint)IOHIDElementGetUsage( b );
	if( usageA != usageB ) {
		return (CFComparisonResult)( usageA > usageB );
	}

	const uint cookieA = (uint)IOHIDElementGetCookie( a );
	const uint cookieB = (uint)IOHIDElementGetCookie( b );
	return (CFComparisonResult)( cookieA > cookieB );
}

static const HashMap<uint, ushort> s_axisMap = {
	{ kHIDUsage_GD_X, 0 },
	{ kHIDUsage_GD_Y, 1 },
	{ kHIDUsage_GD_Rx, 2 },
	{ kHIDUsage_GD_Ry, 3 },
	{ kHIDUsage_GD_Z, 4 },
	{ kHIDUsage_GD_Rz, 5 },
	{ kHIDUsage_Sim_Rudder, 6 },
	{ kHIDUsage_Sim_Throttle, 7 },
	{ kHIDUsage_Sim_Steering, 8 },
	{ kHIDUsage_Sim_Accelerator, 9 },
	{ kHIDUsage_Sim_Brake, 10 }
};

static inline void initInputMap( IOHIDDeviceRef &device, HidInputMap &inputMap ) {
	CFArrayRef elementsConst = IOHIDDeviceCopyMatchingElements( device, nullptr, kIOHIDOptionsTypeNone );
	const CFIndex numInputs = CFArrayGetCount( elementsConst );
	CFMutableArrayRef elements = CFArrayCreateMutableCopy( kCFAllocatorDefault, numInputs, elementsConst );
	CFRange range = CFRangeMake( 0, numInputs );

	ushort numButtons = 0;
	ushort numHats = 0;

	HashSet<uint> buttonUsages;
	std::vector<uint> deferredButtons;

	CFArraySortValues( elements, range, portCompare, nullptr );
	for( CFIndex i = 0; i < numInputs; i++ ) {
		IOHIDElementRef element = (IOHIDElementRef)CFArrayGetValueAtIndex( elements, i );
		if( !element ) continue;

		const uint type = (uint)IOHIDElementGetType( element );
		const uint page = (uint)IOHIDElementGetUsagePage( element );
		const uint usage = (uint)IOHIDElementGetUsage( element );
		const uint cookie = (uint)IOHIDElementGetCookie( element );

		switch( page ) {
			case kHIDPage_GenericDesktop:
				if( type != kIOHIDElementTypeInput_Misc ) break;
				if( usage == kHIDUsage_GD_Hatswitch ) {
					inputMap.hat[cookie] = numHats++;
				} else if( s_axisMap.count( usage ) > 0 ) {
					inputMap.axis[cookie] = s_axisMap.at( usage );
				} else if( buttonUsages.count( usage ) > 0 ) {
					deferredButtons.push_back( cookie );
				} else {
					inputMap.button[cookie] = numButtons++;
					buttonUsages.insert( usage );
				}
				break;
			case kHIDPage_Consumer:
			case kHIDPage_Button:
				if( type == kIOHIDElementTypeInput_Misc || type == kIOHIDElementTypeInput_Button ) {
					if( buttonUsages.count( usage ) > 0 ) {
						deferredButtons.push_back( cookie );
					} else {
						inputMap.button[cookie] = numButtons++;
						buttonUsages.insert( usage );
					}
				}
				break;
			case kHIDPage_Simulation:
				if( s_axisMap.count( usage ) > 0 ) {
					inputMap.axis[cookie] = s_axisMap.at( usage );
				} else if( buttonUsages.count( usage ) > 0 ) {
					deferredButtons.push_back( cookie );
				} else {
					inputMap.button[cookie] = numButtons++;
					buttonUsages.insert( usage );
				}
				break;
			default: break;
		}
	}

	for( uint cookie : deferredButtons ) {
		inputMap.button[cookie] = numButtons++;
	}
}

static HidDevice createHidDevice( IOHIDDeviceRef &device ) {
	HidDevice deviceInfo;

	deviceInfo.info.name = getStringProperty( device, CFSTR( kIOHIDProductKey ) );
	deviceInfo.info.controllerId.vendorId = getShortProperty( device, CFSTR( kIOHIDVendorIDKey ) );
	deviceInfo.info.controllerId.productId = getShortProperty( device, CFSTR( kIOHIDProductIDKey ) );
	deviceInfo.portId = getIntProperty( device, CFSTR( kIOHIDLocationIDKey ) );

	// Mimic how SDL makes UUIDs
	ubyte uniqueId[16];
	std::memset( &uniqueId, 0, 16 );
	if( deviceInfo.info.controllerId.productId > 0 && deviceInfo.info.controllerId.vendorId > 0 ) {
		reinterpret_cast<ushort*>( uniqueId )[0] = getShortProperty( device, CFSTR( kIOHIDTransportKey ) );
		reinterpret_cast<ushort*>( uniqueId )[4] = deviceInfo.info.controllerId.vendorId;
		reinterpret_cast<ushort*>( uniqueId )[8] = deviceInfo.info.controllerId.productId;
		reinterpret_cast<ushort*>( uniqueId )[12] = getShortProperty( device, CFSTR( kIOHIDVersionNumberKey ) );
	} else {
		std::strncpy( (char*)uniqueId, deviceInfo.info.name.c_str(), 12 );
	}

	deviceInfo.info.uuid = Uuid( uniqueId );

	initInputMap( device, deviceInfo.inputMap );

	deviceInfo.info.numButtons = (ushort)deviceInfo.inputMap.button.size();
	deviceInfo.info.numAxes = 11;
	deviceInfo.info.numHats = (ushort)deviceInfo.inputMap.hat.size();

	deviceInfo.state.buttons.resize( deviceInfo.inputMap.button.size() );
	deviceInfo.state.axes.resize( 11 );
	deviceInfo.state.hats.resize( deviceInfo.inputMap.hat.size() );

	return deviceInfo;
}

static void deviceDiconnected( void *context, IOReturn, void* ) {
	const GamepadId gamepadId = (GamepadId)reinterpret_cast<std::intptr_t>( context );
	if( s_devices.count( gamepadId ) > 0 ) {
		QtThread::safeAsync( [=](){
			s_events.gamepadDisconnected({ gamepadId });
		});
	}
	s_devices.erase( gamepadId );
}

static void deviceConnected( void*, IOReturn, void*, IOHIDDeviceRef device ) {
	const GamepadId gamepadId = (GamepadId)getIntProperty( device, CFSTR( kIOHIDUniqueIDKey ) );

	if( s_devices.count( gamepadId ) > 0 ) return;

	if( IOHIDDeviceOpen( device, kIOHIDOptionsTypeNone ) != kIOReturnSuccess ) {
		return;
	}

	s_devices[gamepadId] = createHidDevice( device );

	IOHIDDeviceRegisterInputValueCallback( device, inputReceived, reinterpret_cast<void*>( (std::intptr_t)gamepadId ) );
	IOHIDDeviceRegisterRemovalCallback( device, deviceDiconnected, reinterpret_cast<void*>( (std::intptr_t)gamepadId ) );
	IOHIDDeviceScheduleWithRunLoop( device, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );

	const ControllerInfo info = s_devices[gamepadId].info;
	QtThread::safeAsync( [=](){
		GamepadConnectedEvent event = { gamepadId, info };
		s_events.gamepadConnected( event );
	});
}

static void appendMatcher(
	CFMutableArrayRef &array,
	uint page,
	uint usage
) {
	CFMutableDictionaryRef matcher = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
	CFNumberRef pageKey = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &page );
	CFNumberRef usageKey = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &usage );
	CFDictionarySetValue( matcher, CFSTR( kIOHIDDeviceUsagePageKey ), pageKey );
	CFDictionarySetValue( matcher, CFSTR( kIOHIDDeviceUsageKey ), usageKey );
	CFArrayAppendValue( array, matcher );
	CFRelease( pageKey );
	CFRelease( usageKey );
	CFRelease( matcher );
}

void HidDriver::start() {
	if( s_active ) return;

	s_hidManager = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDOptionsTypeNone );
	if( !s_hidManager ) return;

	IOHIDManagerSetDeviceMatching( s_hidManager, nullptr );
	IOHIDManagerScheduleWithRunLoop( s_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode );

	CFSetRef deviceSet = IOHIDManagerCopyDevices( s_hidManager );
	CFIndex numDevices = CFSetGetCount( deviceSet );
	std::vector<IOHIDDeviceRef> connectedDevices( numDevices );
	CFSetGetValues( deviceSet, (const void**)connectedDevices.data() );

	for( IOHIDDeviceRef &device : connectedDevices ) {
		if(
			!IOHIDDeviceConformsTo( device, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick ) &&
			!IOHIDDeviceConformsTo( device, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad )
		) continue;

		deviceConnected( nullptr, (IOReturn)0, nullptr, device );
	}

	CFMutableArrayRef deviceMatcher = CFArrayCreateMutable( kCFAllocatorDefault, 0,	&kCFTypeArrayCallBacks );
	appendMatcher( deviceMatcher, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick );
	appendMatcher( deviceMatcher, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad );
	IOHIDManagerSetDeviceMatchingMultiple( s_hidManager, deviceMatcher );
	IOHIDManagerRegisterDeviceMatchingCallback( s_hidManager, deviceConnected, nullptr );
	CFRelease( deviceMatcher );

	s_active = true;
}

void HidDriver::stop() {
	if( !s_active || !s_hidManager ) return;

	IOHIDManagerUnscheduleFromRunLoop( s_hidManager, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );
	IOHIDManagerClose( s_hidManager, kIOHIDOptionsTypeNone );
	CFRelease( s_hidManager );

	s_devices.clear();
	s_active = false;
}

bool HidDriver::isRunning() const {
	return s_active;
}

static HashMap<GamepadId,HidDevice> getConnectedDevices() {
	if( s_active ) return s_devices;

	HashMap<GamepadId,HidDevice> result;

	IOHIDManagerRef hidManager = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDOptionsTypeNone );
	if( hidManager == nullptr ) return {};
	IOHIDManagerSetDeviceMatching( hidManager, nullptr );

	CFSetRef deviceSet = IOHIDManagerCopyDevices( hidManager );
	CFIndex numDevices = CFSetGetCount( deviceSet );
	std::vector<IOHIDDeviceRef> connectedDevices( numDevices );
	CFSetGetValues( deviceSet, (const void**)connectedDevices.data() );

	for( IOHIDDeviceRef &device : connectedDevices ) {
		if(
			!IOHIDDeviceConformsTo( device, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick ) &&
			!IOHIDDeviceConformsTo( device, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad )
		) continue;

		const GamepadId gamepadId = (GamepadId)getIntProperty( device, CFSTR( kIOHIDUniqueIDKey ) );
		if( IOHIDDeviceOpen( device, kIOHIDOptionsTypeNone ) != kIOReturnSuccess ) continue;

		result[gamepadId] = createHidDevice( device );
		IOHIDDeviceClose( device, kIOHIDOptionsTypeNone );
	}

	IOHIDManagerClose( hidManager, kIOHIDOptionsTypeNone );
	CFRelease( hidManager );

	return result;
}

std::vector<ConnectedGamepad> HidDriver::getConnected() {
	std::vector<ConnectedGamepad> gamepads;
	if( s_active ) {
		gamepads.reserve( s_devices.size() );
		for( const auto &[ gamepadId, hidDevice ] : s_devices ) {
			gamepads.push_back({ gamepadId, hidDevice.info });
		}
	} else {
		const HashMap<GamepadId,HidDevice> devices = getConnectedDevices();
		for( const auto &[ gamepadId, hidDevice ] : devices ) {
			gamepads.push_back({ gamepadId, hidDevice.info });
		}
	}
	return gamepads;
}

GamepadState HidDriver::getState( GamepadId id ) const {
	const auto &i = s_devices.find( id );
	if( i == s_devices.end() ) return GamepadState();
	return i->second.state;
}

std::vector<Uuid> HidDriver::getDeviceOrder() {
	std::map<uint,Uuid> portMap;
	if( s_active ) {
		foreach_cvalue( device, s_devices ) {
			portMap[device.portId] = device.info.uuid;
		}
	} else {
		const HashMap<GamepadId,HidDevice> devices = getConnectedDevices();
		foreach_cvalue( device, devices ) {
			portMap[device.portId] = device.info.uuid;
		}
	}

	std::vector<Uuid> sortedDevices;
	sortedDevices.reserve( portMap.size() );
	foreach_cvalue( device, portMap ) {
		sortedDevices.push_back( std::move( device ) );
	}

	return sortedDevices;
}
