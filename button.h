#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

class Button {
public:
    Button(sf::Font& font, const std::string& text, sf::Vector2f position, sf::Vector2f size);

    void handleEvent(sf::Event event);
    void update(sf::Vector2f mousePos);
    void render(sf::RenderWindow& window);
    bool contains(sf::Vector2f mousePos) const;

private:
    sf::RectangleShape shape;
    sf::Text label;
    bool isHovered;
    bool isPressed;
    sf::Keyboard::Key assignedKey; // Nowe pole przechowuj¹ce przypisany klawisz
};

#endif // BUTTON_H
