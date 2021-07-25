extends KinematicBody

var player1 = load("res://libxwiimote-godot.gdns").new()
var vel = Vector3()
var start_transform
var gvel = Vector3()

func _ready():
	player1.set_iface(1)
	player1.open_iface()
	start_transform = transform

func reset():
	vel=Vector3.ZERO
	gvel = Vector3.ZERO
	transform = start_transform




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	player1.poll_iface()
	#your rotation may be off, if so change the signs for direction of rotation 
	rotate_object_local(Vector3(0,0,1), -((player1.motionplus_Y())/20000)*PI*_delta)
	rotate_object_local(Vector3(1,0,0), -((player1.motionplus_Z())/20000)*PI*_delta)
	rotate_object_local(Vector3(0,1,0), ((player1.motionplus_X())/20000)*PI*_delta)



	var accelraw = Vector3( -player1.accel_X(), -player1.accel_Z(), player1.accel_Y() )
	if player1.button_A():
		reset()


	vel+=accelraw*_delta

	gvel=Vector3(0,95,0)

	
	translate_object_local(accelraw*_delta*_delta)
	
	global_translate(gvel*_delta*_delta)
