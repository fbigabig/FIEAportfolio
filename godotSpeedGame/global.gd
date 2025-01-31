#singleton manager with global functions

extends Node

var time = 0
var currentlevel = -1
var player
var defVol
var lowVol= -30
var loaded=[false,false,false,false,false]
var worldLevels = []
var worldsUnlocked = [true,false,false,true]
var levels = {
	0: "res://scenes/levels/beginnerlevel.tscn",
	1 : "res://scenes/levels/newlevel1.tscn",
	2: "res://scenes/levels/newlevel3.tscn",
	3: "res://scenes/levels/newlevel2.tscn",
	4: "res://scenes/levels/newlevel5.tscn",
	100: "res://scenes/levels/newlevel4.tscn",
	5:"res://scenes/levels/level6.tscn",
	6: "res://scenes/levels/level7.tscn",
	7: "res://scenes/levels/level_conveyor.tscn",
	8: "res://scenes/levels/level_conveyor_2.tscn",
	9: "res://scenes/levels/level_bubble_easy.tscn",
	200: "res://scenes/levels/levelbubble.tscn",
	10: "res://scenes/levels/level_ice.tscn",
	11: "res://scenes/levels/level_icesuperbounce.tscn",
	12: "res://scenes/levels/level_superbounce.tscn",
	13: "res://scenes/levels/level_cannon_easy.tscn",
	14: "res://scenes/levels/level_cannon.tscn",
	300: "res://scenes/levels/world3chal.tscn",
	1000: "res://scenes/levels/superchal.tscn"
}
var worlds = {
	1: "res://scenes/worlds/world1.tscn",
	2: "res://scenes/worlds/world2.tscn",
	3: "res://scenes/worlds/world3.tscn",
	4: "res://scenes/worlds/world4.tscn",
}
var curWorld = 1
var levelEntryPos = {
	-1: Vector2(-256,153) #start pos, same for every world
}
var times = {}
var flooredTimes = { #goal times for levels
	0: 15,
	1: 15,
	2: 20,
	3: 20,
	4: 35,
	100: 40,
	5: 20,
	6: 35,
	7: 15,
	8: 20,
	200: 20,
	9: 30,
	10:35,
	11:15,
	12:25,
	13:35,
	14:20,
	300: 25,
	1000: 30
}
var devTimes = {
	0: 9.11,
	1: 7.13,
	2: 10.45,
	3: 9.31,
	4: 16.98,
	100: 18.81,
	5: 8.98,
	6: 14.25,
	7:3.43,
	8: 11.11,
	200: 7.39,
	9: 10.13,
	10: 15.40,
	11:6.78,
	12:11.41,
	13:12.74,
	14:7.89,
	300: 11.80,
	1000:13.5 
}
func exitToWorld(): #exit level 
	Musicplayer.setVolume(defVol)
	get_tree().change_scene_to_file(worlds[curWorld])
func newWorld(worldNum): #load new world
	defVol=Musicplayer.defVol[worldNum-1]
	curWorld=worldNum
	currentlevel=-1
	exitToWorld()
	Musicplayer.update()
func nextLevel(): #leave current level, save time
	if(time<times[currentlevel]||times[currentlevel]==0):
		times[currentlevel] = time
	
func setLevel(l):
	currentlevel=l
	

func _ready():
	pass




func _process(delta):
	pass
	
func save_func(): #save times for each leve and which worlds are unlocked to a file
	var yes= true
	#print(global.worldLevels)
	for level in global.worldLevels:
		if(level>99): continue
		if not (global.times[level]!=0.0):
			yes = false
	if(yes and curWorld!=4):
		worldsUnlocked[curWorld-1+1]=true
	var save_game = FileAccess.open("user://savegame.save", FileAccess.WRITE)
	save_game.store_line("savedata")
	save_game.store_line(str(times.size()))
	for i in times:
		save_game.store_line(str(i))
		save_game.store_line(str(times[i]).pad_decimals(2))
	for i in worldsUnlocked:
		save_game.store_line(str(i))
	save_game.close()
func load_func(): #read in a save data file and update data to match
	if not FileAccess.file_exists("user://savegame.save"):
		return 
	var save_game = FileAccess.open("user://savegame.save", FileAccess.READ)
	var test = save_game.get_line()
	if(test!="savedata"):
		return
	var num = int(save_game.get_line())

	for i in num:
		var level = int(save_game.get_line())
		times[level]=float(save_game.get_line())
	for i in worldsUnlocked.size():
		var tmp:bool = save_game.get_line()=="true"
		worldsUnlocked[i]=tmp


