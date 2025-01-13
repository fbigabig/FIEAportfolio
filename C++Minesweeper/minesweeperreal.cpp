#include <SFML/Graphics.hpp> //SFML version 2.6
#include "board.h"
int main()
{
    ifstream f;
    f.open("./boards/config.cfg");
    int l;
    f >> l;
    int h;
    f >> h;
    int m;
    f >> m;
    sf::RenderWindow window(sf::VideoMode(l * 32, h * 32 + 88), "Minesweeper");
    board b(l, h, m);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    b.clicked(event.mouseButton.x, event.mouseButton.y, true);

                }
                else if (event.mouseButton.button == sf::Mouse::Left) {
                    b.clicked(event.mouseButton.x, event.mouseButton.y, false);
                }
            }
        }

        window.clear(sf::Color::White);


        for (int i = 0; i < l; i++) {
            for (int j = 0; j < h; j++) {
                // cout << i << j;
                board::tile* temp = b.get(i, j);
                window.draw(temp->box);
                if (temp->flagged) {
                    window.draw(temp->flag);
                }
                if (b.debugOn && !b.over) {
                    for (board::tile* a : b.boom) {
                        window.draw(a->sprite);
                    }
                }
                if ((temp->num > 0 && temp->vis) || (!b.win && b.over && (temp->num > 0 || temp->mine))) {
                    //cout << temp->num;
                    window.draw(temp->sprite);
                }


            }
        }
        for (int i = 0; i < 3; i++) {
            window.draw(b.tests[i]);
        }
        window.draw(b.debug);
        window.draw(b.face);
        if (b.checkcount()) {
            window.draw(b.digits[0]);

        }
        for (int i = 1; i < 4; i++) {
            window.draw(b.digits[i]);
        }

        window.display();
    }
    f.close();
    return 0;
}