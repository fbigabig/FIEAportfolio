extends CharacterBody2D

signal landed
const SPEED = 150.0
const JUMP_VELOCITY = -190.5 #oldest is 200, old is 190.5
const ACCEL = 10 #old:10
#const ACCELBOOST = 2
const OVERDECEL = 5 #old = 3
const AIRCONTROL= 2
const SUPERBOUNCE = 1.325
var hitBouncy = false
const CONVEYORFORCE = {
	"black": 150,
	"white": 15
}
const CONVEYOR_WHITEINITBOOSTFACTOR=10
const CONVEYORMAXFORCE = 400 #old: 350
const CONVEYORGRAVFACT = .5
#const CONVEYORSTICKYFORCE = 20
enum BounceState {
	None,
	Bouncing,
	Landing
}
@export var linelength=20
var justCol=false
var UItemplate = preload("res://prefabs/UI/pause_UI.tscn")
var UI
var oldOnConveyor=false
var bounce:BounceState = BounceState.None
var didBounce = false
var flip =false
var dashFact = 2
var bounceFact = 1.00
var oldV = Vector2.ZERO
var aim_dir
var canBounce = true
var beingShot = false
var cannon 
var target = Vector2.ZERO
var oldFloor = true
var deathSign = preload("res://prefabs/deathSign.tscn")
var oneTickGroundDelay =false
var onice=false
var onslope=0
var coyotecounter = 0
var grounded = true
var oldPos
#var gothru=false
var onconveyor = false
const ICECONTROL = 4
var direction=1
var velOffset
var buffBounce=false
var buffForce=Vector2.ZERO
var timerStarted = false
var fieldFresh = Color8(0,0,255,85)
var fieldUsed = Color8(125,0,255,85)
var paused=false
@onready var line = $aimline
@onready var cursor = $cursor
@onready var field = $bounceField
@onready var player = $player
var jumpNoise= preload("res://assets/audio/jump (1).wav")
var bounceNoise= preload("res://assets/audio/bounce.wav")
var bounceNoise1= preload("res://assets/audio/bounce1.wav")
var bounceNoiseDone= preload("res://assets/audio/bouncedone.wav")
var superbounceNoise = preload("res://assets/audio/superjump.wav")
var timer
@export var interactBox = false

var gravity = 12.5
var jumpTicks = 0
const jumpTicksNum=5
@onready var wheel = $wheel
@onready var interacter = $interacter
@export var tileMap: TileMap
var useMouse=false
var hitConveyor = {
	"white": false,
	"black": false
}
func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)
	global.player=self
	global.time=0
	field.hide()

	timer = get_tree().get_first_node_in_group("timer")

	UI = UItemplate.instantiate() 
	get_parent().add_child.call_deferred(UI)
func _input(event):
	if (event is InputEventKey or event is InputEventMouse) and !useMouse:
		Input.set_mouse_mode(Input.MOUSE_MODE_CONFINED)

		useMouse = true
	if (event is InputEventJoypadButton or event is InputEventJoypadMotion) and useMouse:
		Input.set_mouse_mode(Input.MOUSE_MODE_HIDDEN)
		await get_tree().create_timer(.05).timeout

		useMouse = false
	if(not timerStarted):
		if(not event is InputEventMouseMotion and (event.is_action("moveLeft") or event.is_action("moveRight")  or event.is_action("jump") )) or Input.get_axis("moveLeft","moveRight")!=0:
			timerStarted=true
	# Mouse in viewport coordinates.
func _process(delta):

	if(timer!=null and timerStarted):
		global.time+=delta
		timer.text="[center]"+str(global.time).pad_decimals(2)+"[/center]"
	if(direction!=0):
		wheel.play()
		
		wheel.flip_h = direction<0
	else:
		wheel.pause()
	if(interactBox && Input.is_action_just_pressed("confirm")):
		for i in interacter.get_overlapping_areas():
			if(i.is_in_group("selector")):
				i.doLoad()
	elif(Input.is_action_just_pressed("restart")):
		die()
	elif Input.is_action_just_pressed("shoot")&&  canBounce&&(aim_dir!=Vector2.ZERO):

		if(useMouse):
			#velocity = Vector2.ZERO
			#print("Mouse Click at: ", event.position)
			
			aim_dir= (get_global_mouse_position()-global_position).normalized()


		else:
			aim_dir = Input.get_vector("aimLeft", "aimRight", "aimUp", "aimDown").normalized()
		
		if(beingShot):
			buffBounce=true
			field.modulate=fieldFresh
			field.show()
			buffForce = aim_dir*SPEED*dashFact
			return
		startBounce()
		
		velocity = aim_dir*SPEED*dashFact
	elif(Input.is_action_just_pressed("jump")):
		jumpTicks=jumpTicksNum
	
