#ifndef _QUADTREE_NODE_H_
#define _QUADTREE_NODE_H_

#include <Utils.h>

#include <Scene/Map/MapObject.h>

class QuadTreeNode : public sf::Drawable
{
public:

	QuadTreeNode(sf::Uint16 level = 0, const sf::FloatRect& bounds = sf::FloatRect(0.f, 0.f, 0.f, 0.f));
	
	virtual ~QuadTreeNode() { }

	std::vector<MapObject*> retrieve(const sf::FloatRect& bounds, sf::Uint16& currentDepth);

	void insert(const MapObject& object);

protected:

	sf::Int16 _getIndex(const sf::FloatRect& bounds);

	void _split();

protected:

	const sf::Uint16 MAX_OBJECTS;
	const sf::Uint16 MAX_LEVELS;

	sf::Uint16 m_level;
	sf::FloatRect m_bounds;
	std::vector<MapObject*> m_objects;
	std::vector<std::shared_ptr<QuadTreeNode>> m_children;
	sf::RectangleShape m_debugShape;

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};

class QuadTreeRoot : public QuadTreeNode
{
public:

	QuadTreeRoot(sf::Uint16 level = 0, const sf::FloatRect& bounds = sf::FloatRect(0.f, 0.f, 0.f, 0.f)) :
		QuadTreeNode(level, bounds), m_depth(0u), m_searchDepth(0u)
	{
	}

	void clear(const sf::FloatRect& newBounds);

	std::vector<MapObject*> retrieve(const sf::FloatRect& bounds)
	{
		return QuadTreeNode::retrieve(bounds, m_searchDepth);
	}

private:

	sf::Uint16 m_depth, m_searchDepth;

};

#endif
