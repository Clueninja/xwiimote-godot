
extends RigidBody

var start_transform
var initvel
var initspin

func _ready():
	OS.window_fullscreen=true
	start_transform = transform
	initvel= linear_velocity
	initspin = angular_velocity 
	
	
	
func reset():
	linear_velocity= initvel
	transform = start_transform
	angular_velocity=initspin
	

func _input(event):
	if event.is_action_pressed("ui_cancel"):
		reset()
		
		
func _process(delta):
	if (global_transform.origin.x>10 || global_transform.origin.x<-10 || global_transform.origin.z>25 || global_transform.origin.z<-25 || global_transform.origin.y<0 || global_transform.origin.y>10):
		reset()
  
