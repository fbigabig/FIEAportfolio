extends Area2D
@export var speed : int

var curTile = Vector2(1,1)
var oldTile = Vector2(1,1)
var goalTile = Vector2(1,1)
#@onready var global = get_node("/root/global")
var bulletScene = preload("res://player/basicbullet.tscn")
var canShoot = true;
var wantsToShoot = false
var invincible = false
#movement vars
var canMove = true
var xyIndex = 0
var buffMove=' '
var playerSprites = [preload("res://player/circlewhite.png"), preload("res://player/circlegreen.png")]
var moves = 0
var cast = false
var wantToCast = false


signal wannaCast
# Called when the node enters the scene tree for the first time.
func _ready():
	global.player = self
	$reticle.position.x=$reticle.position.x+(global.tileSize[0]*1.5+global.buffer*1.5)*2
	$reticle.visible = true
	# Replace with function body.

	global.cardUI.castCallBack.connect(doCast)
	wannaCast.connect(global.cardUI.playerCastAttempt)
func doCast(sp):
	var thisSpell = sp.spellAttack.instantiate()
	thisSpell.position = global.tilePos[curTile[0]][curTile[1]]
	thisSpell.tile = [curTile[0],curTile[1]]
	global.level.add_child(thisSpell)
	cast = false
	wantToCast = false
	#global.cardUI.hideIMG()
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	#target
	
	#print($reticle.position)
	if(cast):
		if(Input.is_action_just_pressed("cast")):
			wantToCast=true

	if(canShoot): #implement on enemy later
		
		if(Input.is_action_just_pressed("shoot")):
			wantsToShoot = true	
	#movement stuff
	if canMove:
		if(wantToCast||(Input.is_action_just_pressed("cast"))): #"""&&cast""")):
			"""var thisSpell = spell.instantiate()
			thisSpell.position = global.tilePos[curTile[0]][curTile[1]]
			thisSpell.tile = [curTile[0],curTile[1]]
			global.root.add_child(thisSpell)
			cast = false
			wantToCast = false
			global.cardUI.hideIMG()
			"""
			wannaCast.emit()
		if(wantsToShoot||(Input.is_action_just_pressed("shoot")&&canShoot)): #make this happen when move done
			var bullet = bulletScene.instantiate()
			
			bullet.position = global.tilePos[curTile[0]][curTile[1]]
			global.level.add_child(bullet)
			$shootTimer.start()
			$sprite.texture = playerSprites[0] #switch to exhausted sprite
			canShoot=false
			wantsToShoot = false
		if buffMove!=' ':
			if buffMove=='l':
				move(0,-1,0)
			elif buffMove=='r':
				move(0,1,0)
			elif buffMove=='u':
				move(1,0,-1)
			elif buffMove=='d':
				move(1,0,1)
			
		elif Input.is_action_just_pressed("moveleft")&&curTile.x>0:
			move(0,-1,0)
		elif Input.is_action_just_pressed("moveright")&&curTile.x<global.xS-1:
			move(0,1,0)
		elif Input.is_action_just_pressed("moveup")&&curTile.y>0:
			move(1,0,-1)
		elif Input.is_action_just_pressed("movedown")&&curTile.y<global.yS-1:
			move(1,0,1)

	else:
		var step = speed * global.tileSize[xyIndex]*delta
		if(xyIndex==0):
			position.x = move_toward(position.x,global.tilePos[goalTile[0]][goalTile[1]].x,step)

		else:
			position.y = move_toward(position.y,global.tilePos[goalTile[0]][goalTile[1]].y,step)



		if(position == global.tilePos[goalTile[0]][goalTile[1]]):
			canMove=true;
			global.occupied[oldTile[0]][oldTile[1]]=false
		
		#buffer move system
		if Input.is_action_just_pressed("moveleft")&&goalTile.x>0:
			buffMove='l'
		elif Input.is_action_just_pressed("moveright")&&goalTile.x<global.xS-1:
			buffMove='r'
		elif Input.is_action_just_pressed("moveup")&&goalTile.y>0:
			buffMove='u'
		elif Input.is_action_just_pressed("movedown")&&goalTile.y<global.yS-1:
			buffMove='d'
			
			
		

		

func move(xyIdx, vectoridx1, vectoridx2):

	xyIndex=xyIdx
	canMove=false
	goalTile = curTile + Vector2(vectoridx1,vectoridx2)
	oldTile=curTile
	buffMove=' '
	curTile=goalTile
	global.occupied[curTile[0]][curTile[1]]=true
	global.playerTile=curTile
	moves+=1




func _on_shoot_timer_timeout():
	canShoot=true
	$sprite.texture = playerSprites[1]
	#pass # Replace with function body.
func hit(damage):
	if(invincible):
		return
	global.hp-=damage
	$sprite.modulate.a = .5
	$iframes.start()
	invincible=true
	if(global.hp<=0):
		get_tree().change_scene_to_file("res://world/menu.tscn")



func _on_area_entered(area):
	if(area.is_in_group("enemy_bullet")): 
		if(area.hitPlayer(goalTile,oldTile)):
			hit(area.damage)
		


func _on_iframes_timeout():
	invincible=false
	$sprite.modulate.a = 1 
