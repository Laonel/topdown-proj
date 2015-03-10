#include <Scene/Map/MapLoader.h>

#include <zlib.h>

MapLoader::MapLoader(const std::string& mapDirectory) :
	m_width(1u),
	m_height(1u),
	m_tileWidth(1u),
	m_tileHeight(1u),
	m_tileRatio(1.f),
	m_mapLoaded(false),
	m_quadTreeAvailable(false),
	m_failedImage(false)
{
	// reserve some space to help reduce reallocations
	m_layers.reserve(10);

	addSearchPath(mapDirectory);
}

bool MapLoader::load(const std::string& map)
{
	std::string mapPath = m_searchPaths[0] + _fileFromPath(map);
	_unload();

	// parse map xml, return on error
	pugi::xml_document mapDoc;
	pugi::xml_parse_result result = mapDoc.load_file(mapPath.c_str());
	if (!result)
	{
		PRINT_ERROR << "Failed to open " << map << std::endl;
		PRINT_ERROR << "Reason: " << result.description() << std::endl;
		return m_mapLoaded = false;
	}

	// set map properties
	pugi::xml_node mapNode = mapDoc.child("map");
	if (!mapNode)
	{
		PRINT_ERROR << "Map node not found. Map " << map << " not loaded." << std::endl;
		return m_mapLoaded = false;
	}

	if (!(m_mapLoaded = _parseMapNode(mapNode)))
		return false;
	if (!(m_mapLoaded = _parseTileSets(mapNode)))
		return false;

	// actually we need to traverse map node children and parse each layer as found
	pugi::xml_node currentNode = mapNode.first_child();
	while (currentNode)
	{
		std::string name = currentNode.name();
		if (name == "layer")
		{
			if (!(m_mapLoaded = _parseLayer(currentNode)))
			{
				_unload();
				return false;
			}
		}
		else if (name == "imageLayer")
		{
			if (!(m_mapLoaded = _parseImageLayer(currentNode)))
			{
				_unload();
				return false;
			}
		}
		else if (name == "objectgroup")
		{
			if (!(m_mapLoaded = _parseObjectGroup(currentNode)))
			{
				_unload();
				return false;
			}
		}

		currentNode = currentNode.next_sibling();
	}

	_createDebugGrid();

	PRINT_DEBUG << "Parsed " << m_layers.size() << " layers." << std::endl;
	PRINT_DEBUG << "Loaded " << map << " successfully." << std::endl;

	return m_mapLoaded = true;
}

void MapLoader::addSearchPath(const std::string& path)
{
	m_searchPaths.push_back(path);

	std::string& s = m_searchPaths.back();
	std::replace(s.begin(), s.end(), '\\', '/');

	if (s.size() > 1 && *s.rbegin() != '/')
		s += '/';
	else if (s == "/" || s == "\\")
		s = "";
}

void MapLoader::updateQuadTree(const sf::FloatRect& rootArea)
{
	m_rootNode.clear(rootArea);
	for (const auto& layer : m_layers)
		for (const auto& object : layer.objects)
			m_rootNode.insert(object);

	m_quadTreeAvailable = true;
}

std::vector<MapObject*> MapLoader::queryQuadTree(const sf::FloatRect& testArea)
{
	assert(m_quadTreeAvailable);
	return m_rootNode.retrieve(testArea);
}

std::vector<MapLayer>& MapLoader::getLayers()
{
	return m_layers;
}

const std::vector<MapLayer>& MapLoader::getLayers() const
{
	return m_layers;
}

void MapLoader::draw(sf::RenderTarget& target, MapLayer::DrawType type, bool debug)
{
	_setDrawingBounds(target.getView());
	switch (type)
	{
	default:
	case MapLayer::All:
		for (const auto& l : m_layers)
			target.draw(l);
		break;
	case MapLayer::Back:
		{
			MapLayer& layer = m_layers.front();
			_drawLayer(target, layer, debug);
		}
		break;
	case MapLayer::Debug:
		for (auto layer : m_layers)
		{
			if (layer.type == ObjectGroup)
			{
				for (const auto& object : layer.objects)
					if (m_bounds.intersects(object.getAABB()))
						object.drawDebugShape(target);
			}
		}
		target.draw(m_gridVertices);
		target.draw(m_rootNode);
		break;
	}
}

void MapLoader::draw(sf::RenderTarget& target, sf::Uint16 index, bool debug)
{
	_setDrawingBounds(target.getView());
	_drawLayer(target, m_layers[index], debug);
}

sf::Vector2f MapLoader::isometricToOrthogonal(const sf::Vector2f& projectedCoords)
{
	if (m_orientation != Isometric)
		return projectedCoords;

	return sf::Vector2f(projectedCoords.x - projectedCoords.y, 
						(projectedCoords.x / m_tileRatio) + (projectedCoords.y / m_tileRatio));
}

sf::Vector2f MapLoader::orthogonalToIsometric(const sf::Vector2f& worldCoords)
{
	if (m_orientation != Isometric)
		return worldCoords;

	return sf::Vector2f(((worldCoords.x / m_tileRatio) + worldCoords.y),
						(worldCoords.y - (worldCoords.x / m_tileRatio)));
}

sf::Vector2u MapLoader::getMapSize() const
{
	return sf::Vector2u(m_width * m_tileWidth, m_height * m_tileHeight);
}

