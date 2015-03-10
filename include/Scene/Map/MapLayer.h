#ifndef _MAP_LAYER_H_
#define _MAP_LAYER_H_

#include <Utils.h>

#include <Scene/Map/MapObject.h>

class LayerSet;

class TileQuad
{
	friend class LayerSet;

public:

	typedef std::shared_ptr<TileQuad> Ptr;

	TileQuad(sf::Uint16 i0, sf::Uint16 i1, sf::Uint16 i2, sf::Uint16 i3);

	void move(const sf::Vector2f& distance);

private:

	std::array<sf::Uint16, 4u> m_indices;
	sf::Vector2f m_movement;
	bool m_needsUpdate;

};

// drawable composed of vertices representing a set of tiles on a layer
class LayerSet : public sf::Drawable
{
public:

	LayerSet(const sf::Texture& texture);

	TileQuad::Ptr addTile(sf::Vertex v0, sf::Vertex v1, sf::Vertex v2, sf::Vertex v3);

	void cull(const sf::FloatRect& bounds);

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void _updateAABB(sf::Vector2f position, sf::Vector2f size);

private:

	const sf::Texture& m_texture;
	mutable std::vector<TileQuad::Ptr> m_quads;
	mutable std::vector<sf::Vertex> m_vertices;

	sf::FloatRect m_boundingBox;
	bool m_visible;
};

// used to query the type of layer, for example when looking for layers containing collision objects
enum MapLayerType
{
	Layer,
	ObjectGroup,
	ImageLayer
};

// represents a layer of tiles, corresponding to a tmx layer, object group or image layer
class MapLayer : public sf::Drawable
{
public:

	// used for drawing specific layers
	enum DrawType
	{
		Front,
		Back,
		Debug,
		All
	};

	MapLayer(MapLayerType layerType);

	std::string name;
	float opacity;
	bool visible;
	MapTiles tiles;
	MapObjects objects;
	MapLayerType type;
	std::map<std::string, std::string> properties;

	std::map<sf::Uint16, std::shared_ptr<LayerSet>> layerSets;
	void setShader(const sf::Shader& shader);
	void cull(const sf::FloatRect& bounds);

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const sf::Shader* m_shader;
};

#endif
