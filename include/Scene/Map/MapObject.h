#ifndef _MAP_OBJECT_H_
#define _MAP_OBJECT_H_

#include <Utils.h>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <Scene/Map/DebugShape.h>

#include <Math/Vector2.h>

class TileQuad;

enum MapObjectShape
{
	Rectangle,
	Ellipse,
	Circle,
	Polygon,
	Polyline,
	Tile
};

class MapObject
{
public:

	MapObject();

	// returns empty string if property not found
	std::string getPropertyString(const std::string& name);

	// sets a property value, adds it if property doesn't exist
	void setProperty(const std::string& name, const std::string& value);

	// returns top left corner of bouding rectangle
	sf::Vector2f getPosition() const { return m_position; }

	void setPosition(float x, float y);

	void setPosition(const sf::Vector2f& position);

	sf::Vector2f getCentre() const { return m_centrePoint; }

	MapObjectShape getShapeType() const { return m_shape; }

	void setShapeType(MapObjectShape shape) { m_shape = shape; }

	std::string getName() const { return m_name; }

	void setName(const std::string& name) { m_name = name; }

	std::string getType() const { return m_type; }

	void setType(const std::string& type) { m_type = type; }

	std::string getParent() const { return m_parent; }

	void setParent(const std::string& parent) { m_parent = parent; }

	sf::FloatRect getAABB() const { return m_AABB; }

	bool isVisible() const { return m_visible; }

	void setVisible(bool visible) { m_visible = visible; }

	void move(float x, float y);

	void move(const sf::Vector2f& distance);

	void setSize(const sf::Vector2f& size) { m_size = size; }

	// call createDebugShape() afterwards to rebuild debug output
	void addPoint(const sf::Vector2f& point) { m_polypoints.push_back(point); }

	// checks if an object contains given point in world coords.
	// always returns false for polylines.
	bool contains(sf::Vector2f point) const;

	// checks if two objects intersect, including polylines
	bool intersects(const MapObject& object) const;

	// creates a shape used for debug drawing
	void createDebugShape(const sf::Color& color);

	// draws debug shape to given target
	void drawDebugShape(sf::RenderTarget& target) const;

	// returns first point of poly point member
	sf::Vector2f firstPoint() const;

	// returns last point of poly point member
	sf::Vector2f lastPoint() const;

	// returns a unit vector normal to the polyline segment if intersected
	// takes the start and end point of a trajectory
	sf::Vector2f collisionNormal(const sf::Vector2f& start, const sf::Vector2f& end) const;

	// creates a vector of segments making up the poly shape
	void createSegments();

	// returns if an objects poly shape is convex or not
	bool convex() const;

	// returns a reference to the array of points making up the object
	const std::vector<sf::Vector2f>& polyPoints() const;

	// reversing winding of object points
	void reverseWinding();

	// sets the quad used to draw the tile for tile objects
	void setQuad(std::shared_ptr<TileQuad> quad);

private:

	struct Segment
	{
		Segment(const sf::Vector2f& s, const sf::Vector2f& e) :
			start(s),
			end(e)
		{ }

		bool intersects(const Segment& segment);

		sf::Vector2f start;
		sf::Vector2f end;

	};

	sf::Vector2f _calcCentre() const;

	void _calcTestValues();

	void _createAABB();

private:

	std::string m_name, m_type, m_parent;
	sf::Vector2f m_position, m_size;
	std::map<std::string, std::string> m_properties;
	std::vector<sf::Vector2f> m_polypoints;
	MapObjectShape m_shape;
	DebugShape m_debugShape;
	sf::Vector2f m_centrePoint;

	bool m_visible;

	std::vector<Segment> m_polySegs;
	std::shared_ptr<TileQuad> m_tileQuad;

	float m_furthestPoint;

	sf::FloatRect m_AABB;

};

typedef std::vector<MapObject> MapObjects;

// represents single tile on a layer
struct MapTile
{
	sf::Vector2f getBase() const
	{
		return sf::Vector2f(sprite.getPosition().x + (sprite.getLocalBounds().width / 2.f),
							sprite.getPosition().y + sprite.getLocalBounds().height);
	}

	sf::Sprite sprite;
	sf::Vector2i gridCoord;
	sf::RenderStates states;
};

typedef std::vector<MapTile> MapTiles;

#endif