std::string MapLoader::getPropertyString(const std::string& name)
{
	assert(m_properties.find(name) != m_properties.end());
	return m_properties[name];
}

void MapLoader::setLayerShader(sf::Uint16 layerId, const sf::Shader& shader)
{
	m_layers[layerId].setShader(shader);
}

bool MapLoader::quadTreeAvailable() const
{
	return m_quadTreeAvailable;
}

MapLoader::TileInfo::TileInfo() :
	tilesetId(0u)
{
}

MapLoader::TileInfo::TileInfo(const sf::IntRect& r, const sf::Vector2f& s, sf::Uint16 tsId) :
	size(s),
	tilesetId(tsId)
{
	coords[0] = sf::Vector2f(static_cast<float>(r.left), static_cast<float>(r.top));
	coords[1] = sf::Vector2f(static_cast<float>(r.left + r.width), static_cast<float>(r.top));
	coords[2] = sf::Vector2f(static_cast<float>(r.left + r.width), static_cast<float>(r.top + r.height));
	coords[3] = sf::Vector2f(static_cast<float>(r.left), static_cast<float>(r.top + r.height));
}

void MapLoader::_unload()
{
	m_tilesetTextures.clear();
	m_tileInfo.clear();
	m_layers.clear();
	m_imageLayerTextures.clear();
	m_mapLoaded = false;
	m_quadTreeAvailable = false;
	m_failedImage = false;
}

void MapLoader::_setDrawingBounds(const sf::View& view)
{
	if (view.getCenter() != m_lastViewPos)
	{
		sf::FloatRect bounds;
		bounds.left = view.getCenter().x - (view.getSize().x / 2.f);
		bounds.top = view.getCenter().y - (view.getSize().y / 2.f);
		bounds.width = view.getSize().x;
		bounds.height = view.getSize().y;

		// add a tile border to prevent gaps appearing
		bounds.left -= static_cast<float>(m_tileWidth);
		bounds.top -= static_cast<float>(m_tileHeight);
		bounds.width += static_cast<float>(m_tileWidth * 2);
		bounds.height += static_cast<float>(m_tileHeight * 2);
		m_bounds = bounds;
	}

	m_lastViewPos = view.getCenter();
}

bool MapLoader::_parseMapNode(const pugi::xml_node& mapNode)
{
	if (!(m_width = mapNode.attribute("width").as_int()) ||
		!(m_height = mapNode.attribute("height").as_int()) ||
		!(m_tileWidth = mapNode.attribute("tilewidth").as_int()) ||
		!(m_tileHeight = mapNode.attribute("tileheight").as_int()))
	{
		PRINT_ERROR << "Invalid tile size found, check map data. Map not loaded." << std::endl;
		return false;
	}

	// parse orientation property
	std::string orientation = mapNode.attribute("orientation").as_string();

	if (orientation == "orthogonal")
	{
		m_orientation = Orthogonal;
	}
	else if (orientation == "isometric")
	{
		m_orientation = Isometric;
		m_tileRatio = static_cast<float>(m_tileWidth) / static_cast<float>(m_tileHeight);
	}
	else
	{
		PRINT_ERROR << "Map orientation " << orientation << " not currently supported. Map not loaded." << std::endl;
		return false;
	}

	// parse any map properties
	if (pugi::xml_node propertiesNode = mapNode.child("properties"))
	{
		pugi::xml_node propertyNode = propertiesNode.child("property");
		while (propertyNode)
		{
			std::string name = propertyNode.attribute("name").as_string();
			std::string value = propertyNode.attribute("value").as_string();
			m_properties[name] = value;
			propertyNode = propertyNode.next_sibling("property");
			PRINT_ERROR << "Added map property " << name << " with value " << value << std::endl;
		}
	}

	return true;
}

bool MapLoader::_parseTileSets(const pugi::xml_node& mapNode)
{
	pugi::xml_node tileset;
	if (!(tileset = mapNode.child("tileset")))
	{
		PRINT_ERROR << "No tile sets found." << std::endl;
		return false;
	}

	PRINT_DEBUG << "Caching image files." << std::endl;

	// empty vertex tile
	m_tileInfo.push_back(TileInfo());

	// parse tile sets in order so GUIDs match index
	while (tileset)
	{
		// if source attribute parse external tsx
		if (tileset.attribute("source"))
		{
			// try loading tsx
			std::string file = _fileFromPath(tileset.attribute("source").as_string());
			std::string path;
			pugi::xml_document tsxDoc;
			pugi::xml_parse_result result;

			for (auto& p : m_searchPaths)
			{
				path = p + file;
				result = tsxDoc.load_file(path.c_str());
				if (result)
					break;
			}

			if (!result)
			{
				PRINT_ERROR << "Failed to open external tsx document: " << path << std::endl;
				PRINT_ERROR << "Reason: " << result.description() << std::endl;
				PRINT_ERROR << "Make sure to add any external paths with addSearchPath()" << std::endl;
				_unload();
				return false;
			}

			// try parsing tileset node
			pugi::xml_node ts = tsxDoc.child("tileset");

			if (!_processTiles(ts))
				return false;
		}
		else // try for tmx map file data
		{
			if (!_processTiles(tileset))
				return false;
		}

		// move on to next tileset node
		tileset = tileset.next_sibling("tileset");
	}

	return true;
}

