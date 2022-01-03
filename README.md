# XWiimote-Godot

XWiimote-Godot is a GNU/Linux shared library API for communication between godot GDscript to the xwiimote kernel API


# Requirements:
linux kernel 3.1 or newer (3.11 or newer recommended)

bluez-4.101 or newer (bluez-5.0 or newer recommended)

I'm not sure if your user needs to be in the input group in order to run this without escalating privileges.
if you experience issues add your user to the 'input' group.


## Prerequisites
xwiimote-2 (included in kernel since linux-3.1)

SCons

[godot-cpp](https://github.com/godotengine/godot-cpp)

### Setting up GDNative C++

Follow [these Instructions](https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-cpp-example.html)
until Creating a simple Plugin

## Building xwiimote-godot
place godot-cpp in same directory as the src directory and run
```bash
$ scons platform=linux target=release
```

 in the module folder.
 
This will create 'libxwiimote-godot.so' in the '/wiimote/bin/x11' directory which will execute when the library is loaded from gdscript.

To change the target directory to your program eg example edit the SConstruct file line 14 from 'wiimote/bin' to 'example/bin'

## Setting up your own project
In the wiimote controlled object load the godot native library

```js
var player = load("res://bin/libxwiimote-godot.gdns").new()
```
check if there are available devices using list_devices
```js
player.list_devices()
```

then set the xwiimote interface using set_iface, returns 0 if unsuccessful
```js
player.set_iface(playerid)
```
then open the interface using open_iface

```js
player.open_iface()
```

then poll the interface using poll_iface
```js
player.poll_iface()
```

this starts a new thread that edits the XWiimoteGodot object indefinetly until
```js
player.close_iface()
```
is called





## All XWiimoteGodot interfaces
```js

// miscellaneous methods
// sets interface pointer
.set_iface( playerid )

// opens interface to wiimote
.open_iface()

// creates a new detached thread then indefinately edits stored values
.poll_iface()

// closes interface
.close_iface()

.list_devices()

.get_battery()

.set_led()

.get_led()

.set_rumble()

.reset_all()

// wiimote methods for gdscript
// wiimote accelerometer gdscript methods
.wiimote_accel_X(), _Y(), _Z()

// wiimote button gdcript methods
.wiimote_button_A(), _B(), _LEFT(), _RIGHT(), _UP(), _DOWN(), _HOME(), _PLUS(), _MINUS(), _ONE(), _TWO() 

// wiimote infrared gdscript methods
.wiimote_ir_X(), _Y()

// wiimote motionplus gdscript methods
.wiimote_motionplus_X(), _Y(), _Z()

// wiimote absolute rotation methods
.wiimote_rotation_X(), _Y(), _Z()

// nunchuk methods for gdscript
// nunchuk accelerometer gdcript methods
.nunchuk_accel_X(), _Y(), _Z()

// nunchuk button gdcript methods
.nunchuk_button_C(), _Z()

// nunchuk analog stick gdcript methods
.nunchuk_analog_X(), _Y()

```

[xwiimote website](https://dvdhrm.github.io/xwiimote)
