#include "EndScreen.h"

EndScreen::EndScreen(sf::Vector2u size)
    : titleText(font), subtitleText(font), retryText(font), windowSize(size)
{
    overlay.setSize(sf::Vector2f(size));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    // Bouton Retry
    retryBtn.setSize({ 200.f, 55.f });
    retryBtn.setFillColor(sf::Color(60, 60, 200));
    retryBtn.setOutlineThickness(2.f);
    retryBtn.setOutlineColor(sf::Color::White);

    fontLoaded = font.openFromFile("arial.ttf") ||
                 font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
                 font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

    if (fontLoaded)
    {
        titleText.setFont(font);
        titleText.setCharacterSize(72);
        titleText.setStyle(sf::Text::Bold);

        subtitleText.setFont(font);
        subtitleText.setCharacterSize(22);
        subtitleText.setFillColor(sf::Color::White);

        retryText.setFont(font);
        retryText.setString("Rejouer");
        retryText.setCharacterSize(26);
        retryText.setFillColor(sf::Color::White);
        retryText.setStyle(sf::Text::Bold);
    }
}

void EndScreen::Show(EndResult result)
{
    if (!fontLoaded) return;

    if (result == EndResult::Captured)
    {
        titleText.setString("CAPTURE !");
        titleText.setFillColor(sf::Color::Red);
        subtitleText.setString("L'intrus a ete capture !");
    }
    else
    {
        titleText.setString("MISSION ACCOMPLIE !");
        titleText.setFillColor(sf::Color::Green);
        subtitleText.setString("L'intrus a atteint l'objectif !");
    }

    Layout();
}

void EndScreen::Layout()
{
    float cx = windowSize.x / 2.f;
    float cy = windowSize.y / 2.f;

    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    titleText.setPosition({ cx, cy - 80.f });

    sf::FloatRect sb = subtitleText.getLocalBounds();
    subtitleText.setOrigin({ sb.size.x / 2.f, sb.size.y / 2.f });
    subtitleText.setPosition({ cx, cy });

    // Bouton centré en bas
    retryBtn.setOrigin({ 100.f, 27.5f });
    retryBtn.setPosition({ cx, cy + 80.f });

    sf::FloatRect rb = retryText.getLocalBounds();
    retryText.setOrigin({ rb.size.x / 2.f, rb.size.y / 2.f });
    retryText.setPosition({ cx, cy + 80.f });
}

bool EndScreen::HandleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (click->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mouse(static_cast<float>(click->position.x),
                               static_cast<float>(click->position.y));
            if (retryBtn.getGlobalBounds().contains(mouse))
                return true;
        }
    }

    // Hover : changer la couleur du bouton
    if (event.is<sf::Event::MouseMoved>())
    {
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse(static_cast<float>(mouseI.x), static_cast<float>(mouseI.y));
        bool hovered = retryBtn.getGlobalBounds().contains(mouse);
        retryBtn.setFillColor(hovered ? sf::Color(90, 90, 255) : sf::Color(60, 60, 200));
    }

    return false;
}

void EndScreen::Draw(sf::RenderWindow& window)
{
    window.draw(overlay);
    if (fontLoaded)
    {
        window.draw(titleText);
        window.draw(subtitleText);
        window.draw(retryBtn);
        window.draw(retryText);
    }
}