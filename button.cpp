#include <SFML/Graphics.hpp>
#include "Button.h"

Button::Button(sf::Font& font, const std::string& text, sf::Vector2f position, sf::Vector2f size)
    : isHovered(false), isPressed(false) {

    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::Black);

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(18);
    label.setPosition(position.x + size.x / 2 - label.getGlobalBounds().width / 2,
        position.y + size.y / 2 - label.getGlobalBounds().height / 2);
    label.setFillColor(sf::Color::Black);
}
void Button::handleEvent(sf::Event event) {
    if (event.type == sf::Event::MouseMoved) {
        isHovered = shape.getGlobalBounds().contains(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (isHovered) {
            isPressed = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        if (isPressed && isHovered) {
            
           isPressed = false;
        }
    }
}

void Button::update(sf::Vector2f mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        isHovered = true;
    }
    else {
        isHovered = false;
    }
}

void Button::render(sf::RenderWindow& window) {
    if (isHovered) {
        shape.setFillColor(sf::Color(200, 200, 200)); // Kolor podœwietlenia
    }
    else {
        shape.setFillColor(sf::Color::White);
    }
    window.draw(shape);
    window.draw(label);
}
bool Button::contains(sf::Vector2f mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}