bool MapLoader::_processTiles(const pugi::xml_node& tilesetNode)
{
	sf::Uint16 tileWidth, tileHeight, spacing, margin;

	// try and parse tile sizes
	if (!(tileWidth = tilesetNode.attribute("tilewidth").as_int()) || 
		!(tileHeight = tilesetNode.attribute("tileheight").as_int()))
	{
		PRINT_ERROR << "Invalid tileset data found. Map not loaded." << std::endl;
		_unload();
		return false;
	}

	spacing = (tilesetNode.attribute("spacing")) ? tilesetNode.attribute("spacing").as_int() : 0u;
	margin = (tilesetNode.attribute("margin")) ? tilesetNode.attribute("margin").as_int() : 0u;

	// try parsing image node
	pugi::xml_node imageNode;
	if (!(imageNode = tilesetNode.child("image")) || !imageNode.attribute("source"))
	{
		PRINT_ERROR << "Missing image data in tmx file. Map not loaded." << std::endl;
		_unload();
		return false;
	}

	// process image from disk
	std::string imageName = _fileFromPath(imageNode.attribute("source").as_string());
	sf::Image sourceImage = _loadImage(imageName);
	if (m_failedImage)
	{
		PRINT_ERROR << "Failed to load image " << imageName << std::endl;
		PRINT_ERROR << "Please check image exists and add any external paths with addSearchPath()" << std::endl;
		return false;
	}

	// add transparency mask from color if it exists
	if (imageNode.attribute("trans"))
		sourceImage.createMaskFromColor(_colorFromHex(imageNode.attribute("trans").as_string()));

	// store image as a texture for drawing with vertex array
	sf::Texture tileset;
	tileset.loadFromImage(sourceImage);
	m_tilesetTextures.push_back(tileset);

	// parse offset node if it exists - TODO store somewhere tileset info can be referenced
	sf::Vector2u offset;
	if (pugi::xml_node offsetNode = tilesetNode.child("tileoffset"))
	{
		offset.x = (offsetNode.attribute("x")) ? offsetNode.attribute("x").as_uint() : 0u;
		offset.y = (offsetNode.attribute("y")) ? offsetNode.attribute("y").as_uint() : 0u;
	}

	// TODO parse any tile properties and store with offset above

	// slice into tiles
	int columns = (sourceImage.getSize().x - margin) / (tileWidth + spacing);
	int rows = (sourceImage.getSize().y - margin) / (tileHeight + spacing);

	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < columns; ++x)
		{
			sf::IntRect rect;
			rect.top = y * (tileHeight + spacing);
			rect.top += margin;
			rect.height = tileHeight;
			rect.left = x * (tileWidth + spacing);
			rect.left += margin;
			rect.width = tileWidth;

			// store texture coords and tileset index for vertex array
			m_tileInfo.push_back(TileInfo(rect,
								 sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height)),
								 m_tilesetTextures.size() - 1u));
		}
	}

	PRINT_ERROR << "Processed " << imageName << std::endl;
	return true;
}

