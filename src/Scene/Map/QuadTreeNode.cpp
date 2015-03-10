#include <Scene/Map/QuadTreeNode.h>

QuadTreeNode::QuadTreeNode(sf::Uint16 level, const sf::FloatRect& bounds) :
	MAX_OBJECTS(5u),
	MAX_LEVELS(5u),
	m_level(level),
	m_bounds(bounds)
{
	m_children.reserve(4);
	m_debugShape = sf::RectangleShape(sf::Vector2f(bounds.width, bounds.height));
	m_debugShape.setPosition(bounds.left, bounds.top);
	m_debugShape.setFillColor(sf::Color::Transparent);
	m_debugShape.setOutlineColor(sf::Color::Green);
	m_debugShape.setOutlineThickness(-2.f);
}

void QuadTreeRoot::clear(const sf::FloatRect& newBounds)
{
	m_objects.clear();
	m_children.clear();
	m_bounds = newBounds;
	m_debugShape.setPosition(newBounds.left, newBounds.top);
	m_debugShape.setSize(sf::Vector2f(newBounds.width, newBounds.height));

	m_searchDepth = 0u;
	m_depth = 0;
}

std::vector<MapObject*> QuadTreeNode::retrieve(const sf::FloatRect& bounds, sf::Uint16& searchDepth)
{
	searchDepth = m_level;
	std::vector<MapObject*> foundObjects;
	sf::Int16 index = _getIndex(bounds);

	// recursively add objects of child node if bounds is fully contained
	if (!m_children.empty() && index != -1)
	{
		foundObjects = m_children[index]->retrieve(bounds, searchDepth);
	}
	else
	{
		for (auto& child : m_children)
		{
			if (bounds.intersects(child->m_bounds))
			{
				std::vector<MapObject*> childObjects = child->retrieve(bounds, searchDepth);
				foundObjects.insert(foundObjects.end(), childObjects.begin(), childObjects.end());
			}
		}
	}

	// and append objects in this node
	foundObjects.insert(foundObjects.end(), m_objects.begin(), m_objects.end());
	m_debugShape.setOutlineColor(sf::Color::Red);
	return foundObjects;
}

void QuadTreeNode::insert(const MapObject& object)
{
	if (!object.getAABB().intersects(m_bounds))
		return;

	if (!m_children.empty())
	{
		sf::Int16 index = _getIndex(object.getAABB());
		if (index != -1)
		{
			m_children[index]->insert(object);
			return;
		}
	}
	// else add object to this node
	m_objects.push_back(const_cast<MapObject*>(&object));

	if (m_objects.size() > MAX_OBJECTS && m_level < MAX_LEVELS)
	{
		if (m_children.empty())
			_split();

		sf::Uint16 i = 0;
		while (i < m_objects.size())
		{
			sf::Int16 index = _getIndex(m_objects[i]->getAABB());
			if (index != -1)
			{
				m_children[index]->insert(*m_objects[i]);
				m_objects.erase(m_objects.begin() + i);
			}
			else
			{
				i++;
				// we only increment i when not erasing, brcause erasing moves
				// everything up one inside the vector
			}
		}
	}
}

void QuadTreeNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// recursively draw children
	for (auto& child : m_children)
		target.draw(*child);

	target.draw(m_debugShape);
}

void QuadTreeNode::_split()
{
	const float halfWidth = m_bounds.width / 2.f;
	const float halfHeight = m_bounds.height / 2.f;
	const float x = m_bounds.left;
	const float y = m_bounds.top;

	m_children.push_back(std::make_shared<QuadTreeNode>(m_level + 1, sf::FloatRect(x + halfWidth, y, halfWidth, halfHeight)));
	m_children.push_back(std::make_shared<QuadTreeNode>(m_level + 1, sf::FloatRect(x, y, halfWidth, halfHeight)));
	m_children.push_back(std::make_shared<QuadTreeNode>(m_level + 1, sf::FloatRect(x, y + halfHeight, halfWidth, halfHeight)));
	m_children.push_back(std::make_shared<QuadTreeNode>(m_level + 1, sf::FloatRect(x+ halfWidth, y + halfHeight, halfWidth, halfHeight)));
}

sf::Int16 QuadTreeNode::_getIndex(const sf::FloatRect& bounds)
{
	sf::Int16 index = -1;
	float verticalMidpoint = m_bounds.left + (m_bounds.width / 2.f);
	float horizontalMidpoint = m_bounds.top + (m_bounds.height / 2.f);

	bool topQuadrant = (bounds.top < horizontalMidpoint && bounds.top + bounds.height < horizontalMidpoint);
	bool bottomQuadrant = (bounds.top > horizontalMidpoint);

	if (bounds.left < verticalMidpoint && bounds.left + bounds.width < verticalMidpoint)
	{
		if (topQuadrant)
			index = 1;
		else if (bottomQuadrant)
			index = 2;
	}
	// object can completely fit within the right quadrants
	else if (bounds.left > verticalMidpoint)
	{
		if (topQuadrant)
			index  = 0;
		else if (bottomQuadrant)
			index = 3;
	}

	return index;
}
