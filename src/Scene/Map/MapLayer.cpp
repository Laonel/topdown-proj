#include <Scene/Map/MapLayer.h>

TileQuad::TileQuad(sf::Uint16 i0, sf::Uint16 i1, sf::Uint16 i2, sf::Uint16 i3) :
	m_needsUpdate(false)
{
	m_indices[0] = i0;
	m_indices[1] = i1;
	m_indices[2] = i2;
	m_indices[3] = i3;
}

void TileQuad::move(const sf::Vector2f& distance)
{
	m_movement = distance;
	m_needsUpdate = true;
}

LayerSet::LayerSet(const sf::Texture& texture) :
	m_texture(texture),
	m_visible(true)
{
}

TileQuad::Ptr LayerSet::addTile(sf::Vertex v0, sf::Vertex v1, sf::Vertex v2, sf::Vertex v3)
{
	m_vertices.push_back(v0);
	m_vertices.push_back(v1);
	m_vertices.push_back(v2);
	m_vertices.push_back(v3);

	sf::Uint16 i = m_vertices.size() - 4u;
	m_quads.push_back(std::make_shared<TileQuad>(i, i + 1, i + 2, i + 3));

	_updateAABB(v0.position, v2.position);

	return m_quads.back();
}

void LayerSet::cull(const sf::FloatRect& bounds)
{
	if (bounds.contains(m_boundingBox.left, m_boundingBox.top) ||
		bounds.contains(m_boundingBox.width, m_boundingBox.height) ||
		m_boundingBox.contains(bounds.left, bounds.top) ||
		m_boundingBox.contains(bounds.left + bounds.width, bounds.top + bounds.height))
	{
		m_visible = true;
	}
	else
	{
		m_visible = false;
	}
}

void LayerSet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto& q : m_quads)
	{
		if (q->m_needsUpdate)
		{
			for (auto& i : q->m_indices)
				m_vertices[i].position += q->m_movement;

			q->m_needsUpdate = false;
		}
	}

	if (!m_vertices.empty() && m_visible)
	{
		states.texture = &m_texture;
		target.draw(&m_vertices[0], static_cast<unsigned int>(m_vertices.size()), sf::Quads, states);
	}
}

void LayerSet::_updateAABB(sf::Vector2f position, sf::Vector2f size)
{
	if (m_boundingBox.width == 0.f)
	{
		// not been set yet so take on initial size
		m_boundingBox = sf::FloatRect(position, size);
		return;
	}

	if (position.x < m_boundingBox.left)
		m_boundingBox.left = position.x;

	if (position.y < m_boundingBox.top)
		m_boundingBox.top = position.y;

	if (size.x > m_boundingBox.left + m_boundingBox.width)
		m_boundingBox.width = size.x - m_boundingBox.left;

	if (size.y > m_boundingBox.top + m_boundingBox.height)
		m_boundingBox.height = size.y - m_boundingBox.top;
}

MapLayer::MapLayer(MapLayerType type) :
	opacity(1.f),
	visible(true),
	type(type),
	m_shader(nullptr)
{
}

void MapLayer::setShader(const sf::Shader& shader)
{
	m_shader = &shader;
}

void MapLayer::cull(const sf::FloatRect& bounds)
{
	for (auto& ls : layerSets)
		ls.second->cull(bounds);
}

void MapLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!visible)
		return;

	states.shader = m_shader;
	for (const auto& ls : layerSets)
		target.draw(*ls.second, states);

	if (type == ImageLayer)
		for (const auto& tile : tiles)
			target.draw(tile.sprite, tile.states);
}
