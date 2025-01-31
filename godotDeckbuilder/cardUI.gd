#this script manages the card UI and the deck itself
extends Node
var cardArray = []
var file = "res://spellstuff/cards.txt"
var cardNum
var icon
var canDraw = false
var cardChoicesOut = false
var blankSpellCard = preload("res://spellstuff/blank_spell.png")
var greenBar = preload("res://assets/bar.tres")
var greyBar = preload("res://assets/greybar.tres")
var fillerCard = preload("res://spellstuff/dummy.png")
var chosenCardIndex= [0,0,0,0,0]

const CHOICESGOAL = 5
var choices = CHOICESGOAL #how many spells there are to pick from
var shuffling = false

signal castCallBack(spellOut)
func playerCastAttempt(): #handle the player attempting to cast, give them a card if there is one in the hand
	if(hand.size()>0):
		castCallBack.emit(hand[0])
		used()

@onready var spellCards = {  #dictionary of all the possible spells in the deck
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
}

@onready var keys = spellCards.keys()
@export var deck = ["Fireball"] #the deck of cards, default is 1 fireball
var hand = []

var handSize = 5
@onready var updatedDeck = deck.duplicate() #working copy of the base deck that actually gets modified, the original deck value is used as a base to make this
var discard = []
var rng = RandomNumberGenerator.new()

#ui element references
@onready var handIcons = [$Control2/VBoxContainer/card1,$Control2/VBoxContainer/card2,$Control2/VBoxContainer/card3,$Control2/VBoxContainer/card4,$Control2/VBoxContainer/card5]
@onready var buttons = [$Control/PanelContainer/HBoxContainer/VBoxContainer/card1,$Control/PanelContainer/HBoxContainer/VBoxContainer3/card2,$Control/PanelContainer/HBoxContainer/VBoxContainer4/card3,$Control/PanelContainer/HBoxContainer/VBoxContainer5/card4,$Control/PanelContainer/HBoxContainer/VBoxContainer2/card5]
@onready var costlabels = [$Control/PanelContainer/HBoxContainer/VBoxContainer/HBoxContainer/costlabel1,$Control/PanelContainer/HBoxContainer/VBoxContainer3/HBoxContainer2/costlabel2,$Control/PanelContainer/HBoxContainer/VBoxContainer4/HBoxContainer3/costlabel3,$Control/PanelContainer/HBoxContainer/VBoxContainer5/HBoxContainer4/costlabel4,$Control/PanelContainer/HBoxContainer/VBoxContainer2/HBoxContainer5/costlabel5]

class spell: #defines the custom spell class that stores the actual attack object, the description of the spell, and the image associated
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
		
class dummy: #class for dummy spells that just take up slots, as some spells add dummy spells to your hard as their cost
	func isDummy():
		return true
		
func addCard(card):
	deck.append(card)
	
func _ready():
	$Control/timerBar.max_value=$cooldownTimer.wait_time #setup cooldown bar
	global.cardUI=self
	rng.randomize()
	cardNum = spellCards.size()
	for i in handSize: #initialize array of cards
		cardArray.append("0")


func _process(delta):
	if(shuffling): 
		$Control/timerBar.value=$Control/timerBar.max_value-$shuffleTimer.time_left
	else:
		$Control/timerBar.value=$Control/timerBar.max_value-$cooldownTimer.time_left
		if Input.is_action_just_pressed("shuffle"): #start a shuffle, shuffle discards back into the deck
			$cooldownTimer.stop()
			shuffleTimerSetup()
			shuffleDiscard()
			if(cardChoicesOut): #if the game is paused with the 
				get_tree().paused = false
				$Control/PanelContainer.visible = false
				$Control/timerBar.visible=true
				cardChoicesOut=false
				
	if(cardChoicesOut): #handle the player picking one of the available cards shown to them
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

func buttonPress(choice): #handle the player picking a card from the available options shown while the game is paused
	if(choice>choices): #if player picks an invalid option
		return
	get_tree().paused = false 
	var tmpSpell = spellCards.get(cardArray[choice-1])
	addToHand(tmpSpell)
	updatedDeck.remove_at(chosenCardIndex[choice-1]) #remove chosen card from deck and add it to the hand


	$Control/PanelContainer.visible = false
	$Control/timerBar.visible=true
	cardChoicesOut=false
	if(updatedDeck.size()>0): #if there are cards left in the deck, normal cooldown applies
		$cooldownTimer.start()
	else: #otherwise, start a shuffle
		shuffleTimerSetup()

	
func dealChoices(): #pause the game and present the player with up to 5 cards to choose from to add to their hand
	if(updatedDeck.size()==0):
		if(discard.size()>0):
			shuffleDiscard()
		else:
			return
	canDraw=false
	#hideIMG()
	cardChoicesOut = true
	$Control/timerBar.visible=false
	get_tree().paused = true
	var usedNums = []
	if(updatedDeck.size()<CHOICESGOAL):  #handle the deck having less than the desired amount cards to choose from
		choices=updatedDeck.size()
		for i in range(choices,CHOICESGOAL):
			buttons[i].set_texture(blankSpellCard)
			costlabels[i].text = ""
	else:
		choices=CHOICESGOAL
	
	for i in range(choices): #pick a random card in the deck for each possible choice
		var num = rng.randi_range(0,updatedDeck.size()-1)
		while(usedNums.has(num)):
			num = rng.randi_range(0,updatedDeck.size()-1)
		usedNums.append(num)
		cardArray[i]=(updatedDeck[num])
		chosenCardIndex[i]=num
		var curCard = spellCards.get(cardArray[i])
		buttons[i].set_texture(curCard.card)

		costlabels[i].text = str(curCard.size)
	$Control/PanelContainer.visible = true #make the choices UI visible

	
func shuffleTimerSetup(): #start shuffling the deck, and indicate the time it will take
	$shuffleTimer.wait_time = deck.size()/4
	$Control/timerBar.max_value=$shuffleTimer.wait_time
	$Control/timerBar.add_theme_stylebox_override("fill",greyBar)
	$shuffleTimer.start()
	shuffling = true

#recieve input from the choice UI button elements
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


func hideIMG():
	global.icon.visible = false 


func _on_cooldown_timer_timeout():
	canDraw = true 
	
func used(): #handle a card being consumeed, add it to the discard and remove it from the player's hand
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
		
func shuffleDiscard(): #helped function to put the discard pile back in the deck and clear the discard pile after
	if(discard.size()>0):
		updatedDeck+=discard
	discard.clear()
	
func addToHand(sp): #add a chosen spell to the player's hand/queue, either to the front of the hand or back depending on if cast is held down
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


func _on_shuffle_timer_timeout(): #end shuffling, and switch to the normal cooldown
	shuffling = false
	$Control/timerBar.max_value=$cooldownTimer.wait_time
	$Control/timerBar.add_theme_stylebox_override("fill",greenBar)
	$cooldownTimer.start() # Replace with function body.