bool MapLoader::_parseLayer(const pugi::xml_node& layerNode)
{
	PRINT_DEBUG << "Found standard map layer " << layerNode.attribute("name").as_string() << std::endl;

	MapLayer layer(Layer);
	if (layerNode.attribute("name"))
		layer.name = layerNode.attribute("name").as_string();
	if (layerNode.attribute("opacity"))
		layer.opacity = layerNode.attribute("opacity").as_float();
	if (layerNode.attribute("visible"))
		layer.visible = layerNode.attribute("visible").as_bool();

	pugi::xml_node dataNode;
	if (!(dataNode = layerNode.child("data")))
	{
		PRINT_ERROR << "Layer data missing or corrupt. Map not loaded." << std::endl;
		return false;
	}

	// decode and decompress data first if necessary
	// see https://github.com/bjorn/tiled/wiki/TMX-Map-Format#data
	// for explanation of bytestream retrieved when using compression
	if (dataNode.attribute("encoding"))
	{
		std::string encoding = dataNode.attribute("encoding").as_string();
		std::string data = dataNode.text().as_string();

		if (encoding == "base64")
		{
			PRINT_DEBUG << "Found Base64 encoded layer data, decoding..." << std::endl;
			// remove any newlines or white space created by tab spaces in document
			std::stringstream ss;
			ss << data;
			ss >> data;
			data = base64_decode(data);

			// calculate the expected size of the uncompressed string
			int expectedSize = m_width * m_height * 4;
			std::vector<unsigned char> byteArray;
			byteArray.reserve(expectedSize);

			// check for compression (only used with base64 encoded data)
			if (dataNode.attribute("compression"))
			{
				std::string compression = dataNode.attribute("compression").as_string();
				PRINT_DEBUG << "Found " << compression << " compressd layer data, decompressing..." << std::endl;

				// decompress with zlib
				int dataSize = data.length() * sizeof(unsigned char);
				if (!_decompress(data.c_str(), byteArray, dataSize, expectedSize))
				{
					PRINT_ERROR << "Failed to decompress map data. Map not loaded." << std::endl;
					return false;
				}
			}
			else // uncompressed
			{
				byteArray.insert(byteArray.end(), data.begin(), data.end());
			}

			// extract tile GIDs using bitshift
			sf::Uint16 x, y;
			x = y = 0;
			for (int i = 0; i < expectedSize - 3; i += 4)
			{
				sf::Uint32 tileGID = byteArray[i] | byteArray[i+1] << 8 | byteArray[i+2] << 16 | byteArray[i+3] << 24;

				_addTileToLayer(layer, x, y, tileGID);

				++x;
				if (x == m_width)
				{
					x = 0;
					y++;
				}
			}
		}
		else if (encoding == "csv")
		{
			PRINT_DEBUG << "CSV encoded layer data found." << std::endl;

			std::vector<sf::Uint32> tileGIDs;
			std::stringstream datastream(data);

			sf::Uint32 i;
			while (datastream >> i)
			{
				tileGIDs.push_back(i);
				if (datastream.peek() == ',')
					datastream.ignore();
			}

			// create tiles from IDs
			sf::Uint16 x, y;
			x = y = 0;
			for (unsigned int i = 0; i < tileGIDs.size(); ++i)
			{
				_addTileToLayer(layer, x, y, tileGIDs[i]);
				++x;
				if (x == m_width)
				{
					x = 0;
					++y;
				}
			}
		}
		else
		{
			PRINT_ERROR << "Unsupported encoding of layer data found. Map not loaded." << std::endl;
			return false;
		}
	}
	else // unencoded
	{
		PRINT_DEBUG << "Found unencoded data." << std::endl;
		pugi::xml_node tileNode;
		if (!(tileNode = dataNode.child("tile")))
		{
			PRINT_ERROR << "No tile data found. Map not loaded." << std::endl;
			return false;
		}

		sf::Uint16 x, y;
		x = y = 0;
		while (tileNode)
		{
			sf::Uint32 gid = tileNode.attribute("gid").as_uint();

			_addTileToLayer(layer, x, y, gid);

			tileNode = tileNode.next_sibling("tile");
			x++;
			if (x == m_width)
			{
				x = 0;
				++y;
			}
		}
	}

	// parse any layer properties
	if (pugi::xml_node propertiesNode = layerNode.child("properties"))
		_parseLayerProperties(propertiesNode, layer);

	// convert layer tile coords to isometric if needed
	if (m_orientation == Isometric)
		_setIsometricCoords(layer);

	m_layers.push_back(layer);
	return true;
}

