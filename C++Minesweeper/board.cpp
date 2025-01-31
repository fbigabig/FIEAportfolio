//board class, handles most of the game logic and holds most of the game data
#include "board.h"
board::tile::tile(int x, int y, board& b) { //tile class constructor
	xc = x;
	yc = y;
	mine = false;
	vis = false;
	flagged = false;
	num = 0;
	box.setTexture(b.hide);
	flag.setTexture(b.flag);
	flag.setPosition(sf::Vector2f(x * 32, y * 32));
	sprite.setPosition(sf::Vector2f(x * 32, y * 32));
	box.setPosition(sf::Vector2f(x * 32, y * 32));
}
bool board::tile::rc() { //flag a tile when you right click
	flagged = !flagged;
	return flagged;
}
bool board::tile::arm() { //place a mine on a tile if a tile doesn't already have a mine
	if (mine) {
		return true;
	}
	mine = true;
	return false;
}

void board::tile::initadj(board& b) { //set up a tile's vector of adjacent tiles
	for (int i = -1; i <= 1; i++) {
		if (i + xc < 0 || i + xc >= b.col) {
			continue;
		}
		for (int j = -1; j <= 1; j++) {
			if ((i == 0 && j == 0) || (j + yc < 0 || j + yc >= b.rows)) {
				continue;
			}
			adj.push_back(b.get(xc + i, yc + j));

		}
	}

}
int board::clear(tile t) {
	return 0;
}
board::tile* board::get(int x, int y) {
	return &tiles.at(x).at(y);
}
bool board::checkcount() { //get the count of how many flags are left
	int count = mines - flags;
	vector<int> tens = { 100, 10, 1 };
	if (count < 0) {
		count *= -1;
	}
	for (int i = 1; i < 4; i++) {

		if (count > 9.999 * tens.at(i - 1)) {
			digits.at(i).setTextureRect(sf::IntRect(21 * 9, 0, 21, 32));
		}
		else {
			int tc = count / tens.at(i - 1);
			//cout << tc << endl;
			if (tc > 9) tc = 9;
			digits.at(i).setTextureRect(sf::IntRect(21 * tc, 0, 21, 32));
			count %= tens.at(i - 1);
		}

	}
	return mines - flags < 0;
}
void board::reset() { //reset the board for a new game
	ifstream f;
	f.open("./boards/config.cfg");
	f >> col;
	f >> rows;
	f >> mines;
	f.close();
	flags = 0;
	tiles.clear();
	boom.clear();
	over = false;
	revcount = 0;
	win = false;
	face.setTexture(faces[0]);
	for (int i = 0; i < col; i++)  //set up the tile 2d vector
	{
		vector<tile> temp;
		for (int j = 0; j < rows; j++)
		{
			temp.push_back(tile(i, j, *this));
		}
		tiles.push_back(temp);
	}
	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			get(i, j)->initadj(*this);
		}
	}
	std::random_device rng;
	std::mt19937 generator(rng());
	std::uniform_int_distribution<> distro_x(0, col - 1);
	std::uniform_int_distribution<> distro_y(0, rows - 1);
	for (int i = 0; i < mines; i++) //randomly place all the mines
	{
		int x = distro_x(generator);
		int y = distro_y(generator);
		if (tiles.at(x).at(y).arm()) {
			i--;
		}
		else {
			boom.push_back(get(x, y));
		}
	}
	nums();
}
void board::load(int t) { //load one of the 3 test boards
	flags = 0;
	mines = 0;
	tiles.clear();
	boom.clear();
	over = false;
	revcount = 0;
	win = false;
	face.setTexture(faces[0]);
	ifstream f;
	string s = "./boards/testBoard";
	s += to_string(t);
	s += ".brd";
	f.open(s);
	for (int i = 0; i < col; i++)
	{
		vector<tile> temp;
		for (int j = 0; j < rows; j++)
		{
			temp.push_back(tile(i, j, *this));
		}
		tiles.push_back(temp);
	}
	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			get(i, j)->initadj(*this);
		}
	}
	for (int j = 0; j < rows; j++)
	{
		string s;
		f >> s;
		for (int i = 0; i < col; i++) {
			int num = s.at(i) - '0';
			if (num == 1) {
				tiles.at(i).at(j).arm();
				boom.push_back(get(i, j));
				mines++;

			}
		}
	}
	nums();
	f.close();
}

