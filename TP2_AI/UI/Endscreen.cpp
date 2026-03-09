#include "EndScreen.h"

EndScreen::EndScreen(sf::Vector2u size)
    : titleText(font), subtitleText(font), retryText(font)
{
    windowSize = size;

    fontLoaded = font.openFromFile("arial.ttf") ||
                 font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
                 font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

    overlay.setSize(sf::Vector2f(size));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));

    retryBtn.setSize(sf::Vector2f(200.f, 55.f));
    retryBtn.setFillColor(sf::Color(60, 60, 200));
    retryBtn.setOutlineThickness(2.f);
    retryBtn.setOutlineColor(sf::Color::White);

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
    if (!fontLoaded)
        return;

    if (result == EndResult::Captured)
    {
        titleText.setString("CAPTURÉ !");
        titleText.setFillColor(sf::Color::Red);
        subtitleText.setString("L'intrus a ete capturé !");
    }
    else
    {
        titleText.setString("MISSION ACCOMPLIE !");
        titleText.setFillColor(sf::Color::Green);
        subtitleText.setString("L'intrus a atteint l'objectif !");
    }

    float cx = windowSize.x / 2.f;
    float cy = windowSize.y / 2.f;

    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(tb.size.x / 2.f, tb.size.y / 2.f));
    titleText.setPosition(sf::Vector2f(cx, cy - 80.f));

    sf::FloatRect sb = subtitleText.getLocalBounds();
    subtitleText.setOrigin(sf::Vector2f(sb.size.x / 2.f, sb.size.y / 2.f));
    subtitleText.setPosition(sf::Vector2f(cx, cy));

    retryBtn.setOrigin(sf::Vector2f(100.f, 27.5f));
    retryBtn.setPosition(sf::Vector2f(cx, cy + 80.f));

    sf::FloatRect rb = retryText.getLocalBounds();
    retryText.setOrigin(sf::Vector2f(rb.size.x / 2.f, rb.size.y / 2.f));
    retryText.setPosition(sf::Vector2f(cx, cy + 80.f));
}

bool EndScreen::HandleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (click->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos;
            mousePos.x = static_cast<float>(click->position.x);
            mousePos.y = static_cast<float>(click->position.y);

            if (retryBtn.getGlobalBounds().contains(mousePos))
                return true;
        }
    }

    if (event.is<sf::Event::MouseMoved>())
    {
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos;
        mousePos.x = static_cast<float>(mouseI.x);
        mousePos.y = static_cast<float>(mouseI.y);

        if (retryBtn.getGlobalBounds().contains(mousePos))
            retryBtn.setFillColor(sf::Color(90, 90, 255));
        else
            retryBtn.setFillColor(sf::Color(60, 60, 200));
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