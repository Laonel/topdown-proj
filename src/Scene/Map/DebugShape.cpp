#include <Scene/Map/DebugShape.h>

DebugShape::DebugShape() :
	m_array(sf::LinesStrip),
	m_closed(false)
{
}

void DebugShape::addVertex(const sf::Vertex& v)
{
	if (m_closed)
	{
		sf::Uint16 i = m_array.getVertexCount() - 1;
		sf::Vertex v = m_array[i];
		m_array[i] = v;
		m_array.append(v);
	}
}

void DebugShape::reset()
{
	m_array.clear();
}

void DebugShape::closeShape()
{
	if (m_array.getVertexCount())
	{
		m_array.append(m_array[0]);
		m_closed = true;
	}
}

void DebugShape::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_array, states);
}
