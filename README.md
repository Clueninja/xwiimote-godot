# godot-wiimote
A GDNative C++ project to add wiimote functionality to godot on linux

1: Using evtest find the path_suffix eg "26" for all devices you want to use.

2: Open the device you want to poll

3: Poll the device, because this is so generic, only returning an array of events, this could be expanded to any input device.

4: Figure out what the events mean using evtest and xwiishow to work out what each code and associated value means