func startBounce():
	emit_signal("landed") #used to reset cannons
	onconveyor=false
	onice=false
	oldOnConveyor=false
	bounce=BounceState.Bouncing
	field.modulate=fieldFresh
	field.show()
	if(player.playing):
		player.stop()
	if(player.stream!=bounceNoise1):
		player.stream=bounceNoise1
	player.play()
	canBounce=false
func transgenderBounce(collision):
	field.modulate=fieldUsed
	bounce=BounceState.Landing
	velocity = velocity.bounce(collision.get_normal()) * bounceFact * (SUPERBOUNCE if hitBouncy else 1)
	if(player.playing):
		player.stop()
	if(hitBouncy):
		if(player.stream!=superbounceNoise):
			player.stream=superbounceNoise
	else:
		if(player.stream!=bounceNoise):
			player.stream=bounceNoise
	player.play()
	#print(hitBouncy)
	#if(hitBouncy):
		#velocity=Vector2(pow(abs(velocity.x),1.1)*(1 if velocity.x>0 else -1),pow(abs(velocity.y),1.1)*(1 if velocity.y>0 else -1))
		#print(velocity)

func endBounce():
	bounce=BounceState.None
	field.hide()
	if(player.playing):
		player.stop()
	if(player.stream!=bounceNoiseDone):
		player.stream=bounceNoiseDone
	player.play()



func _physics_process(delta):

	justCol=false
	if(beingShot):
		#var didThing=false
		#
		#var a = (abs(position.y-target.y))
		#var b = (abs(position.x-target.x))
		#if b>16-7:
