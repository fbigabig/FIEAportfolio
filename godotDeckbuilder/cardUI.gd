extends Node
var cardArray = []
var file = "res://spellstuff/cards.txt"
var cardNum
var icon
var canDraw = false
var handOut = false
var blankSpellCard = preload("res://spellstuff/blank_spell.png")
var greenBar = preload("res://assets/bar.tres")
var greyBar = preload("res://assets/greybar.tres")
var fillerCard = preload("res://spellstuff/dummy.png")
var chosenCardIndex= [0,0,0,0,0]

var choices = 5
#mechanic where you can stockpile cards (up to 5) then unleash them in order?
#shift = queue instead queue
#combo spells that double spells?
var shuffling = false
#TODO: actually implement dynamic hand #mostly done just need to actually do the between level stuff, deck is flexible and wraps around when used up
signal castCallBack(spellOut)
func playerCastAttempt():
	if(hand.size()>0):
		castCallBack.emit(hand[0])

		used()

@onready var spellCards = { #add size cost thing
	"Explosion": spell.new(
		"Explosion",
		load("res://spellstuff/purple_spell_card.png"),
		load("res://spellstuff/boomspawn.tscn"),
		"Create an explosion 4 tiles away",
		1
		),
	"Fireball": spell.new(
		"Fireball",
		load("res://spellstuff/red_spell_card.png"),
		load("res://spellstuff/fireball.tscn"),
		"Shoot out a fireball"
		,1
		),	
	"Spike": spell.new(
		"Spike",
		load("res://spellstuff/db_spell_card.png"),
		load("res://spellstuff/spikespell.tscn"),
		"Deploy a slow-moving spike"
		,1
		),
	"Iceball": spell.new(
		"Iceball",
		load("res://spellstuff/blue_spell_card.png"),
		load("res://spellstuff/iceball.tscn"),
		"Shoot out an iceball, slowing an enemy on hit"
		,2
		),
	"Doubler": spell.new(
		"Doubler",
		load("res://spellstuff/green_spell_card.png"),
		load("res://spellstuff/doubler.tscn"),
		"Acts as a copy of the next queued spell"
		,2
	),
	"Flameburst":spell.new(
		"Flameburst",
		load("res://spellstuff/orange_spell_card.png"),
		load("res://spellstuff/flameburst.tscn"),
		"Sends out flame forward 3 tiles",
		3
	),
	"Laser":spell.new(
		"Laser",
		load("res://spellstuff/laser_spell_card.png"),
		load("res://spellstuff/laser.tscn"),
		"Sends out flame forward 3 tiles",
		2
	),
	"Damocles":spell.new(
		"Damocles",
		load("res://spellstuff/damocles_spell_card.png"),
		load("res://spellstuff/damoclesSpawn.tscn"),
		"Summons a hanging sword to fall",
		1
	),
	"Backbomb":spell.new(
		"Backbomb",
		load("res://spellstuff/backbomb.png"),
		load("res://spellstuff/backbomb.tscn"),
		"Explode the back row",
		2
	),
	"Trihit":spell.new(
		"Trihit",
		load("res://spellstuff/trihit_spell_card.png"),
		load("res://spellstuff/trihit.tscn"),
		"Hit a triangle in front of you",
		1
	),
	"Backball":spell.new(
		"Backball",
		load("res://spellstuff/back_spell_card.png"),
		load("res://spellstuff/backball.tscn"),
		"Fire a fireball towards you. Can do self-damage.",
		1
	),
	"Bounce":spell.new(
		"Bounce",
		load("res://spellstuff/bounce_spell_card.png"),
		load("res://spellstuff/bounce.tscn"),
		"Bouncy attack",
		2
	),
	"EvilBounce":spell.new(
		"EvilBounce",
		load("res://spellstuff/evilbounce_spell_card.png"),
		load("res://spellstuff/evilbounce.tscn"),
		"Bouncy attack",
		1
	)
	#spell ideas:
	#instead of root maybe shock that (simple stun)
	#fire constant dps
	# water push but put out fire and doesn't work if ice
	#ice half damage if fire
	#wet status effect = half fire damage double shock
}

