 
/*
Marked Sections of code may be under the XWiimote License:

Copyright (c) 2011-2013 David Herrmann <dh.herrmann@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
 

#include <Godot.hpp>
#include <Reference.hpp>
#include <Array.hpp>
#include <thread>

#include <xwiimote.h>

#include <poll.h>



using namespace godot;


class XWiimoteGodot: public Reference{
    GODOT_CLASS(XWiimoteGodot, Reference);
public:
    
    
    XWiimoteGodot(){}


    void _init(){
        
        _name = String("XWiimoteGodot");
        
        reset_all();
    }

    void reset_all(){
        
        wiimote.button={false,false,false, false, false,false,false,false, false, false,false};
        
        
        wiimote.accel={0,0,0};
        
        for (int i=0; i<4; i++){
            wiimote.ir.X[i] = 0;
            wiimote.ir.Y[i]=0;
        }
        
        
        wiimote.motionplus={0,0,0,false};
        
        wiimote.rotation={0,0,0};
        
        nunchuk.accel={0,0,0};
  
                
        nunchuk.button={false, false};
        
        nunchuk.analog={false,false};
        

        
    }
    

    void list_devices(){
        //from https://github.com/dvdhrm/xwiimote/blob/master/tools/xwiishow.c enumerate()
        struct xwii_monitor *mon;
        char *ent;
        int num = 0;

        mon = xwii_monitor_new(false, false);
        if (!mon) {
            Godot::print("Cannot create monitor\n");
        }
        else{
            while ((ent = xwii_monitor_poll(mon))) {
                Godot::print(" Found device #", num, " : ", ent);
                ++num;
                std::free(ent);
            }

            xwii_monitor_unref(mon);
        }
    }
    Array get_led(){
        Array led_state_array = Array();
        for (int led=1;led<5;led++){
            bool state;
            xwii_iface_get_led(iface,led,&state);
            led_state_array.append(state);
        }
        return led_state_array;
    }

    int get_battery(){
        uint8_t capacity;
        xwii_iface_get_battery(iface, &capacity);
        return (int) capacity;
    }
    

    void set_led(Array led_state_array){
        for (int led=1;led<5;led++){
            bool state = led_state_array[led-1];
            xwii_iface_set_led(iface,led,state);
        }
    }
	// set rumble to state
    void set_rumble(bool state){
        xwii_iface_rumble(iface, state);
    }
	// method to set the interface to availiable device
    int set_iface(int num){
        //from https://github.com/dvdhrm/xwiimote/blob/master/tools/xwiishow.c lines 2684-2689 including get_dev()
        
        struct xwii_monitor *mon;
        char *ent;
        int i = 0;

        mon = xwii_monitor_new(false, true);
        if (!mon) {
            Godot::print("Cannot create monitor\n");
            return 0;
        }

        while ((ent = xwii_monitor_poll(mon))) {
            if (++i == num)
                break;
            std::free(ent);
        }

        xwii_monitor_unref(mon);

        if (!ent)
            Godot::print("Cannot find device with number ", num);

        int ret = xwii_iface_new(&iface, ent);
        
		std::free(ent);
        
		if (ret) {
			Godot::print("Cannot create xwii_iface");
            return 0;
		}
		return 1;
    }
	// method to open any availiable interfaces/
    int open_iface(){
        // from https://github.com/dvdhrm/xwiimote/blob/master/tools/xwiishow.c lines 2713-2717
        int ret = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);
        if (ret){
            Godot::print("Error: Cannot open interface");
            return -1;
            
        }
        
        ret = xwii_iface_watch(iface, true);
        if (ret){
            Godot::print("Error: Cannot initialize hotplug watch descriptor");
            return -1;
        }
        reset_all();
        return 1;

    }
	// simple method to unreference the pointer
    void close_iface(){           
        xwii_iface_unref(iface);
        running = false;
        
    }
    
    
    static void thread_poll(XWiimoteGodot* w){
    	struct pollfd fds[2];
    	int ret;

		memset(fds, 0, sizeof(fds));
		fds[0].fd = 0;
		fds[0].events = POLLIN;
		fds[1].fd = xwii_iface_get_fd(w->iface);
		fds[1].events = POLLIN;
		int fds_num = 2;
		
		
    	while (w->running){
			struct xwii_event event;
			
			ret = poll(fds, fds_num, -1);
			if (ret < 0) {
				if (errno != EINTR) {
					ret = -errno;
					Godot::print("Error: Cannot poll fds");
					break;
				}
			}

		    ret = xwii_iface_dispatch(w->iface, &event, sizeof(event));
		    
		    //from https://github.com/dvdhrm/xwiimote/blob/master/tools/xwiishow.c lines 2521-2526
		    
		    if (ret) {
		        if (ret != -EAGAIN) {
		            Godot::print("Error: Read failed with err");
		        }
		    }
		    else {
		        switch (event.type) {
		        // if wiimote has gone
		        case XWII_EVENT_GONE:
		            Godot::print("Info: Device gone");
		            break;
		            
		        // if wiimote has appeared
		        case XWII_EVENT_WATCH:
		            // i dont know how to deal with this
		            //from https://github.com/dvdhrm/xwiimote/blob/master/tools/xwiishow.c handle_watch()
		            
		            ret = xwii_iface_open(w->iface, xwii_iface_available(w->iface) | XWII_IFACE_WRITABLE);
		            if (ret==-1){
		                Godot::print("Error: Cannot open interface");
		            }
		            break;
		            
		        // if wiimote key is pressed
		        case XWII_EVENT_KEY:                    
		            switch(event.v.key.code){
		            // if key A event
		            case XWII_KEY_A:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.A=true;
		                else
		                    w->wiimote.button.A=false;
		                break;
		            // if key B event
		            case XWII_KEY_B:
		                if (event.v.key.state)
		       				// if pressed
		                    w->wiimote.button.B=true;
		                else
		                    w->wiimote.button.B=false;
		                break;
		            // if key up event
		            case XWII_KEY_UP:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.UP=true;
		                else
		                    w->wiimote.button.UP=false;
		                break;
		            // if key down event
		            case XWII_KEY_DOWN:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.DOWN=true;
		                else
		                    w->wiimote.button.DOWN=false;
		                break;
		            // if key left event
		            case XWII_KEY_LEFT:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.LEFT=true;
		                else
		                    w->wiimote.button.LEFT=false;
		                break;
		            // if key right event
		            case XWII_KEY_RIGHT:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.RIGHT=true;
		                else
		                    w->wiimote.button.RIGHT=false;
		                break;
		            // if key home event
		            case XWII_KEY_HOME:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.HOME=true;
		                else
		                    w->wiimote.button.HOME=false;
		                break;
		            // if key plus event
		            case XWII_KEY_PLUS:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.PLUS=true;
		                else
		                    w->wiimote.button.PLUS=false;
		                break;
		            // if key minus event
		            case XWII_KEY_MINUS:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.MINUS=true;
		                else
		                    w->wiimote.button.MINUS=false;
		                break;
		            // if key one event
		            case XWII_KEY_ONE:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.ONE=true;
		                else
		                    w->wiimote.button.ONE=false;
		                break;
		            // if key two event
		            case XWII_KEY_TWO:
		                if (event.v.key.state)
		                // if pressed
		                    w->wiimote.button.TWO=true;
		                else
		                    w->wiimote.button.TWO=false;
		                break;
		                
		            }
		            break;
		            
		        
		        // if acceleration event
		        case XWII_EVENT_ACCEL:
		        // set wiimote accel values to event abs values
		            w->wiimote.accel.X = event.v.abs[0].x;
		            w->wiimote.accel.Y = event.v.abs[0].y;
		            w->wiimote.accel.Z = event.v.abs[0].z;
		            break;
		            
		            
		            
		            
		        // if infrared event
		        case XWII_EVENT_IR:
		            for(int i=0; i<4;i++){
		            // if valid ir event
		                if (xwii_event_ir_is_valid(&event.v.abs[i])) {
		                // set internal values
		                    w->wiimote.ir.X[i] =  event.v.abs[i].x;
		                    w->wiimote.ir.X[i] = event.v.abs[i].y;
		                }
		                
		                else{
		                // set to bad values
		                    w->wiimote.ir.X[i] =  -1;
		                    w->wiimote.ir.X[i] = -1;
		                }
		            }
		            break;
		            
		        // if wiimote motionplus event
		        case XWII_EVENT_MOTION_PLUS:
		        // set wiimote internal values
		        	// normalise wiimote motionplus values
		        	
		        	if (w->wiimote.motionplus.reset){
		        		w->wiimote.motionplus.reset = false;
		        		int x,y,z,factor;
		        		
		        		if (event.v.abs[0].x>5000 || event.v.abs[0].y>5000 || event.v.abs[0].y>5000){
		        			w->wiimote.motionplus.reset = true;
		        			xwii_iface_get_mp_normalization(w->iface, &x, &y, &z, &factor);
							x = event.v.abs[0].x + x;
							y = event.v.abs[0].y + y;
							z = event.v.abs[0].z + z;
							xwii_iface_set_mp_normalization(w->iface, x, y, z, factor);
		        		}
		        	}
		            w->wiimote.motionplus.X = event.v.abs[0].x;
		            w->wiimote.motionplus.Y = event.v.abs[0].y;
		           	w->wiimote.motionplus.Z = event.v.abs[0].z;
		           	
		           	
		           	w->wiimote.rotation.X += event.v.abs[0].x;
		            w->wiimote.rotation.Y += event.v.abs[0].y;
		           	w->wiimote.rotation.Z += event.v.abs[0].z;
		            break;
		        
		            
		        // if nunchuk key event
		        case XWII_EVENT_NUNCHUK_KEY:
		        // could do switch case here but it doesnt make much sense
		        // if nunchuk key is C
		            if (event.v.key.code==XWII_KEY_C){
		                w->nunchuk.button.C = event.v.key.state;
		            }
		        // if nunchuk key is Z
		            else if (event.v.key.code==XWII_KEY_Z){
		                w->nunchuk.button.Z = event.v.key.state;
		            }
		            break;
		        // if nunchuk move event
		        case XWII_EVENT_NUNCHUK_MOVE:
		        
		        // set values for acceleration for nunchuck
		            w->nunchuk.accel.X=event.v.abs[1].x;
		            w->nunchuk.accel.Y=event.v.abs[1].y;
		            w->nunchuk.accel.Z=event.v.abs[1].z;
		            
		        // set values for analog
		            w->nunchuk.analog.X=event.v.abs[0].x;
		            w->nunchuk.analog.Y=event.v.abs[0].y;
		            break;
		        
		        
		        
		        
		      }
		    }
        }
        Godot::print("Wiimote disconnected");
	}
    
    // sarts the input thread2
    void poll_iface(){
        
   		running = true;
        
        std::thread XWiimoteInput( &XWiimoteGodot::thread_poll, this);
        XWiimoteInput.detach();

        
    }

	
    
// I think separate data for nunchuk and wiimote makes sense
    struct{
    // boolean button data
        struct {
            bool A,B,UP,DOWN,LEFT,RIGHT,HOME,PLUS,MINUS,ONE,TWO;
        } button;
	// integer accel data, could be double/float
        struct {
            int X,Y,Z;
        } accel;
	// integer arrays for ir data (up to 4 points)
        struct {
            int X[4], Y[4];
        } ir;
	// integer values for motionplus rotation
        struct {
            int X,Y,Z;
            bool reset;
        } motionplus;
        
        struct {
            int X,Y,Z;
        } rotation;
        
    }wiimote;
    
    
    // nunchuk struct data
    struct {
    	// nunchuk accelerometer data
        struct {
            int X,Y,Z;
        } accel;
        // nunchuk analog data
        struct {
            int X,Y;
        } analog;
        // nunchuk button/key data
        struct {
            bool C,Z;
        } button;
    } nunchuk;
    
    // interface pointer to be initialised
    struct xwii_iface *iface;
    // running boolean for closing the input thread
    bool running;
   
	// idk
    String _name;
    
    // register methods for gdscript methods
    
    
    // absolutely horrible but it works, 
	// wiimote getter functions for gdscript interface
    bool wiimote_button_A(){return wiimote.button.A;}
    bool wiimote_button_B(){return wiimote.button.B;}
    bool wiimote_button_UP(){return wiimote.button.UP;}
    bool wiimote_button_DOWN(){return wiimote.button.DOWN;}
    bool wiimote_button_LEFT(){return wiimote.button.LEFT;}
    bool wiimote_button_RIGHT(){return wiimote.button.RIGHT;}
    bool wiimote_button_HOME(){return wiimote.button.HOME;}
    bool wiimote_button_PLUS(){return wiimote.button.PLUS;}
    bool wiimote_button_MINUS(){return wiimote.button.MINUS;}
    bool wiimote_button_ONE(){return wiimote.button.ONE;}
    bool wiimote_button_TWO(){return wiimote.button.TWO;}
    
    int wiimote_accel_X(){return wiimote.accel.X;}
    int wiimote_accel_Y(){return wiimote.accel.Y;}
    int wiimote_accel_Z(){return wiimote.accel.Z;}
    
    int wiimote_ir_X(){return wiimote.ir.X[1];}
    int wiimote_ir_Y(){return wiimote.ir.Y[1];}
    
    int wiimote_motionplus_X(){int x =wiimote.motionplus.X; /* wiimote.motionplus.X=0 ;*/ return x;}
    int wiimote_motionplus_Y(){int y =wiimote.motionplus.Y; /* wiimote.motionplus.Y=0; */ return y;}
    int wiimote_motionplus_Z(){int z =wiimote.motionplus.Z; /* wiimote.motionplus.Z=0; */ return z;}
    
    int wiimote_rotation_X(){int x =wiimote.rotation.X; /* wiimote.motionplus.X=0 ;*/ return x;}
    int wiimote_rotation_Y(){int y =wiimote.rotation.Y; /* wiimote.motionplus.Y=0; */ return y;}
    int wiimote_rotation_Z(){int z =wiimote.rotation.Z; /* wiimote.motionplus.Z=0; */ return z;}
    
    // nunchuck getter functions
    
    int nunchuk_accel_X(){return nunchuk.accel.X;}
    int nunchuk_accel_Y(){return nunchuk.accel.Y;}
    int nunchuk_accel_Z(){return nunchuk.accel.Z;}
    
    int nunchuk_analog_X(){return nunchuk.analog.X;}
    int nunchuk_analog_Y(){return nunchuk.analog.Y;}
    
    bool nunchuk_button_C(){return nunchuk.button.C;}
    bool nunchuk_button_Z(){return nunchuk.button.Z;}
    
    
    
    static void _register_methods(){
        
        // miscellaneous methods
        // sets interface pointer
        register_method ("set_iface",  &XWiimoteGodot::set_iface);
        // opens interface to wiimote
        register_method ("open_iface", &XWiimoteGodot::open_iface);
        // creates a new detached thread then indefinately edits stored values
        register_method ("poll_iface", &XWiimoteGodot::poll_iface);
        // closes interface, should prob use this
        register_method ("close_iface", &XWiimoteGodot::close_iface);
        // lists all connected devices, used for debugging
        register_method ("list_devices", &XWiimoteGodot::list_devices);
        // gets wiimote battery
        register_method("get_battery", &XWiimoteGodot::get_battery);
        // led methods (godot program needs sudo privilages)
        register_method("set_led", &XWiimoteGodot::set_led);
        register_method("get_led", &XWiimoteGodot::get_led);
        // set rumble state
        register_method("set_rumble", &XWiimoteGodot::set_rumble);
        //reset all internal values to initial values
        register_method("reset_all", &XWiimoteGodot::reset_all);


		// wiimote methods for gdscript
		// wiimote button gdcript methods
        register_method ("wiimote_button_A", &XWiimoteGodot::wiimote_button_A);
        register_method ("wiimote_button_B", &XWiimoteGodot::wiimote_button_B);
        register_method ("wiimote_button_UP", &XWiimoteGodot::wiimote_button_UP);
        register_method ("wiimote_button_DOWN", &XWiimoteGodot::wiimote_button_DOWN);
        register_method ("wiimote_button_LEFT", &XWiimoteGodot::wiimote_button_LEFT);
        register_method ("wiimote_button_RIGHT", &XWiimoteGodot::wiimote_button_RIGHT);
        register_method ("wiimote_button_HOME", &XWiimoteGodot::wiimote_button_HOME);
        register_method ("wiimote_button_PLUS", &XWiimoteGodot::wiimote_button_PLUS);
        register_method ("wiimote_button_MINUS", &XWiimoteGodot::wiimote_button_MINUS);
        register_method ("wiimote_button_ONE", &XWiimoteGodot::wiimote_button_ONE);
        register_method ("wiimote_button_TWO", &XWiimoteGodot::wiimote_button_TWO);
        // wiimote accelerometer gdscript methods
        register_method ("wiimote_accel_X", &XWiimoteGodot::wiimote_accel_X);
        register_method ("wiimote_accel_Y", &XWiimoteGodot::wiimote_accel_Y);
        register_method ("wiimote_accel_Z", &XWiimoteGodot::wiimote_accel_Z);
    	// wiimote infrared gdscript methods
        register_method ("wiimote_ir_X", &XWiimoteGodot::wiimote_ir_X);
        register_method ("wiimote_ir_Y", &XWiimoteGodot::wiimote_ir_Y);
        // wiimote motionplus getter gdscript methods
        register_method ("wiimote_motionplus_X", &XWiimoteGodot::wiimote_motionplus_X);
        register_method ("wiimote_motionplus_Y", &XWiimoteGodot::wiimote_motionplus_Y);
        register_method ("wiimote_motionplus_Z", &XWiimoteGodot::wiimote_motionplus_Z);
        
        
        register_method ("wiimote_rotation_X", &XWiimoteGodot::wiimote_rotation_X);
        register_method ("wiimote_rotation_Y", &XWiimoteGodot::wiimote_rotation_Y);
        register_method ("wiimote_rotation_Z", &XWiimoteGodot::wiimote_rotation_Z);
        // nunchuk methods for gdscript
        // nunchuk accelerometer getter methods for gdscript
        register_method ("nunchuk_accel_X", &XWiimoteGodot::nunchuk_accel_X);
        register_method ("nunchuk_accel_Y", &XWiimoteGodot::nunchuk_accel_Y);
        register_method ("nunchuk_accel_Z", &XWiimoteGodot::nunchuk_accel_Z);
        // nunchuck button getter methods for gdscript
        register_method ("nunchuk_button_C", &XWiimoteGodot::nunchuk_button_C);
        register_method ("nunchuk_button_Z", &XWiimoteGodot::nunchuk_button_Z);
        // nunchuck analog getter methods for gdscript
        register_method ("nunchuk_analog_X", &XWiimoteGodot::nunchuk_analog_X);
        register_method ("nunchuk_analog_Y", &XWiimoteGodot::nunchuk_analog_Y);
        

    }
};

// gdnative initialisation I dont understand

/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
	godot::Godot::gdnative_init(o);
}

/** GDNative Terminate **/
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
	godot::Godot::gdnative_terminate(o);
}

/** NativeScript Initialize **/
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
	godot::Godot::nativescript_init(handle);
	godot::register_class<XWiimoteGodot>();
}