bool MapLoader::_parseObjectGroup(const pugi::xml_node& groupNode)
{
	PRINT_ERROR << "Found object layer " << groupNode.attribute("name").as_string() << std::endl;

	pugi::xml_node objectNode;
	if (!(objectNode = groupNode.child("object")))
	{
		PRINT_ERROR << "Object group contains no objects" << std::endl;
		return true;
	}

	// add layer to map layers
	MapLayer layer(ObjectGroup);

	layer.name = groupNode.attribute("name").as_string();
	if (groupNode.attribute("opacity"))
		layer.opacity = groupNode.attribute("opacity").as_float();
	if (pugi::xml_node propertiesNode = groupNode.child("properties"))
		_parseLayerProperties(propertiesNode, layer);

	// parse all object nodes into MapObjects
	while (objectNode)
	{
		if (!objectNode.attribute("x") || !objectNode.attribute("y"))
		{
			PRINT_ERROR << "Object missing position data. Map not loaded." << std::endl;
			_unload();
			return false;
		}

		MapObject object;

		// set position
		sf::Vector2f position(objectNode.attribute("x").as_float(),
							  objectNode.attribute("y").as_float());
		position = isometricToOrthogonal(position);
		object.setPosition(position);

		// set size if specified
		if (objectNode.attribute("width") && objectNode.attribute("height"))
		{
			sf::Vector2f size(objectNode.attribute("width").as_float(),
							  objectNode.attribute("height").as_float());

			if (objectNode.child("ellipse"))
			{
				// add points to make ellipse
				const float x = size.x / 2.f;
				const float y = size.y / 2.f;
				const float tau = 6.283185f;
				const float step = tau / 16.f; // number of points to make up ellipse

				for (float angle = 0.f; angle < tau; angle += step)
				{
					sf::Vector2f point(x + x * cos(angle), y + y * sin(angle));
					object.addPoint(isometricToOrthogonal(point));
				}

				if (size.x == size.y)
					object.setShapeType(Circle);
				else
					object.setShapeType(Ellipse);
			}
			else // add points for rectangle to use in intersection testing
			{
				object.addPoint(isometricToOrthogonal(sf::Vector2f()));
				object.addPoint(isometricToOrthogonal(sf::Vector2f(size.x, 0.f)));
				object.addPoint(isometricToOrthogonal(sf::Vector2f(size.x, size.y)));
				object.addPoint(isometricToOrthogonal(sf::Vector2f(0.f, size.y)));
			}

			object.setSize(size);
		}
		else if (objectNode.child("polygon") || objectNode.child("polyline"))
		{
			if (objectNode.child("polygon"))
				object.setShapeType(Polygon);
			else
				object.setShapeType(Polyline);

			// split coords into pairs
			if (objectNode.first_child().attribute("points"))
			{
				PRINT_DEBUG << "Processing poly shape points..." << std::endl;
				std::string pointlist = objectNode.first_child().attribute("points").as_string();
				std::stringstream stream(pointlist);
				std::vector<std::string> points;
				std::string pointstring;

				while (std::getline(stream, pointstring, ' '))
					points.push_back(pointstring);

				// parse each pair into sf::Vector2i
				for (unsigned int i = 0; i < points.size(); ++i)
				{
					std::vector<float> coords;
					std::stringstream coordstream(points[i]);

					float j;
					while (coordstream >> j)
					{
						coords.push_back(j);
						if (coordstream.peek() == ',')
							coordstream.ignore();
					}

					object.addPoint(isometricToOrthogonal(sf::Vector2f(coords[0], coords[1])));
				}
			}
			else
			{
				PRINT_ERROR << "Points for polygon or polyline are missing" << std::endl;
			}
		}
		else if (!objectNode.attribute("gid")) // invalid attributes
		{
			PRINT_ERROR << "Objects with no parameters found, skipping..." << std::endl;
			objectNode = objectNode.next_sibling("object");
			continue;
		}

		// parse object node property values
		if (pugi::xml_node propertiesNode = propertiesNode.child("property"))
		{
			pugi::xml_node propertyNode = propertiesNode.child("property");
			while (propertyNode)
			{
				std::string name = propertyNode.attribute("name").as_string();
				std::string value = propertyNode.attribute("value").as_string();
				object.setProperty(name, value);

				PRINT_DEBUG << "Set object property " << name << " with value " << value << std::endl;
				propertyNode = propertyNode.next_sibling("property");
			}
		}

		// set object properties
		if (objectNode.attribute("name"))
			object.setName(objectNode.attribute("name").as_string());
		if (objectNode.attribute("type"))
			object.setType(objectNode.attribute("type").as_string());
		if (objectNode.attribute("visible"))
			object.setVisible(objectNode.attribute("visible").as_bool());
		if (objectNode.attribute("gid"))
		{
			sf::Uint32 gid = objectNode.attribute("gid").as_int();

			PRINT_DEBUG << "Found object with tile GID " << gid << std::endl;

			object.move(0.f, static_cast<float>(-m_tileHeight));
			const sf::Uint16 x = static_cast<sf::Uint16>(object.getPosition().x / m_tileWidth);
			const sf::Uint16 y = static_cast<sf::Uint16>(object.getPosition().y / m_tileHeight);

			sf::Vector2f offset(object.getPosition().x - (x * m_tileWidth), (object.getPosition().y - (y * m_tileHeight)));
			object.setQuad(_addTileToLayer(layer, x, y, gid, offset));
			object.setShapeType(Tile);

			TileInfo info = m_tileInfo[gid];
			// create bounding poly
			float width = static_cast<float>(info.size.x);
			float height = static_cast<float>(info.size.y);

			object.addPoint(sf::Vector2f());
			object.addPoint(sf::Vector2f(width, 0.f));
			object.addPoint(sf::Vector2f(width, height));
			object.addPoint(sf::Vector2f(0.f, height));
			object.setSize(sf::Vector2f(width, height));

			if (info.size.y != m_tileHeight)
				object.move(0.f, static_cast<float>(m_tileHeight - info.size.y) / 2.f);
		}

		object.setParent(layer.name);

		sf::Color debugColor;
		if (groupNode.attribute("color"))
		{
			std::string color = groupNode.attribute("color").as_string();
			// ctop leading hash and pop the last char
			std::remove(color.begin(), color.end(), '#');
			color.pop_back();
			debugColor = _colorFromHex(color.c_str());
		}
		else
			debugColor = sf::Color(127u, 127u, 127u);

		debugColor.a = static_cast<sf::Uint8>(255.f * layer.opacity);
		object.createDebugShape(debugColor);

		// creates line segments from any available points
		object.createSegments();

		// add objects to vector
		layer.objects.push_back(object);
		objectNode = objectNode.next_sibling("object");
	}

	m_layers.push_back(layer);
	PRINT_DEBUG << "Processed " << layer.objects.size() << " objects" << std::endl;
	return true;
}

bool MapLoader::_parseImageLayer(const pugi::xml_node& imageLayerNode)
{
	PRINT_DEBUG << "Found image layer " << imageLayerNode.attribute("name").as_string() << std::endl;

	pugi::xml_node imageNode;
	// load image
	if (!(imageNode = imageLayerNode.child("image")) || !imageNode.attribute("source"))
	{
		PRINT_ERROR << "Image layer " << imageLayerNode.attribute("name").as_string() << " missing image source property. Map not loaded." << std::endl;
		return false;
	}

	std::string imageName = imageNode.attribute("source").as_string();
	sf::Image image = _loadImage(imageName);
	if (m_failedImage)
	{
		PRINT_ERROR << "Failed to load image at " << imageName << std::endl;
		PRINT_ERROR << "Please check image exists and add any external paths with addSearchPath()" << std::endl;
		return false;
	}

	// set transparency if required
	if (imageNode.attribute("trans"))
		image.createMaskFromColor(_colorFromHex(imageNode.attribute("trans").as_string()));

	// load image to texture
	sf::Texture texture;
	texture.loadFromImage(image);
	m_imageLayerTextures.push_back(texture);

	// add texture to layer as sprite, set layer properties
	MapTile tile;
	tile.sprite.setTexture(m_imageLayerTextures.back());

	MapLayer layer(ImageLayer);
	layer.name = imageLayerNode.attribute("name").as_string();

	if (imageLayerNode.attribute("opacity"))
	{
		layer.opacity = imageLayerNode.attribute("opacity").as_float();
		sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * layer.opacity);
		tile.sprite.setColor(sf::Color(255u, 255u, 255u, opacity));
	}

	layer.tiles.push_back(tile);

	// parse layer properties
	if (pugi::xml_node propertiesNode = imageLayerNode.child("properties"))
		_parseLayerProperties(propertiesNode, layer);

	// push back layer
	m_layers.push_back(layer);

	return true;
}