@onready var keys = spellCards.keys()
@export var deck = ["Fireball"] #default is 1 fireball
var hand = []
@onready var handIcons = [$Control2/VBoxContainer/card1,$Control2/VBoxContainer/card2,$Control2/VBoxContainer/card3,$Control2/VBoxContainer/card4,$Control2/VBoxContainer/card5]
var handSize = 5
@onready var updatedDeck = deck.duplicate()
var discard = []
var rng = RandomNumberGenerator.new()
@onready var buttons = [$Control/PanelContainer/HBoxContainer/VBoxContainer/card1,$Control/PanelContainer/HBoxContainer/VBoxContainer3/card2,$Control/PanelContainer/HBoxContainer/VBoxContainer4/card3,$Control/PanelContainer/HBoxContainer/VBoxContainer5/card4,$Control/PanelContainer/HBoxContainer/VBoxContainer2/card5]
@onready var costlabels = [$Control/PanelContainer/HBoxContainer/VBoxContainer/HBoxContainer/costlabel1,$Control/PanelContainer/HBoxContainer/VBoxContainer3/HBoxContainer2/costlabel2,$Control/PanelContainer/HBoxContainer/VBoxContainer4/HBoxContainer3/costlabel3,$Control/PanelContainer/HBoxContainer/VBoxContainer5/HBoxContainer4/costlabel4,$Control/PanelContainer/HBoxContainer/VBoxContainer2/HBoxContainer5/costlabel5]
# Called when the node enters the scene tree for the first time.
class spell:
	var card: Texture2D
	var name
	var slots
	var spellAttack: PackedScene
	var description: String
	var size
	func _init(n,t,s,d,z):
		name = n
		card = t
		spellAttack=s
		description=d
		size = z
	func isDummy():
		return false
class dummy:
	func isDummy():
		return true
func addCard(card):
	deck.append(card)
func _ready():


	$Control/timerBar.max_value=$cooldownTimer.wait_time

	#print(spellCards.size())
	global.cardUI=self
	
	rng.randomize()
	#var f = FileAccess.open(file, FileAccess.READ)
	#var tmp = f.get_csv_line(',')
	#for i in tmp:
		
	#	var tex : Texture2D = load(i)

	#	cardFiles.append(tex)
		
	cardNum = spellCards.size()
	for i in handSize:
		cardArray.append("0")

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if(shuffling):
		$Control/timerBar.value=$Control/timerBar.max_value-$shuffleTimer.time_left
	else:
		$Control/timerBar.value=$Control/timerBar.max_value-$cooldownTimer.time_left
		if Input.is_action_just_pressed("shuffle"):
			$cooldownTimer.stop()
			shuffleTimerSetup()
			shuffleDiscard()
			if(handOut):
				get_tree().paused = false
				$Control/PanelContainer.visible = false
				$Control/timerBar.visible=true
				handOut=false
				
	if(handOut):
		if Input.is_action_just_pressed("choice1"):
			buttonPress(1)
		elif Input.is_action_just_pressed("choice2"):
			buttonPress(2)
		elif Input.is_action_just_pressed("choice3"):
			buttonPress(3)
		elif Input.is_action_just_pressed("choice4"):
			buttonPress(4)
		elif Input.is_action_just_pressed("choice5"):
			buttonPress(5)

func buttonPress(choice):
	#print(choice)
	if(choice>choices):
		return
	get_tree().paused = false
	var tmpSpell = spellCards.get(cardArray[choice-1])
	addToHand(tmpSpell)
	#displayIMG(tmpSpell.card)

	updatedDeck.remove_at(chosenCardIndex[choice-1])

	#print("choices " + str(choices))
	$Control/PanelContainer.visible = false
	$Control/timerBar.visible=true
	handOut=false
	if(updatedDeck.size()>0):
		$cooldownTimer.start()
	else:
		#print("sizes "+str(discard.size())+" "+str(updatedDeck.size()))
		shuffleTimerSetup()

	#print("updated deck size " + str(updatedDeck.size()))
	
