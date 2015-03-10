#ifndef _MAP_LOADER_H_
#define _MAP_LOADER_H_

#include <Utils.h>

#include <Scene/Map/QuadTreeNode.h>
#include <Scene/Map/MapLayer.h>

#include <Filesystem/Xml/pugixml.h>

#include <bitset>

enum MapOrientation
{
	Orthogonal,
	Isometric,
	SteppedIsometric
};

class MapLoader : public sf::Drawable, private sf::NonCopyable
{
public:

	MapLoader(const std::string& mapDirectory);

	bool load(const std::string& mapFile);

	void addSearchPath(const std::string& path);

	void updateQuadTree(const sf::FloatRect& rootArea);

	std::vector<MapObject*> queryQuadTree(const sf::FloatRect& testArea);

	std::vector<MapLayer>& getLayers();

	const std::vector<MapLayer>& getLayers() const;

	void draw(sf::RenderTarget& target, MapLayer::DrawType type, bool debug = false);

	void draw(sf::RenderTarget& target, sf::Uint16 index, bool debug = false);

	sf::Vector2f isometricToOrthogonal(const sf::Vector2f& projectedCoords);

	sf::Vector2f orthogonalToIsometric(const sf::Vector2f& worldCoords);

	sf::Vector2u getMapSize() const;

	std::string getPropertyString(const std::string& name);

	void setLayerShader(sf::Uint16 layerId, const sf::Shader& shader);

	bool quadTreeAvailable() const;

private:

	struct TileInfo
	{
		TileInfo();
		TileInfo(const sf::IntRect& r, const sf::Vector2f& s, sf::Uint16 tsId);

		std::array<sf::Vector2f, 4> coords;
		sf::Vector2f size;
		sf::Uint16 tilesetId;
	};

	void _unload();

	void _setDrawingBounds(const sf::View& view);

	bool _parseMapNode(const pugi::xml_node& mapNode);
	bool _parseTileSets(const pugi::xml_node& mapNode);
	bool _processTiles(const pugi::xml_node& tlesetNode);
	bool _parseLayer(const pugi::xml_node& layerNode);
	bool _parseObjectGroup(const pugi::xml_node& groupNode);
	bool _parseImageLayer(const pugi::xml_node& imageLayerNode);
	bool _parseLayerProperties(const pugi::xml_node& propertiesNode, MapLayer& destLayer);

	TileQuad::Ptr _addTileToLayer(MapLayer& layer, sf::Uint16 x, sf::Uint16 y, sf::Uint32 gid, const sf::Vector2f& offset = sf::Vector2f());
	void _setIsometricCoords(MapLayer& layer);
	void _drawLayer(sf::RenderTarget& target, MapLayer& layer, bool debug = false);
	std::string _fileFromPath(const std::string& path);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	// utility method for parsing color values from hex values
	sf::Color _colorFromHex(const char* hexStr) const;

	// method for decompressing zlib compressed strings
	bool _decompress(const char* source, std::vector<unsigned char>& dest, int inSize, int expectedSize);

	void _createDebugGrid();

	sf::Image& _loadImage(const std::string& imageName);

	std::vector<unsigned char> _intToBytes(sf::Uint32 paramInt);
	std::pair<sf::Uint32, std::bitset<3>> _resolveRotation(sf::Uint32 gid);

	void _flipY(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
	void _flipX(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
	void _flipD(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);

	void _doFlips(std::bitset<3> bits, sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);

private:

	sf::Uint16 m_width, m_height;
	sf::Uint16  m_tileWidth, m_tileHeight;
	MapOrientation m_orientation;
	float m_tileRatio;
	std::map<std::string, std::string> m_properties;

	mutable sf::FloatRect m_bounds;
	mutable sf::Vector2f m_lastViewPos;
	std::vector<std::string> m_searchPaths;

	std::vector<MapLayer> m_layers;
	std::vector<sf::Texture> m_imageLayerTextures;
	std::vector<sf::Texture> m_tilesetTextures;

	std::vector<TileInfo> m_tileInfo;

	sf::VertexArray m_gridVertices;
	bool m_mapLoaded, m_quadTreeAvailable;
	QuadTreeRoot m_rootNode;

	std::map<std::string, std::shared_ptr<sf::Image>> m_cachedImages;
	bool m_failedImage;

};

static std::string base64_decode(std::string const& string);

#endif
