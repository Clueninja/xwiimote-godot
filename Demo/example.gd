
var player1 = load("res://libxwiimote-godot.gdns").new()
var vel = Vector3(0,0,0)

func _ready():
	player1.set_iface(1)
	player1.open_iface()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	player1.poll_iface()
	vel+= Vector3(player1.accel_X(),  player1.accel_Z()-95, -player1.accel_Y())*_delta*0.5
	
	global_translate(vel*_delta*0.1) # only works if wiimote is flat, if tilted it will give bad results lol
	
