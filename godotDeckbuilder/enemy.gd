#basic enemy that moves and shoots, inherits from the base enemy class
extends "res://enemies/enemy_base.gd"


var rng = RandomNumberGenerator.new()
var bullet = preload("res://enemies/enemybullet.tscn")
var move = [0,0]
var num 


var shoot = false

#ranges to randomly select a cooldown value from
@export var moveRange : Array 
@export var shootRange : Array

func _ready():
	rng.randomize()
	#start timers set to random numbers in their respective ranges
	$moveTimer.set_wait_time(rng.randf_range(moveRange[0],moveRange[1]))
	$moveTimer.start()
	$shootTimer.set_wait_time(rng.randf_range(shootRange[0],shootRange[1]))
	$shootTimer.start()
	$hpLabel.text = str(hp)
	super() #access base class ready function




func _process(delta):
	

	var step = speed*delta

	if moving: 
		if(move[0]!=0):
			step*=global.tileSize[0] #adjust step so that it takes the same time to move between tiles in the x direction and the y direction 

			position.x = move_toward(position.x,global.tilePos[tile[0]][tile[1]].x,step) #move towards destination tile
		else:
			step*=global.tileSize[1]

			position.y = move_toward(position.y,global.tilePos[tile[0]][tile[1]].y,step)
		
		if(position == global.tilePos[tile[0]][tile[1]]): #goal tile reached, wait before moving again

			doneMoving()

			$moveTimer.set_wait_time(rng.randf_range(moveRange[0],moveRange[1]))
			$moveTimer.start()
	elif shoot: #if not moving and the shoot timer is finished, shoot
		var instance = bullet.instantiate()
		instance.position = position
		global.level.add_child(instance); #add the bullet to the level so it doesn't get its transform parented to this enemy
		$shootTimer.set_wait_time(rng.randf_range(shootRange[0],shootRange[1]))
		$shootTimer.start()
		shoot=false
func _on_move_timer_timeout(): #determine which possible moves are valid, choose one, and start moving
	var validMoves = []
	if(tile[0]>global.xS&&isValidTile(tile[0]-1,tile[1])):
		validMoves.append([-1,0])
	if(tile[0]<global.xS*2-1&&isValidTile(tile[0]+1,tile[1])):
		validMoves.append([1,0])
	if(tile[1]>0&&isValidTile(tile[0],tile[1]-1)):
		validMoves.append([0,-1])
	if(tile[1]<global.yS-1&&isValidTile(tile[0],tile[1]+1)):
		validMoves.append([0,1])
	if(validMoves.size()==0):
		$moveTimer.set_wait_time(rng.randf_range(moveRange[0],moveRange[1]))
		$moveTimer.start()
	else:

		move = validMoves[rng.randi_range(1, validMoves.size())-1]
		tile[0]+=move[0]
		tile[1]+=move[1]
		startMoving()
	


func _on_shoot_timer_timeout(): #shoot when able
	shoot = true