#
			#print("aaaaa")
			#position.x=move_toward(position.x,target.x, velocity.length()*delta)
			#didThing=true
		#if a>16-7:
			#print("bb")
			#position.y=move_toward(position.y,target.y, velocity.length()*delta)
			#didThing=true
		#if(!didThing):
			##if(beingShot==1):
				##print("A")
				##position.y=move_toward(position.y,target.y, velocity.length()*delta)
				##if(position.y==target.y):
					##didThing=true
			##elif(beingShot==2):
				##print("b")
				##position.x=move_toward(position.x,target.x, velocity.length()*delta)
				##if(position.x==target.x):
					##didThing=true
			##if(didThing):
			#beingShot=0
			#velocity=oldV
		position.x=move_toward(position.x,target.x, oldV.length()*delta) #if velocity.length()*delta > 1 else 1)
		position.y=move_toward(position.y,target.y,  oldV.length()*delta) #if velocity.length()*delta > 1 else 1)
		if(position==target):
			beingShot=false
			velocity=oldV*1.5
			cannon.doneShooting()
			if(buffBounce):
				buffBounce=false
				startBounce()
				velocity=buffForce

		return
	for key in hitConveyor:
		hitConveyor[key]=false

	oldFloor=grounded
	if(useMouse):
		aim_dir= (get_global_mouse_position()-global_position).normalized()
	else:
		aim_dir = Input.get_vector("aimLeft", "aimRight", "aimUp", "aimDown").normalized()

	if(aim_dir.length()==0):
		#cursor.hide()
		line.hide()
	else:
		#cursor.show()
		line.show()
	#cursor.rotation = Transform2D.IDENTITY.looking_at(aim_dir).get_rotation()
	line.set_point_position(1,aim_dir*linelength)
	var tmp = gravity *(CONVEYORGRAVFACT if oldOnConveyor else 1)

	velocity.y += tmp
	
	# Handle jump.
	if (jumpTicks>0) and (grounded||coyotecounter>0) and bounce==BounceState.None:
		
		jumpTicks=0
		coyotecounter=0
		velocity.y = JUMP_VELOCITY
		if(player.playing):
			player.stop()
		if(player.stream!=jumpNoise):
			player.stream=jumpNoise
		player.play()

	# Get the input direction and handle the movement/deceleration.
	# As good practice, you should replace UI actions with custom gameplay actions.
	direction = Input.get_axis("moveLeft", "moveRight")

	
	velOffset = velocity.normalized()
	oldV=velocity

	match bounce:
		#if direction && (abs(velocity.x)<SPEED||velocity.x*direction<0):
			#velocity.x += direction * SPEED * delta * AIRCONTROL
		BounceState.None:
			if direction && (abs(velocity.x)<SPEED||velocity.x*direction<0):

				#var acc
				#if(velocity.x*direction<0 and abs(velocity.x)<SPEED and not onconveyor): #boost acceleration if turning and not speeding (to keep exceel speed a thing you have to balance)
					#acc=ACCEL*ACCELBOOST
				#else:
					#acc=ACCEL
				velocity.x = move_toward(velocity.x,SPEED*direction,((ACCEL if grounded else ACCEL/AIRCONTROL) if !onice else ACCEL/ICECONTROL)+(OVERDECEL if abs(velocity.x)>SPEED and velocity.x*direction<0 else 0))

				if(velocity.x*direction>0):

					velocity.x = min(SPEED,abs(velocity.x))*(1 if velocity.x>0 else -1)

			elif(!direction and !onconveyor):

				velocity.x = move_toward(velocity.x, 0, (ACCEL if !onice else ACCEL/(2*ICECONTROL)))#*ACCELBOOST

				if(onice and onslope*velocity.x<0 and abs(velocity.x)<40):
					velocity=Vector2.ZERO
					#fixes weird glitch involving going backwards on upslopes at slow speeds


			elif(is_on_floor()&&abs(velocity.x)>SPEED and !onconveyor and !onice):

				velocity.x = move_toward(velocity.x, SPEED, OVERDECEL) #don't steal speed too quick

			

			move_and_slide()
			oldOnConveyor=onconveyor
			onconveyor=false
			onslope=0
			onice=false
			#if(tileMap): floor_snap_length=0

			
			if(get_slide_collision_count()>0):
				for i in get_slide_collision_count():
					justCol=true
					var col = get_slide_collision(i)
					
					if(!tileMap):
						hitCheck(col.get_collider())

					else:

						handleTile(col) #col.get_position()-off
						#print(get_slide_collision_count())
						#print(colpos)
						#var off = (global_position-col.get_position()).normalized()+Vector2(.001,.001 ) ##offset fixes off by one error when exactly lined up



		BounceState.Bouncing:
			var tmpPos=position
			var collision = move_and_collide(velocity * delta)
			var didThing= false
			if(collision):

				if(collision.get_collider() is Node): #enable to clip through corners of one way plats
					var cold = collision.get_collider()

					#print(cold.get_groups())
					if(cold.is_in_group("gothruplat")):
						var norm = collision.get_normal()

						norm = snapped(norm,Vector2(.1,.1))


						if(norm.x!=0 and norm.y!=0 and !grounded):

							didThing=true
							position+= oldV*delta
						#print("a")
						#if((global_position.x+2.5<cold.global_position.x-cold.w or global_position.x-2.5>cold.global_position.x+cold.w)and not grounded):
							#print("zone")
							# 
							#position+= velocity*delta
				
				if(!didThing):
					justCol=true
					if(!tileMap):
						hitCheck(collision.get_collider())
					else:
						#var off = (global_position-collision.get_position()).normalized()
						handleTile(collision)

							
					#print(gothru)
					#if(gothru):
						#position=tmpPos
						#oldV.y=0
						#position += oldV*delta
						#velocity=oldV
					#else:
					transgenderBounce(collision)
		BounceState.Landing:
			move_and_slide()
			oldOnConveyor=onconveyor
			onconveyor=false
			if(get_slide_collision_count()>0):
				justCol=true
				for i in get_slide_collision_count():
					var col = get_slide_collision(i)
					if(!tileMap):
						hitCheck(col.get_collider())
					else:
						#var off = (global_position-col.get_position()).normalized()

						handleTile(col) #col.get_position()-off

				endBounce()


		#var collision = move_and_collide(velocity * delta)
		#if(collision):
			#if(bounce == 2):
				#endBounce()
				#
			#elif(bounce==1):
				#bounce+=1
				#velocity = velocity.bounce(collision.get_normal()) * bounceFact
			#hitCheck(collision.get_collider())




	
			
		
	hitBouncy=false
	if(is_on_floor()&&bounce == BounceState.None):
		if(oneTickGroundDelay):
			canBounce=true
			grounded=true
		oneTickGroundDelay=true #must be on ground two frames to get stuff back
	else:
		oneTickGroundDelay=false
		grounded=false
	if(grounded and not oldFloor):
		emit_signal("landed")
	if(coyotecounter>0):
		coyotecounter-=1
	if(!is_on_floor() and oldFloor):
		coyotecounter=5 #start coyote timer
		
	if(jumpTicks>0):
		jumpTicks-=1

