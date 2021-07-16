extends KinematicBody


var player1 = load("res://bin/libxwiimote-godot.gdns").new()
var curacc =Vector3()
var preacc = Vector3()
#var curpos = Vector3()
#var prepos =Vector3()
# Called when the node enters the scene tree for the first time.

func _ready():

	player1.set_iface(1)
	player1.open_iface()
	player1.poll_iface()
	preacc = Vector3(player1.accel_X(),  player1.accel_Z(), -player1.accel_Y())
	#prerot = currot
	#curpos =Vector3(-player1.motionplus_X(), player1.motionplus_Y(), player1.motionplus_Z())
	#prepos = curpos


	#input.bind_device_event("motionplus", "29")
	#input.open_device("motionplus")




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	player1.poll_iface()
	
	curacc = Vector3(player1.accel_X(),  player1.accel_Z(), -player1.accel_Y())
	global_translate((curacc-preacc)*_delta*0.1) # only works if wiimote is flat, if tilted it will give bad results
	
	preacc = curacc