bool MapLoader::_parseLayerProperties(const pugi::xml_node& propertiesNode, MapLayer& layer)
{
	pugi::xml_node propertyNode = propertiesNode.child("property");
	while (propertyNode)
	{
		std::string name = propertyNode.attribute("name").as_string();
		std::string value = propertyNode.attribute("value").as_string();
		
		layer.properties[name] = value;
		propertyNode = propertyNode.next_sibling("property");

		PRINT_DEBUG << "Added layer property " << name << " with value " << value << std::endl;
	}
}

TileQuad::Ptr MapLoader::_addTileToLayer(MapLayer& layer, sf::Uint16 x, sf::Uint16 y, sf::Uint32 gid, const sf::Vector2f& offset)
{
	sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * layer.opacity);
	sf::Color color = sf::Color(255u, 255u, 255u, opacity);

	// get bits and tile id
	std::pair<sf::Uint32, std::bitset<3>> idAndFlags = _resolveRotation(gid);
	gid = idAndFlags.first;

	// update the layer's tile set(s)
	sf::Vertex v0, v1, v2, v3;

	// applying half pixel trick avoids artifacting when scrolling
	v0.texCoords = m_tileInfo[gid].coords[0] + sf::Vector2f(0.5f, 0.5f);
	v1.texCoords = m_tileInfo[gid].coords[1] + sf::Vector2f(-0.5f, 0.5f);
	v2.texCoords = m_tileInfo[gid].coords[2] + sf::Vector2f(-0.5f, -0.5f);
	v3.texCoords = m_tileInfo[gid].coords[3] + sf::Vector2f(0.5f, -0.5f);

	// flip texture coordinates according to bits set
	_doFlips(idAndFlags.second, &v0.texCoords, &v1.texCoords, &v2.texCoords, &v3.texCoords);

	v0.position = sf::Vector2f(static_cast<float>(m_tileWidth * x), static_cast<float>(m_tileHeight * y));
	v1.position = sf::Vector2f(static_cast<float>(m_tileWidth * x) + m_tileInfo[gid].size.x, static_cast<float>(m_tileHeight * y));
	v2.position = sf::Vector2f(static_cast<float>(m_tileWidth * x) + m_tileInfo[gid].size.x, static_cast<float>(m_tileHeight * y) + m_tileInfo[gid].size.y);
	v3.position = sf::Vector2f(static_cast<float>(m_tileWidth * x), static_cast<float>(m_tileHeight * y) + m_tileInfo[gid].size.y);

	// offset tiles with size not equal to map grid size
	sf::Uint16 tileHeight = static_cast<sf::Uint16>(m_tileInfo[gid].size.y);
	if (tileHeight != m_tileHeight)
	{
		float diff = static_cast<float>(m_tileHeight - tileHeight);
		v0.position.y += diff;
		v1.position.y += diff;
		v2.position.y += diff;
		v3.position.y += diff;	
	}

	// adjust position for isometric maps
	if (m_orientation == Isometric)
	{
		sf::Vector2f isoOffset(-static_cast<float>(x * (m_tileWidth / 2u)), static_cast<float>(x * (m_tileHeight / 2u)));
		isoOffset.x -= static_cast<float>(y * (m_tileWidth / 2u));
		isoOffset.y -= static_cast<float>(y * (m_tileHeight / 2u));
		isoOffset.x -= static_cast<float>(m_tileWidth / 2u);
		isoOffset.y += static_cast<float>(m_tileHeight / 2u);

		v0.position += isoOffset;
		v1.position += isoOffset;
		v2.position += isoOffset;
		v3.position += isoOffset;
	}

	v0.color = color;
	v1.color = color;
	v2.color = color;
	v3.color = color;

	v0.position += offset;
	v1.position += offset;
	v2.position += offset;
	v3.position += offset;

	sf::Uint16 id = m_tileInfo[gid].tilesetId;
	if (layer.layerSets.find(id) == layer.layerSets.end())
	{
		// create a new layerset for texture
		layer.layerSets[id] = std::make_shared<LayerSet>(m_tilesetTextures[id]);
	}

	// add tile to set
	return layer.layerSets[id]->addTile(v0, v1, v2, v3);
}

void MapLoader::_setIsometricCoords(MapLayer& layer)
{
	for (auto tile = layer.tiles.begin(); tile != layer.tiles.end(); ++tile)
	{
		sf::Int16 posX = (tile->gridCoord.x - tile->gridCoord.y) * (m_tileWidth / 2);
		sf::Int16 posY = (tile->gridCoord.y + tile->gridCoord.x) * (m_tileHeight / 2);
		tile->sprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
	}
}