func hitCheck(obj):
	if obj is Node:
		if obj.is_in_group("hazard"):
			die()
func die():

	var dM = deathSign.instantiate()
	dM.position=position
	get_parent().add_child(dM)
	queue_free()




func eraseAll(pos,typeGoal,usedDict):
	var dat = tileMap.get_cell_tile_data(0,pos)
	var type
	if(dat):
		var thing = (dat.get_custom_data("special"))
		if(thing != ""):
			type = thing
		else:
			return
	else:
		return
	if(type==typeGoal):
		tileMap.erase_cell(0,pos)
		for i in range(-1,2):

			for j in range(-1,2):
				var newPos = pos+Vector2i(i,j)
				if(i==0 and j==0) or usedDict.has(newPos):
					continue
				eraseAll(newPos,typeGoal,usedDict)
func handleTile(col):
	if(col.get_collider()!=tileMap):
		hitCheck(col.get_collider())
		return
	if(tileMap):
		var tilePos = tileMap.get_coords_for_body_rid(col.get_collider_rid())
		var type
		#var pos = tileMap.to_local(tilePos)
	#	pos= tileMap.local_to_map(pos)
		#
		var dat = tileMap.get_cell_tile_data(0,tilePos)

		if(dat):
			var thing = (dat.get_custom_data("special"))
			if(thing != ""):
				type = thing
			else:

				return
		else:

			return
		#KNOWN BUG: player will snap to hazards if it can walk along a diagonal hazard, do not put diagonal hazard tiles next to non-hazardous tiles, separate them by at least one flat hazard tile
		#floor snap stuff "fixed" it but broke slopes so it can't exist.
		#if(type=="normal"||type=="ice"||type=="bouncy"):
			#floor_snap_length=5 #prevents snapping to hazards
			#var norm = col.get_normal()
			#norm.x=snapped(norm.x,.1)
			#norm.y=snapped(norm.y,.1)
			#if(norm.x!=0 and norm.y!=0):
				#onslope=1 if norm.x>0 else -1

		match type:
			"flip":
				if(bounce==BounceState.Bouncing):
					#var tmpv=velocity.x
					#velocity.x=abs(velocity.y)*(1 if velocity.x>0 else -1)
					#velocity.y=abs(tmpv)*(1 if velocity.y>0 else -1)

					#var norm = snapped(col.get_normal(),Vector2(.1,.1))
					#if(norm.x!=0):
						#velocity.y*=-1
					#else:
						#velocity.x*=-1
						
					if(abs(velocity.y)>abs(velocity.x)):
						velocity.x=0
					else:
						velocity.y=0
					onice=true
			"normal":
				pass
			#"gothru":
				#
				#gothru=true
			"bouncy":
				hitBouncy=true
			"ice":
				onice=true
			
			"conveyor_white": #white conveyors bring you up to a speed. bouncing off them does little as a result

				if(hitConveyor["white"]): return
				hitConveyor["white"]=true
				#todo: make speedy conveyor
				var force = (CONVEYORFORCE["white"]*col.get_normal()).rotated(PI/2) * (1 if oldOnConveyor else CONVEYOR_WHITEINITBOOSTFACTOR)

				onconveyor=true
				force.x=snapped(force.x,.1)
				force.y=snapped(force.y,.1)
				var normal=col.get_normal()
				var flipped = (dat.get_custom_data("value"))
			
				if(flipped==1): force*=-1

				if(abs(force.x)>.2):
					#if(force.x>0):
						#force.y=min(CONVEYORSTICKYFORCE,(force.y if force.y>0 else CONVEYORSTICKYFORCE))
					#else:
						#force.y=max(-CONVEYORSTICKYFORCE,(force.y if force.y<0 else -CONVEYORSTICKYFORCE))

					if abs(velocity.x)<CONVEYORMAXFORCE or force.x*velocity.x<0:
						velocity.x=min(abs(velocity.x+force.x),CONVEYORMAXFORCE)*(1 if velocity.x+force.x>0 else -1)

					#else:
						#print("toofast1")
					#if(velocity.x*force.x<0):velocity.x=force.x
				else:

					

					if (abs(velocity.y)<CONVEYORMAXFORCE or force.y*velocity.y<0) and (!direction or Vector2(direction,0).angle_to(normal)!=0):
						velocity.y=min(abs(velocity.y+force.y),CONVEYORMAXFORCE)*(1 if velocity.y+force.y>0 else -1)
					#if(velocity.y*force.y<0):velocity.y=force.y
					#else:
						#print("toofast2")
			"conveyor_black": #black conveyors are like dash pads- instant speed. bouncing off them will severely affect ur trajectory.
				onconveyor=true
				if(hitConveyor["black"]): return
				hitConveyor["black"]=true
				var normal=col.get_normal()
				#todo: make speedy conveyor
				var force = (CONVEYORFORCE["black"]*normal).rotated(PI/2)
				force.x=snapped(force.x,1)
				force.y=snapped(force.y,1)
				var flipped = (dat.get_custom_data("value"))

				if(flipped==1): force*=-1
				if(force.x!=0):
					if abs(velocity.x)<CONVEYORFORCE["black"] or force.x*velocity.x<0:
						velocity.x=force.x
					#else:
						#print("toofast1")
				else:

					if (abs(velocity.y)<CONVEYORFORCE["black"] or force.y*velocity.y<0) and (!direction or Vector2(direction,0).angle_to(normal)!=0):
						velocity.y=force.y

					#else:
						#print("toofast2")
						
						
		#check that stops you from grazing hazardous tiles:
		var diff = (global_position-col.get_position())

		if not ((diff*velOffset).length()<0.1 and abs(snapped(col.get_normal(),Vector2(1,1)))==Vector2(1,0)): #ignore jumping alongside walls	

			match type:
				"bounceonly":
					if(bounce==BounceState.Bouncing):
						return
					else:
						die()
				"hazard":
					die()
				
				"crumble":
					#await get_tree().create_timer(.1).timeout
					var used={}
					eraseAll(tilePos,"crumble",used)
					Musicplayer.playBub()
				"crumblebounce":
					var used={}
					eraseAll(tilePos,"crumblebounce",used)
					hitBouncy=true
					Musicplayer.playBub()
			#removed: grav flip stuff, too jank and not enough room to utilize
			#"up":
				#if(gravity == gravities[type]): return
				#gravity = gravities["up"] #totally broken, need to adjust sense of floor? and xy movement to match, ugh
				#up_direction=-gravities["up"].normalized() #this causes stutters idk why
			#"down":
				#if(gravity == gravities[type]): return
				#gravity = gravities["down"]
				#up_direction=-gravities["down"].normalized()
			#"right":
				#if(gravity == gravities[type]): return
				#gravity = gravities["right"]
				#up_direction=-gravities["right"].normalized()
			#"left":
				#if(gravity == gravities[type]): return
				#gravity = gravities["left"]
				#up_direction=-gravities["left"].normalized()
				##add 4 things for each dir of grav flip tiles


func momentumCannonEntered(dir, center,cn):
	target = center
	oldPos=position
	if(justCol):
		velocity=oldV
	oldV=dir*(velocity.length() if velocity.length()>SPEED else SPEED)
	velocity=Vector2.ZERO
	beingShot=true
	cannon=cn


