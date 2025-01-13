extends Area2D
@export var speed = 5
@export var hp = 250
var iceTime = 3
var tile
var oldTile = [0,0]
var occupied = []
var moving = false
var dist = 0

@onready var ice = preload("res://spellstuff/ice_indicator.tscn")
# Called when the node enters the scene tree for the first time.
func _ready():
	occupied.append(tile)
	oldTile[0]=tile[0]
	oldTile[1]=tile[1]

func addOccupied(newTile):
	#print("wtf " + str(newTile))
	occupied.append(newTile)
func doneMoving():
	moving=false;
	
	global.occupied[oldTile[0]][oldTile[1]]=false
	oldTile[0]=tile[0]
	oldTile[1]=tile[1]
	#print(str(occupied)+" wackiness" + str(self.name))
	occupied.remove_at(0)
func startMoving():
	global.occupied[tile[0]][tile[1]] = true
	addOccupied([tile[0],tile[1]])
	moving=true
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
func takeDamage(damage):
	hp-=damage
	if(hp<=0):
		global.enemyCount-=1

		for i in occupied:

			global.occupied[i[0]][i[1]] = false
		die()
	else:
		$hpLabel.text=str(hp)
		
		

	
#status effects
func effect(ef): 

	if(ef== "ice"):
		speed=speed*.5
		#print(speed)
		var iced = ice.instantiate()
		add_child(iced)
		await get_tree().create_timer(iceTime,false).timeout
		iced.queue_free()
		speed=speed*2
		#print(speed)
		#queue_free()
		
func isValidTile(x,y):
	return !global.occupied[x][y]&&!global.broken[x][y]
func isEmptyTile(x,y):
	return !global.occupied[x][y]
func _on_area_entered(area):
	#print(area.name)
	if(area.is_in_group("player_attack")&&not area.is_queued_for_deletion()):
		if(area.hit(tile,oldTile)):
			takeDamage(area.damage)
		
		if(area.get_groups().has("ice")):
			effect("ice")
		if(area.is_in_group("one_time")): #change this to a method akin to how hit player works in order to do stuff like bouncing attacks work
			#area.get_node("CollisionShape2D").disabled = true
			area.queue_free()
			
func die():
	queue_free()