void MapLoader::_drawLayer(sf::RenderTarget& target, MapLayer& layer, bool debug)
{
	layer.cull(m_bounds);
	target.draw(layer);

	if (debug && layer.type == ObjectGroup)
	{
		for (const auto& object : layer.objects)
			if (m_bounds.intersects(object.getAABB()))
				object.drawDebugShape(target);
	}
}

std::string MapLoader::_fileFromPath(const std::string& path)
{
	assert(!path.empty());

	for (auto it = path.rbegin(); it != path.rend(); ++it)
	{
		if (*it == '/' || *it == '\\')
		{
			int pos = std::distance(path.rbegin(), it);
			return path.substr(path.size() - pos);
		}
	}

	return path;
}

void MapLoader::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::View view = target.getView();
	if (view.getCenter() != m_lastViewPos)
	{
		sf::FloatRect bounds;
		bounds.left = view.getCenter().x - (view.getSize().x / 2.f);
		bounds.top = view.getCenter().y - (view.getSize().y / 2.f);
		bounds.width = view.getSize().x;
		bounds.height = view.getSize().y;

		// add a tile border to prevent gaps appearing
		bounds.left -= static_cast<float>(m_tileWidth);
		bounds.top -= static_cast<float>(m_tileHeight);
		bounds.width += static_cast<float>(m_tileWidth * 2);
		bounds.height += static_cast<float>(m_tileHeight * 2);
		m_bounds = bounds;
	}

	m_lastViewPos = view.getCenter();

	for (auto& layer : m_layers)
		target.draw(layer);
}

sf::Color MapLoader::_colorFromHex(const char* hexStr) const
{
	// TODO: proper checking valid string length
	unsigned int value, r, g, b;
	std::stringstream input(hexStr);
	input >> std::hex >> value;

	r = (value >> 16) & 0xff;
	g = (value >> 8) & 0xff;
	b = value & 0xff;

	return sf::Color(r, g, b);
}

bool MapLoader::_decompress(const char* source, std::vector<unsigned char>& dest, int inSize, int expectedSize)
{
	if (!source)
	{
		PRINT_ERROR << "Input string is empty, decompression failed." << std::endl;
		return false;
	}

	int currentSize = expectedSize;
	// TODO: switch to std::make_unique when compatible with all complilers
	std::unique_ptr<unsigned char[]> byteArray(new unsigned char[expectedSize / sizeof(unsigned char)]);
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.next_in = (Bytef*)source;
	stream.avail_in = inSize;
	stream.next_out = (Bytef*)byteArray.get();
	stream.avail_out = expectedSize;

	if (inflateInit2(&stream, 15 + 32) != Z_OK)
	{
		PRINT_ERROR << "inflate 2 failed" << std::endl;
		return false;
	}

	int result = 0;
	do
	{
		result = inflate(&stream, Z_SYNC_FLUSH);

		switch (result)
		{
			case Z_NEED_DICT:
			case Z_STREAM_ERROR:
				result = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&stream);
				PRINT_ERROR << result << std::endl;
				return false;
		}

		if (result != Z_STREAM_END)
		{
			int oldSize = currentSize;
			currentSize *= 2;
			std::unique_ptr<unsigned char[]> newArray(new unsigned char[currentSize / sizeof(unsigned char)]);
			std::memcpy(newArray.get(), byteArray.get(), currentSize / 2);
			byteArray = std::move(newArray);

			stream.next_out = (Bytef*)(byteArray.get() + oldSize);
			stream.avail_out = oldSize;
		}
	} while (result != Z_STREAM_END);

	if (stream.avail_in != 0)
	{
		PRINT_ERROR << "stream.avail_in is 0" << std::endl;
		PRINT_ERROR << "zlib compression failed." << std::endl;
		return false;
	}

	const int outSize = currentSize - stream.avail_out;
	inflateEnd(&stream);

	std::unique_ptr<unsigned char[]> newArray(new unsigned char[outSize / sizeof(unsigned char)]);
	std::memcpy(newArray.get(), byteArray.get(), outSize);
	byteArray = std::move(newArray);

	// copy bytes to vector
	int length = currentSize / sizeof(unsigned char);
	dest.insert(dest.begin(), &byteArray[0], &byteArray[length]);

	return true;
}

void MapLoader::_createDebugGrid()
{
	sf::Color debugColor(0u, 0u, 0u, 120u);

	float mapHeight = static_cast<float>(m_tileHeight * m_height);
	for (int x = 0; x <= m_width; x += 2)
	{
		float posX = static_cast<float>(x * (m_tileWidth / m_tileRatio));
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(posX, 0.f)), debugColor));
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(posX, mapHeight)), debugColor));

		posX += static_cast<float>(m_tileWidth) / m_tileRatio;
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(posX, mapHeight)), debugColor));
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(posX, 0.f)), debugColor));

		posX += static_cast<float>(m_tileWidth) / m_tileRatio;
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(posX, 0.f)), debugColor));
	}

	float mapWidth = static_cast<float>(m_tileWidth * (m_width / m_tileRatio));
	for (int y = 0; y <= m_height; y += 2)
	{
		float posY = static_cast<float>(y * m_tileHeight);
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(0.f, posY)), debugColor));

		posY += static_cast<float>(m_tileHeight);
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(0.f, posY)), debugColor));
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(mapWidth, posY)), debugColor));

		posY += static_cast<float>(m_tileHeight);
		m_gridVertices.append(sf::Vertex(isometricToOrthogonal(sf::Vector2f(mapWidth, posY)), debugColor));
	}

	m_gridVertices.setPrimitiveType(sf::LinesStrip);
}

