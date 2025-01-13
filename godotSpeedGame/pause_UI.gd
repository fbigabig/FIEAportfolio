extends Node2D
var initGrab = false
var pausePressed=false
var paused=false

@onready var UI = $buttonUI
func _input(event):
	if(get_tree().paused):
		if(event is InputEventKey or event is InputEventMouse and is_instance_valid(global.player)):
			Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED)
			global.player.useMouse=true
		elif(event is InputEventJoypadButton or event is InputEventJoypadMotion and is_instance_valid(global.player)):
			if(!initGrab): 
				UI.resumeButton.grab_focus()
				initGrab=true
			Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)
			global.player.useMouse=false
	if(event.is_action_pressed("pause") and !pausePressed):
		pausePressed=true
		paused=false
		if(get_tree().paused):
			UI.hide()
			get_tree().paused=false
			Musicplayer.setVolume(Musicplayer.defVol[global.curWorld])
			if(is_instance_valid(UI.manualInst)):
				UI.manualInst.queue_free()
		elif(is_instance_valid(global.player)):
			pausePressed=false
			paused=true
			#Musicplayer.buttonClicked()
			UI.show()
			initGrab = false
			get_tree().paused=true
			#Musicplayer.update()
			Musicplayer.setVolume(global.lowVol)
	if(event.is_action_released("pause")):
		pausePressed=false

# Called when the node enters the scene tree for the first time.
func _ready():
	UI.hide()


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
