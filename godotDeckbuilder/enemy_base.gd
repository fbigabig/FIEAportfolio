#base class all enemies inherit from

extends Area2D
@export var speed = 5
@export var hp = 250
var iceTime = 3
var tile =[0,0] #this and oldtile store xy tile indexes
var oldTile = [0,0]
var occupied = []
var moving = false
var dist = 0

@onready var ice = preload("res://spellstuff/ice_indicator.tscn") #ice sprite to overlay when frozen

func _ready():
	occupied.append(tile)
	oldTile[0]=tile[0]
	oldTile[1]=tile[1]

func addOccupied(newTile): 
	occupied.append(newTile)
func doneMoving(): #end movement, solely occupt current tile
	moving=false;
	global.occupied[oldTile[0]][oldTile[1]]=false
	oldTile[0]=tile[0]
	oldTile[1]=tile[1]
	occupied.remove_at(0)
	
func startMoving(): #start movement, start occupying destination tile so nothing else moves there
	global.occupied[tile[0]][tile[1]] = true
	addOccupied([tile[0],tile[1]])
	moving=true

func _process(delta):
	pass
	
func takeDamage(damage): #handle being damaged and dying
	hp-=damage
	if(hp<=0):
		global.enemyCount-=1

		for i in occupied:

			global.occupied[i[0]][i[1]] = false
		die()
	else:
		$hpLabel.text=str(hp)
		
		

	
#handle recieving status effects
func effect(ef): 
	if(ef== "ice"):
		speed=speed*.5
		var iced = ice.instantiate()
		add_child(iced)
		await get_tree().create_timer(iceTime,false).timeout
		iced.queue_free()

		
func isValidTile(x,y):
	return !global.occupied[x][y]&&!global.broken[x][y]
func isEmptyTile(x,y):
	return !global.occupied[x][y]
	
func _on_area_entered(area): #handle collision with another area

	if(area.is_in_group("player_attack")&&not area.is_queued_for_deletion()):
		if(area.hit(tile,oldTile)):
			takeDamage(area.damage)
		
		if(area.get_groups().has("ice")):
			effect("ice")
		if(area.is_in_group("one_time")): 
			area.queue_free()
			
func die():
	queue_free()