sf::Image& MapLoader::_loadImage(const std::string& imageName)
{
	for (const auto& p : m_searchPaths)
	{
		const auto i = m_cachedImages.find(p + imageName);
		if (i != m_cachedImages.cend())
			return *i->second;
	}

	// else attempt to load
	std::shared_ptr<sf::Image> newImage = std::make_shared<sf::Image>();

	bool loaded = false;
	std::string path;
	for (const auto& p : m_searchPaths)
	{
		path = p + imageName;
		loaded = newImage->loadFromFile(path);
		if (loaded)
			break;
	}

	if (!loaded)
	{
		newImage->create(20u, 20u, sf::Color::Yellow);
		m_failedImage = true;
		path = "default";
	}

	m_cachedImages[path] = newImage;
	return *m_cachedImages[path];
}

std::vector<unsigned char> MapLoader::_intToBytes(sf::Uint32 paramInt)
{
	std::vector<unsigned char> arrayOfByte(4);
	for (int i = 0; i < 4; i++)
		arrayOfByte[i] = (paramInt >> (i * 8));

	return arrayOfByte;
}

std::pair<sf::Uint32, std::bitset<3>> MapLoader::_resolveRotation(sf::Uint32 gid)
{
	const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
	const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
	const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

	std::vector<unsigned char> bytes = _intToBytes(gid);
	sf::Uint32 tileGID = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;

	bool flippedDiagonally = (tileGID & FLIPPED_DIAGONALLY_FLAG);
	bool flippedHorizontally = (tileGID & FLIPPED_HORIZONTALLY_FLAG);
	bool flippedVertically = (tileGID & FLIPPED_VERTICALLY_FLAG);

	std::bitset<3> b;
	b.set(0, flippedVertically);
	b.set(1, flippedHorizontally);
	b.set(2, flippedDiagonally);

	tileGID &= ~(FLIPPED_HORIZONTALLY_FLAG |
				 FLIPPED_VERTICALLY_FLAG   |
				 FLIPPED_DIAGONALLY_FLAG);

	return std::pair<sf::Uint32, std::bitset<3>>(tileGID, b);
}

void MapLoader::_flipY(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
	// flip Y
	sf::Vector2f tmp = *v0;
	v0->y = v2->y;
	v1->y = v2->y;
	v2->y = tmp.y;
	v3->y = v2->y;
}

void MapLoader::_flipX(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
	// flip X
	sf::Vector2f tmp = *v0;
	v0->x = v1->x;
	v1->x = tmp.x;
	v2->x = v3->x;
	v3->x = v0->x;
}

void MapLoader::_flipD(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
	// flip D
	sf::Vector2f tmp = *v1;
	v1->x = v3->x;
	v1->y = v3->y;
	v3->x = tmp.x;
	v3->y = tmp.y;
}

void MapLoader::_doFlips(std::bitset<3> bits, sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3)
{
	// 000 = no change
    // 001 = vertical = swap y axis
    // 010 = horizontal = swap x axis
    // 011 = horiz + vert = swap both axes = horiz+vert = rotate 180 degrees
    // 100 = diag = rotate 90 degrees right and swap x axis
    // 101 = diag+vert = rotate 270 degrees right
    // 110 = horiz+diag = rotate 90 degrees right
    // 111 = horiz+vert+diag = rotate 90 degrees right and swap y axis

    if (!bits.test(0) && !bits.test(1) && !bits.test(2))
    {
        // Shortcircuit tests for nothing to do
        return;
    }
    else if (bits.test(0) && !bits.test(1) && !bits.test(2))
    {
        // 001
        _flipY(v0,v1,v2,v3);
    }
    else if (!bits.test(0) && bits.test(1) && !bits.test(2))
    {
        // 010
        _flipX(v0,v1,v2,v3);
    }
    else if (bits.test(0) && bits.test(1) && !bits.test(2))
    {
        // 011
        _flipY(v0,v1,v2,v3);
        _flipX(v0,v1,v2,v3);
    }
    else if (!bits.test(0) && !bits.test(1) && bits.test(2))
    {
        // 100
        _flipD(v0,v1,v2,v3);
    }
    else if (bits.test(0) && !bits.test(1) && bits.test(2))
    {
        // 101
        _flipX(v0,v1,v2,v3);
        _flipD(v0,v1,v2,v3);


    }
    else if (!bits.test(0) && bits.test(1) && bits.test(2))
    {
        // 110
        _flipY(v0,v1,v2,v3);
        _flipD(v0,v1,v2,v3);

    }
    else if (bits.test(0) && bits.test(1) && bits.test(2))
    {
        // 111
        _flipY(v0,v1,v2,v3);
        _flipX(v0,v1,v2,v3);
        _flipD(v0,v1,v2,v3);
    }
}

static const std::string base64_chars =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";


static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

static std::string base64_decode(std::string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4)
		{
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			ret += char_array_3[j];
	}

	return ret;
}