board::board(int l, int h, int m) { //board constructor, set it up with a size and minecount
	over = false;

	col = l;
	rows = h;
	mines = m;
	flags = 0;
	win = false;
	debugOn = false;
	revcount = 0;
	sf::Texture tex;
	flag.loadFromFile("./images/flag.png");
	show.loadFromFile("./images/tile_revealed.png");
	hide.loadFromFile("./images/tile_hidden.png");
	mine.loadFromFile("./images/mine.png");
	for (int i = 1; i < 9; i++) {
		string s = "./images/number_";
		s += to_string(i);
		s += ".png";
		tex.loadFromFile(s);
		texs.push_back(tex);
	}
	tests = { sf::Sprite(), sf::Sprite(), sf::Sprite() };
	for (int i = 1; i <= 3; i++) {
		string s = "./images/test_";
		s += to_string(i);
		s += ".png";
		tex.loadFromFile(s);
		buttons.push_back(tex);

	}
	//set up all the texture images
	tests.at(0).setTexture(buttons.at(0));
	tests.at(1).setTexture(buttons.at(1));
	tests.at(2).setTexture(buttons.at(2));
	all.push_back(&tests.at(0));
	all.push_back(&tests.at(1));
	all.push_back(&tests.at(2));

	tex.loadFromFile("./images/face_happy.png");
	faces.push_back(tex);
	tex.loadFromFile("./images/face_win.png");
	faces.push_back(tex);
	tex.loadFromFile("./images/face_lose.png");
	faces.push_back(tex);
	face.setTexture(faces[0]);

	numbs.loadFromFile("./images/digits.png");
	deb.loadFromFile("./images/debug.png");
	debug.setTexture(deb);
	all.push_back(&debug);
	digits = { sf::Sprite(), sf::Sprite(), sf::Sprite(), sf::Sprite() };
	for (int i = 0; i < 4; i++) {
		digits.at(i).setTexture(numbs);
		digits.at(i).setPosition(sf::Vector2f(i * 21, 32 * rows));
	}
	digits.at(0).setTextureRect(sf::IntRect(21 * 10, 0, 21, 32));
	checkcount();
	face.setPosition(col * 32 / 2 - 32, rows * 32);
	for (int i = 3; i > 0; i--) {
		sf::Sprite s;
		tests.at(3 - i).setPosition((col * 32 - i * 64), rows * 32);
	}
	debug.setPosition(col * 32 - 64 * 4, rows * 32);

	for (int i = 0; i < col; i++)
	{
		vector<tile> temp;
		for (int j = 0; j < rows; j++)
		{
			temp.push_back(tile(i, j, *this));
		}
		tiles.push_back(temp);
	}
	for (int i = 0; i < col; i++) //set up the adjacent vectors for each tile
	{
		for (int j = 0; j < rows; j++)
		{
			get(i, j)->initadj(*this);
		}
	}
	std::random_device rng; //randomly place the mines
	std::mt19937 generator(rng());
	std::uniform_int_distribution<> distro_x(0, col - 1);
	std::uniform_int_distribution<> distro_y(0, rows - 1);
	for (int i = 0; i < mines; i++)
	{
		int x = distro_x(generator);
		int y = distro_y(generator);
		if (tiles.at(x).at(y).arm()) {
			i--;
		}
		else {
			boom.push_back(get(x, y));
		}
	}
	nums();
}
void board::gameover() { //end a game, reveal all the mines and remove all the flags
	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < rows; j++) {
			tile* t = get(i, j);
			tr.push_back(t);
			t->box.setTexture(show);
			t->vis = true;
			t->flagged = false;
		}
	}
	face.setTexture(faces[2]);
	over = true;
}

void board::nums() { //count how many mines are adjacent for each tile
	for (tile* a : boom)
	{
		a->sprite.setTexture(mine);
		//cout << "a";
		for (tile* b : a->adj)
		{

			if (!b->mine) {
				b->num++;

			}
		}
	}
	for (int i = 0; i < col; i++)
	{
		for (int j = 0; j < rows; j++) {
			tile* t = get(i, j);
			//cout << t->num << endl;
			if (t->num > 0) {
				t->sprite.setTexture(texs.at(t->num - 1));
			}
		}
	}
}
void board::reveal(tile* t) { //reveal a tile if it hasn't been revealed or flagged, and reveal adjacent tiles if this tile's adjacent mine count is 0
	if (t->vis || t->flagged) {
		return;
	}
	tr.push_back(t);
	t->box.setTexture(show);
	t->vis = true;
	revcount++;
	if (t->num > 0) {
		return;
	}
	for (tile* a : t->adj) {
		reveal(a);
	}
}

void board::clicked(int x, int y, bool rc) { //handle a mouse click
	if (x < 32 * col && y < 32 * rows && !over) { //if a tile was clicked
		x /= 32;
		y /= 32;
		tile* t = get(x, y);
		if (rc) { //flag it if it was a right click
			if (over) {
				return;
			}
			bool f = t->rc();
			if (f) {
				flags++;
			}
			else {
				flags--;
			}
		}
		else if (!t->flagged) { //if it was a left click
			if (t->mine) {//end the game if a mine was clicked
				gameover();
			}
			else {
				reveal(t);
				if (revcount == col * rows - mines) { //otherwise, reveal the tile
					over = true;
					win = true;
					face.setTexture(faces[1]);
					for (tile* a : boom) {
						if (!a->flagged) {
							a->rc();
							flags++;
						}
					}
				}
			}
		}
	}
	else if (!rc) { //if another section of the game was left clicked
		if (x >= face.getPosition().x && x <= face.getPosition().x + 64 && y >= face.getPosition().y && y <= face.getPosition().y + 64)  //if the reset button was clicked, reset the game
			reset();
		}
		else {
			for (sf::Sprite* a : all) { //if another button was hit
				if (x >= a->getPosition().x && x <= a->getPosition().x + 64 && y >= a->getPosition().y && y <= a->getPosition().y + 64) {
					if (a->getTexture() == &deb) { //if the debug button was hit, enter debug mode
						debugOn = !debugOn;
					}
					//if one of the test board buttons was clicked, load that test board
					else if (a->getTexture() == &buttons[0]) {
						load(1);
					}
					else if (a->getTexture() == &buttons[1]) {
						load(2);
					}
					else if (a->getTexture() == &buttons[2]) {
						load(3);

					}
				}

			}
		}
	}
}
