extends VBoxContainer

@onready var resumeButton = $resume
var manual= preload("res://prefabs/UI/manual.tscn")
var manualInst
var title = ("res://scenes/levels/TitleScreen.tscn")
# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func _on_restart_pressed():
	hide()
	Musicplayer.setVolume(Musicplayer.defVol[global.curWorld])
	get_tree().paused=false
	global.player.die()




func _on_level_select_pressed():
	Musicplayer.buttonClicked()
	get_tree().paused=false
	global.exitToWorld()


func _on_manual_pressed():
	Musicplayer.buttonClicked()
	manualInst = manual.instantiate()
	manualInst.parent=self
	get_parent().add_child(manualInst)
	visible=false


func _on_quit_pressed():
	Musicplayer.buttonClicked()
	get_tree().paused=false
	Musicplayer.setVolume(Musicplayer.defVol[1])
	global.currentlevel=-1
	global.curWorld=1
	Musicplayer.update()
	get_tree().change_scene_to_file(title)


func _on_resume_pressed():
	Musicplayer.buttonClicked()
	hide()
	Musicplayer.setVolume(Musicplayer.defVol[global.curWorld])
	get_tree().paused=false
