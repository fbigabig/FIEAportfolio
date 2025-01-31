//board class header file, defines variables and function prototypes.
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <fstream>

using namespace std;
struct board {
	struct tile { //tile subclass, handles the data for one game tile
		sf::Sprite sprite;
		sf::Sprite box;
		sf::Sprite flag;
		bool flagged;
		bool mine;
		bool vis;
		int xc;
		int yc;
		int num;
		bool arm();
		bool rc();
		void initadj(board& b);
		vector<tile*> adj;
		tile(int x, int y, board& b);
	};
	//methods
	void reveal(tile* t);
	void gameover();
	void nums();
	void clicked(int x, int y, bool rc);
	void refresh();
    void load(int t);
	int clear(tile t);
	void reset();
	tile* get(int x, int y);
	board(int r, int c, int m);
	bool checkcount();

	//variables
	vector<sf::Texture> texs;
	sf::Texture mine;
	sf::Texture flag;
	sf::Texture hide;
	sf::Texture show;
	sf::Sprite face;
	vector<sf::Sprite*> all;
	vector<sf::Sprite> digits;
	sf::Sprite debug;
	vector<sf::Sprite> tests;
	sf::Texture numbs;
	vector<sf::Texture> faces;
	vector<sf::Texture> buttons;
	sf::Texture deb;
    vector<tile*> tr;
	vector<tile*> boom;
	vector<vector<tile>> tiles;

	int rows;
	int col;
	int flags;
	bool over;
	int mines;
	int revcount;
	bool debugOn;
	bool win;


};
