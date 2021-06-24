 

#include <Godot.hpp>
#include <Reference.hpp>
#include <String.hpp>
#include <Array.hpp>
#include <Variant.hpp>

using namespace godot;

struct input_event {
      struct timeval time;
      unsigned short type;
      unsigned short code;
      unsigned int value;
};

struct device{
	String name;
	String path_suffix;
	bool is_open = false;
	FILE *stream;

};

class Wiimote: public Reference{
    GODOT_CLASS(Wiimote, Reference);

private:
    float time_passed;
    String event_path = "/dev/input/event";
    device devices[4];
    String _name;
    
    
public:
    void _init() {
		_name = String("Wiimote");
        
        devices[0].name =  String("accel");
        devices[1].name =  String("ir_sensor");
        devices[2].name = String("buttons");
        devices[3].name = String("motionplus");
        
	}

    Array poll(String device_name){ //needs to be extremaly fast, no conversions between types, lookup table for O(1)?
        //std::string str_dev_name = device_name.ascii().get_data(); could be slow#
        Array meta_ret;
        PoolIntArray ret;
        ret.append(0);
        ret.append(0);
        ret.append(0);
        for (int index=0; index<4; index++){
            
            // could get rid of the for loop somehow?
            if(devices[index].name ==device_name && devices[index].is_open){
                
                //what about multiple simultanious events?
                struct input_event ie[4];
                if (fread(&ie, sizeof(struct input_event), 4, devices[index].stream)) {
                    for (int ie_index=0; ie_index<4; ie_index++ ){
                        PoolIntArray temp;
                        
                        temp.append((int)ie[ie_index].type);
                        temp.append((int)ie[ie_index].code);
                        temp.append((int)ie[ie_index].value);
                        meta_ret.append(temp);
                    }
                    return meta_ret;
                }

            }
        }
        meta_ret.append(ret);
        return meta_ret;
    }

    // register all methods here associated in the header
    static void _register_methods(){
        register_method ("bind_device_event", &Wiimote::bind_device_event);
        register_method ("poll", &Wiimote::poll);
        register_method ("open_device", &Wiimote::open_device);
        register_method ("close_device",  &Wiimote::close_device);


    }


    void bind_device_event(String device_name, String path_suffix){
        for (int index = 0; index<4; index++){
            if (devices[index].name == device_name && !devices[index].is_open){
                devices[index].path_suffix = path_suffix;
            
            }
        }
    }

    
    void open_device(String device_name){
        for (int index=0; index<4; index++){
            if(devices[index].name == device_name && !devices[index].is_open){
                // yes this isnt great                
                
                String full_path = event_path+devices[index].path_suffix;
            
                // FILE fopen(const char* path, const char* mode); to my knowledge
                // so some convertion needs to take place for concatenation
                devices[index].stream = fopen(full_path.ascii().get_data(), "r");
                devices[index].is_open = true;
            }
        }

    }


    void close_device(String device_name){

        for (int index=0; index<4; index++){
            if(devices[index].name == device_name && devices[index].is_open){
                fclose(devices[index].stream);
                devices[index].is_open = false;
                
            }
        }
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

	godot::register_class<Wiimote>();
    
    
}
