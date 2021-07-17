 


#include <Godot.hpp>
#include <Reference.hpp>


#include <xwiimote.h>

//#include <errno.h>
//#include <fcntl.h>
//#include <inttypes.h>
//#include <math.h>
//#include <ncurses.h>
#include <poll.h>
//#include <stdarg.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <time.h>


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
        
        button.A=false;
        button.B=false;
        button.UP=false;
        button.DOWN=false;
        button.LEFT=false;
        button.RIGHT=false;
        button.HOME=false;
        button.MINUS=false;
        button.PLUS=false;
        button.ONE=false;
        button.TWO=false;
        
        
        accel.X=0;
        accel.Y=0;
        accel.Z=0;
        
        for (int i=0; i<4; i++){
            ir.X[i] = 0;
            ir.Y[i]=0;
        
        }
        
        motionplus.X=0;
        motionplus.Y=0;
        motionplus.Z=0;

        
    }
    

    void list_devices(){
        struct xwii_monitor *mon;
        char *ent;
        int num = 0;

        mon = xwii_monitor_new(false, false);
        if (!mon) {
            Godot::print("Cannot create monitor\n");
        }
        else{
            while ((ent = xwii_monitor_poll(mon))) {
                Godot::print("  Found device #", num, " : ", ent);
                ++num;
                std::free(ent);
            }

            xwii_monitor_unref(mon);
        }
    }

    int set_iface(int num){
        struct xwii_monitor *mon;
        char *ent;
        int i = 0;

        mon = xwii_monitor_new(false, false);
        if (!mon) {
            Godot::print("Cannot create monitor\n");
            return -1;
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
            return -1;
		}
		return 1;
    }

    int open_iface(){
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
        return 1;

    }

    void close_iface(){
        xwii_iface_unref(iface);
    }

    void poll_iface(){
        struct xwii_event event;

		int ret = xwii_iface_dispatch(iface, &event, sizeof(event));
		if (ret) {
			if (ret != -EAGAIN) {
				Godot::print("Error: Read failed with err");
			}
		}
		else {
			switch (event.type) {
                case XWII_EVENT_GONE:
                    Godot::print("Info: Device gone");
                    break;
                    
                    
                case XWII_EVENT_WATCH:
                    // i dont know how to deal with this
                    ret = xwii_iface_open(iface, xwii_iface_available(iface) | XWII_IFACE_WRITABLE);
                    if (ret==-1){
                        Godot::print("Error: Cannot open interface");
                    }
                    break;
                    
                    
                case XWII_EVENT_KEY:                    
                    switch(event.v.key.code){
                        case XWII_KEY_A:
                            if (event.v.key.state)
                                button.A=true;
                            else
                                button.A=false;
                            break;
                        case XWII_KEY_B:
                            if (event.v.key.state)
                                button.B=true;
                            else
                                button.B=false;
                            break;
                        case XWII_KEY_UP:
                            if (event.v.key.state)
                                button.UP=true;
                            else
                                button.UP=false;
                            break;
                        case XWII_KEY_DOWN:
                            if (event.v.key.state)
                                button.DOWN=true;
                            else
                                button.DOWN=false;
                            break;
                        case XWII_KEY_LEFT:
                            if (event.v.key.state)
                                button.LEFT=true;
                            else
                                button.LEFT=false;
                            break;
                        case XWII_KEY_RIGHT:
                            if (event.v.key.state)
                                button.RIGHT=true;
                            else
                                button.RIGHT=false;
                            break;
                        case XWII_KEY_HOME:
                            if (event.v.key.state)
                                button.HOME=true;
                            else
                                button.HOME=false;
                            break;
                        case XWII_KEY_PLUS:
                            if (event.v.key.state)
                                button.PLUS=true;
                            else
                                button.PLUS=false;
                            break;
                        case XWII_KEY_MINUS:
                            if (event.v.key.state)
                                button.MINUS=true;
                            else
                                button.MINUS=false;
                            break;
                        case XWII_KEY_ONE:
                            if (event.v.key.state)
                                button.ONE=true;
                            else
                                button.ONE=false;
                            break;
                        case XWII_KEY_TWO:
                            if (event.v.key.state)
                                button.TWO=true;
                            else
                                button.TWO=false;
                            break;
                        
                    }
                    break;
                    
                
                 
                case XWII_EVENT_ACCEL:
                    accel.X = event.v.abs[0].x;
                    accel.Y = event.v.abs[0].y;
                    accel.Z = event.v.abs[0].z;
                    break;
                    
                    
                    
                    
                case XWII_EVENT_IR:
                    for(int i=0; i<4;i++){
                        if (xwii_event_ir_is_valid(&event.v.abs[i])) {
                            ir.X[i] =  event.v.abs[i].x;
                            ir.X[i] = event.v.abs[i].y;
                        }
                    }
                    break;
                    
                    
                case XWII_EVENT_MOTION_PLUS:
                    motionplus.X = event.v.abs[0].x;
                    motionplus.Y = event.v.abs[0].y;
                    motionplus.Z = event.v.abs[0].z;
                    break;
			
                
                
                }
            }
    }


    bool button_A(){
        return button.A;
    }
    bool button_B(){
        return button.B;
    }
    bool button_UP(){
        return button.UP;
    }
    bool button_DOWN(){
        return button.DOWN;
    }
    bool button_LEFT(){
        return button.LEFT;
    }
    bool button_RIGHT(){
        return button.RIGHT;
    }
    bool button_HOME(){
        return button.HOME;
    }
    bool button_PLUS(){
        return button.PLUS;
    }
    bool button_MINUS(){
        return button.MINUS;
    }
    bool button_ONE(){
        return button.ONE;
    }
    bool button_TWO(){
        return button.TWO;
    }
    
    int accel_X(){
        return accel.X;
    }
    int accel_Y(){
        return accel.Y;
    }
    int accel_Z(){
        return accel.Z;
    }
    
    int ir_X(){
        return ir.X[1];
    }
    int ir_Y(){
        return ir.Y[1];
    }
    
    int motionplus_X(){
        return motionplus.X;
    }
    int motionplus_Y(){
        return motionplus.Y;
    }
    int motionplus_Z(){
        return motionplus.Z;
    }



    struct my_button{
        bool A,B,UP,DOWN,LEFT,RIGHT,HOME,PLUS,MINUS,ONE,TWO;
    };

    struct my_accel {
        int X,Y,Z;
    };

    struct my_ir {
        int X[4], Y[4];
    };

    struct my_motionplus {
        int X,Y,Z;
    };

    struct xwii_iface *iface;
    struct my_button button;
    
    struct my_accel accel;
    
    struct my_ir ir;
    
    struct my_motionplus motionplus;

    String _name;
    
    
    static void _register_methods(){
        
        register_method ("open_iface", &XWiimoteGodot::open_iface);
        register_method ("poll_iface", &XWiimoteGodot::poll_iface);
        register_method ("close_iface", &XWiimoteGodot::close_iface);
        register_method ("set_iface",  &XWiimoteGodot::set_iface);
        register_method ("list_devices", &XWiimoteGodot::list_devices);

        register_method ("button_A", &XWiimoteGodot::button_A);
        register_method ("button_B", &XWiimoteGodot::button_B);
        register_method ("button_UP", &XWiimoteGodot::button_UP);
        register_method ("button_DOWN", &XWiimoteGodot::button_DOWN);
        register_method ("button_LEFT", &XWiimoteGodot::button_LEFT);
        register_method ("button_RIGHT", &XWiimoteGodot::button_RIGHT);
        register_method ("button_HOME", &XWiimoteGodot::button_HOME);
        register_method ("button_PLUS", &XWiimoteGodot::button_PLUS);
        register_method ("button_MINUS", &XWiimoteGodot::button_MINUS);
        register_method ("button_ONE", &XWiimoteGodot::button_ONE);
        register_method ("button_TWO", &XWiimoteGodot::button_TWO);
        
        register_method ("accel_X", &XWiimoteGodot::accel_X);
        register_method ("accel_Y", &XWiimoteGodot::accel_Y);
        register_method ("accel_Z", &XWiimoteGodot::accel_Z);
    
        register_method ("ir_X", &XWiimoteGodot::ir_X);
        register_method ("ir_Y", &XWiimoteGodot::ir_Y);
        
        register_method ("motionplus_X", &XWiimoteGodot::motionplus_X);
        register_method ("motionplus_Y", &XWiimoteGodot::motionplus_Y);
        register_method ("motionplus_Z", &XWiimoteGodot::motionplus_Z);
        


    }
};

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
