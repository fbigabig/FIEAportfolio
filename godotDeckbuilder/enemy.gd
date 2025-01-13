extends "res://enemies/enemy_base.gd"


var rng = RandomNumberGenerator.new()
var bullet = preload("res://enemies/enemybullet.tscn")
var move = [0,0]
var num 


var shoot = false


@export var moveRange : Array
@export var shootRange : Array
# Called when the node enters the scene tree for the first time.
func _ready():
	rng.randomize()
	$moveTimer.set_wait_time(rng.randf_range(moveRange[0],moveRange[1]))
	$moveTimer.start()
	$shootTimer.set_wait_time(rng.randf_range(shootRange[0],shootRange[1]))
	$shootTimer.start()
	$hpLabel.text = str(hp)
	super()
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	
	#print($moveTimer.time_left," ",move, moving,pos)
	var step = speed*delta
	#print("basic",step)
	if moving: 
		if(move[0]!=0):
			step*=global.tileSize[0]

			position.x = move_toward(position.x,global.tilePos[tile[0]][tile[1]].x,step)
		else:
			step*=global.tileSize[1]

			position.y = move_toward(position.y,global.tilePos[tile[0]][tile[1]].y,step)
		
		if(position == global.tilePos[tile[0]][tile[1]]):

			doneMoving()

			$moveTimer.set_wait_time(rng.randf_range(moveRange[0],moveRange[1]))
			$moveTimer.start()
	elif shoot:
		var instance = bullet.instantiate()
		instance.position = position
		global.level.add_child(instance);
		$shootTimer.set_wait_time(rng.randf_range(shootRange[0],shootRange[1]))
		$shootTimer.start()
		shoot=false
func _on_move_timer_timeout():
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
	


func _on_shoot_timer_timeout():
	shoot = true
	

		#print(area.name)

