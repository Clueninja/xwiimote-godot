extends KinematicBody

export var playerid = int()

var player = load("res://bin/libxwiimote-godot.gdns").new()
var start_transform
var preA=false
var playeron = false
var startnunchuk

func _ready():
	if player.set_iface(playerid):
		playeron=true
		player.open_iface()
		print(player.get_battery())
		startnunchuk = Vector3(-player.nunchuk_analog_X(), player.nunchuk_analog_Y(), 0)
		player.poll_iface(2)
		
		

		
	start_transform = transform
	

func reset():

	transform = start_transform
	startnunchuk = Vector3(-player.nunchuk_analog_X(), player.nunchuk_analog_Y(), 0)
	print(startnunchuk)
	player.reset_all()  

	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	if playeron:
		#player.poll_iface(2000)
		transform.basis = Basis( Quat(
			-(player.wiimote_rotation_Z()/20000) * PI * _delta,
			-(player.wiimote_rotation_X()/20000) * PI * _delta, 
			 (player.wiimote_rotation_Y()/20000) * PI * _delta,
		1))
		
		#print(Vector3(
	#		player.wiimote_rotation_Z(),
	#		player.wiimote_rotation_X(),
	#		player.wiimote_rotation_Y()
	#	))
		
		
		#rotate_object_local(Vector3(1,0,0), -((player.wiimote_motionplus_Z())/20000)*PI*_delta)
		#rotate_object_local(Vector3(0,1,0), -((player.wiimote_motionplus_X())/20000)*PI*_delta)
		#rotate_object_local(Vector3(0,0,1), ((player.wiimote_motionplus_Y())/20000)*PI*_delta)


		
		#if player.wiimote_button_B():
			#player.set_rumble(true)
		#else :
			#player.set_rumble(false)
		#if pressed
		if player.wiimote_button_A() and not preA:
			preA=true
		#if released
		if not player.wiimote_button_A() and preA:
			preA=false
			reset()
			
		
		if player.wiimote_button_HOME() or Input.is_action_pressed("ui_quit"):
			player.close_iface()
			get_tree().quit()
		
		var accel = Vector3(
			player.wiimote_accel_X(),
			player.wiimote_accel_Y(),
			player.wiimote_accel_Z()
		)
		# rotate accel by absolute rotation
		# subtract gravity
		# move bat
		
		
		var nunchuk = Vector3(
			-player.nunchuk_analog_X(),
			player.nunchuk_analog_Y(),
			0)
			
		nunchuk = nunchuk-startnunchuk
		global_translate(nunchuk/10000)
		