func dealHand():
	if(updatedDeck.size()==0):
		if(discard.size()>0):

			shuffleDiscard()
		else:
			return
	canDraw=false
	#hideIMG()
	handOut = true
	$Control/timerBar.visible=false
	get_tree().paused = true
	var usedNums = []
	if(updatedDeck.size()<5): 
		choices=updatedDeck.size()
		for i in range(choices,5):
			buttons[i].set_texture(blankSpellCard)
			costlabels[i].text = ""
	else:
		choices=5
	for i in range(choices):
		var num = rng.randi_range(0,updatedDeck.size()-1)
		while(usedNums.has(num)):
			num = rng.randi_range(0,updatedDeck.size()-1)
		usedNums.append(num)
		cardArray[i]=(updatedDeck[num])
		chosenCardIndex[i]=num
		var curCard = spellCards.get(cardArray[i])
		#print(str(choices) + "  help   "+ str(i))
		#print(str(updatedDeck))

		buttons[i].set_texture(curCard.card)

		costlabels[i].text = str(curCard.size)
	$Control/PanelContainer.visible = true

	
func shuffleTimerSetup():
	$shuffleTimer.wait_time = deck.size()/4
	$Control/timerBar.max_value=$shuffleTimer.wait_time
	$Control/timerBar.add_theme_stylebox_override("fill",greyBar)
	$shuffleTimer.start()
	shuffling = true

func _on_card_1_pressed():
	$Control/PanelContainer/HBoxContainer/card1.button_pressed=false
	buttonPress(1)


func _on_card_2_pressed():
	$Control/PanelContainer/HBoxContainer/card2.button_pressed=false
	buttonPress(2)


func _on_card_3_pressed():
	$Control/PanelContainer/HBoxContainer/card3.button_pressed=false
	buttonPress(3)

func _on_card_4_pressed():
	$Control/PanelContainer/HBoxContainer/card4.button_pressed=false
	buttonPress(4)

func _on_card_5_pressed():
	$Control/PanelContainer/HBoxContainer/card5.button_pressed=false
	buttonPress(5)

"""
func displayIMG(img):
	global.icon.texture = img
	global.icon.visible = true
	"""
func hideIMG():
	global.icon.visible = false 


func _on_cooldown_timer_timeout():

	canDraw = true # Replace with function body.
func used():
	for i in range(0,handSize-hand[0].size,1):
		handIcons[i].set_texture(handIcons[i+hand[0].size].get_texture())
	for i in range(hand[0].size):

		handIcons[hand.size()-i-1].set_texture(blankSpellCard)

	discard.push_back(hand[0].name)
	hand.remove_at(0)
	if(hand.size()!=0):
		while(hand[0].isDummy()):
			hand.remove_at(0)
			if(hand.size()==0):
				break
		
func shuffleDiscard():



	if(discard.size()>0):
		updatedDeck+=discard
	discard.clear()
func addToHand(sp):
	if (Input.is_action_pressed("cast")):
			
		hand.append(sp)

		for i in range(1,sp.size):
			hand.append(dummy.new())

		while(hand.size()>handSize):
			discard.push_back(hand[0].name)
			hand.pop_front()
			if(hand[0].isDummy()):
				while(true):
					if(hand[0].isDummy()):
						hand.pop_front()
					else:
						break
				
				
			
	else:
		
		for i in range(1,sp.size):
			hand.push_front(dummy.new())

		hand.push_front(sp)
		
		while(hand.size()>handSize):
			if(hand[-1].isDummy()):
				while(true):
					if(hand[-1].isDummy()):
						hand.pop_back()
					else:
						discard.push_back(hand[-1].name)
						hand.pop_back()
						break
			else:
				discard.push_back(hand[-1].name)
				hand.pop_back()
	for i in range(hand.size()):
		if(hand[i].isDummy()):
			handIcons[i].set_texture(fillerCard)
		else:
			handIcons[i].set_texture(hand[i].card)
	for i in range(hand.size(),handSize):
		handIcons[i].set_texture(blankSpellCard)


func _on_shuffle_timer_timeout():
	shuffling = false
	$Control/timerBar.max_value=$cooldownTimer.wait_time
	$Control/timerBar.add_theme_stylebox_override("fill",greenBar)
	$cooldownTimer.start() # Replace with function body.
