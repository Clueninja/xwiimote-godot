extends KinematicBody


var player1 = load("res://libxwiimote-godot.gdns").new()
var curacc =Vector3()
var preacc = Vector3()

# Called when the node enters the scene tree for the first time.

func _ready():

	player1.set_iface(1)
	player1.open_iface()
	player1.poll_iface()
	preacc = Vector3(player1.accel_X(),  player1.accel_Z(), -player1.accel_Y())
	

# Called every frame. 'delta' is the elapsed time since the previous frame.

func _process(_delta):
	player1.poll_iface()
	
	curacc = Vector3(player1.accel_X(),  player1.accel_Z(), -player1.accel_Y())
	global_translate((curacc-preacc)*_delta*0.1) # only works if wiimote is flat, if tilted it will give bad results
	
	preacc = curacc

