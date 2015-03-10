#ifndef _MAP_DEBUG_SHAPE_H_
#define _MAP_DEBUG_SHAPE_H_

#include <Utils.h>

#include <SFML/Graphics.hpp>

class DebugShape : public sf::Drawable, public sf::Transformable
{
public:

	DebugShape();

	void addVertex(const sf::Vertex& vert);

	void reset();

	void closeShape();

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:

	bool m_closed;

	sf::VertexArray m_array;
};

#endif
