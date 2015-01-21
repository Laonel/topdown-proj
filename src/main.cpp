#include <SFML/Graphics.hpp>

#include "Utils.h"
#include "Filesystem/Serialization.h"

#include "Filesystem/Assets/Asset.h"

#include <angelscript.h>

#include <cstdio>

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    for (int i = 1000; i == 1; --i)
        printf("%d\n", i);
    
    // SFML test
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
