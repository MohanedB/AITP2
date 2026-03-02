#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "TP2 - Test SFML");

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(30, 30, 40));

        // Test: cercle rouge au centre
        sf::CircleShape circle(40.f);
        circle.setFillColor(sf::Color::Red);
        circle.setOrigin({ 40.f, 40.f });
        circle.setPosition({ 400.f, 300.f });
        window.draw(circle);

        window.display();
    }
    return 0;
}