#pragma once
#include <SFML/Graphics.hpp>
#include<vector>
using namespace std;

// get random unoccupied position for the no button
sf::Vector2f getRandomUnoccupiedPosition(int width, int height, sf::Vector2f buttonSize,
                                         vector<sf::FloatRect>& occupiedAreas) {
    sf::Vector2f newPos;
    bool validPos = false;
    while (!validPos) {
        float x = static_cast<float>(rand() % (width - static_cast<int>(buttonSize.x)));
        float y = static_cast<float>(rand() % (height - static_cast<int>(buttonSize.y)));
        sf::FloatRect newRect(x, y, buttonSize.x, buttonSize.y);
        validPos = true;
        for (int i = 0; i < occupiedAreas.size(); i++) {
            if (newRect.intersects(occupiedAreas[i])) {
                validPos = false;
                break;
            }
        }

        if (validPos) {
            newPos = sf::Vector2f(x, y);
        }
    }
    return newPos;
}
