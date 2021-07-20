# godot-wiimote
A GDNative C++ project to add wiimote functionality to godot on linux using the xwiimote kernal library

Requirements:
linux kernel 3.1 or newer (3.11 or newer recommended)
bluez-4.101 or newer (bluez-5.0 or newer recommended)

To compile download  [godot-cpp](https://github.com/godotengine/godot-cpp) and place in same directory as the src directory and Sconstruct file

then run 
"$scons platform=linux"
in the directory

In project settings, disable vsync to get the wiimote to work properly

To check for connected devices, use .list_devices() which will print out "Found device #" for each device connected

1: set the interface using the .set_iface(int num) method. This sets the interface for the object so acceleration data, button data, etc can be read

2: open the interface for reading .open_iface()

3: to set internal values you need to poll the device, use .poll_iface() then to access the data use,

.accel_X(), _Y(), _Z()

.button_A(), _B(), _LEFT(), _RIGHT(), _UP(), _DOWN(), _HOME(), _PLUS(), _MINUS(), _ONE(), _TWO() 

.ir_X(), _Y()

.motionplus_X(), _Y(), _Z()


[xwiimote website](https://dvdhrm.github.io/xwiimote)
