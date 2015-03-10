#include <Scene/Map/MapObject.h>
#include <Scene/Map/MapLayer.h>

bool MapObject::Segment::intersects(const MapObject::Segment& segment)
{
	sf::Vector2f s1 = end - start;
	sf::Vector2f s2 = segment.end - segment.start;

	float s, t;
	s = (-s1.y * (start.x - segment.start.x) + s1.x * (start.y - segment.start.y)) / (-s2.x * s1.y + s1.x * s2.y);
	t = ( s1.x * (start.y - segment.start.y) - s2.y * (start.x - segment.start.x)) / (-s2.x * s1.y + s1.x * s2.y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// collision detected

		// point at which lines intersect
		sf::Vector2f intersection(start.x + (t * s1.x), start.y + (t * s1.y));
		return true;
	}

	return false;
}

MapObject::MapObject() :
	m_visible(true),
	m_shape(Rectangle),
	m_furthestPoint(0.f)
{
}

std::string MapObject::getPropertyString(const std::string& name)
{
	if (m_properties.find(name) != m_properties.end())
		return m_properties[name];
	else
		return std::string();
}

void MapObject::setProperty(const std::string& name, const std::string& value)
{
	m_properties[name] = value;
}

void MapObject::setPosition(float x, float y)
{
	setPosition(sf::Vector2f(x, y));
}

void MapObject::setPosition(const sf::Vector2f& position)
{
	sf::Vector2f distance = position - m_position;
	move(distance);
}

void MapObject::move(float x, float y)
{
	move(sf::Vector2f(x, y));
}

void MapObject::move(const sf::Vector2f& distance)
{
	// update properties by movement amount
	for (auto& p : m_polypoints)
		p += distance;

	m_debugShape.move(distance);

	m_AABB.left += distance.x;
	m_AABB.top += distance.y;

	// set new position
	m_position += distance;

	// if object is of type tile move vertex data
	if (m_tileQuad)
		m_tileQuad->move(distance);
}

bool MapObject::contains(sf::Vector2f point) const
{
	if (m_shape == Polyline)
		return false;

	// convert point to local coords
	point -= m_position;

	// TODO: transform point instead


	// check if enough poly points
	if (m_polypoints.size() < 3)
		return false;

	// else raycast through points
	bool result = false;
	unsigned int i, j;
	for (i = 0, j = m_polypoints.size() - 1; i < m_polypoints.size(); j = i++)
	{
		if (((m_polypoints[i].y > point.y) != (m_polypoints[j].y > point.y)) &&
			(point.x < (m_polypoints[j].x - m_polypoints[i].x) * (point.y - m_polypoints[i].y)
				/ (m_polypoints[j].y - m_polypoints[i].y) + m_polypoints[i].x))
			result = !result;
	}

	return result;
}

bool MapObject::intersects(const MapObject& object) const
{	
	// check if distance between objects is less than sum of furthest points
	float distance = Vector2(m_centrePoint + object.m_centrePoint).length();
	if (distance > (m_furthestPoint + object.m_furthestPoint))
		return false;

	// check intersection if either object contains a point of the other
	for (auto& p : object.m_polypoints)
		if (contains(p + object.getPosition()))
			return true;

	for (auto& p : m_polypoints)
		if (object.contains(p + getPosition()))
			return true;

	return false;
}

void MapObject::createDebugShape(const sf::Color& color)
{
	if (m_polypoints.size() == 0)
	{
		PRINT_ERROR << "Unable to create debug shape, object data missing." << std::endl;
		PRINT_ERROR << "Check image file paths referenced by tmx file." << std::endl;
		return;
	}

	// reset any existing shapes incase new points have been added
	m_debugShape.reset();

	for (const auto& p : m_polypoints)
		m_debugShape.addVertex(sf::Vertex(p, color));

	if (m_shape != Polyline)
		m_debugShape.closeShape();

	m_debugShape.setPosition(m_position);

	// precompute shape values for intersection testing
	_calcTestValues();

	// create the AABB for quad tree testing
	_createAABB();
}

void MapObject::drawDebugShape(sf::RenderTarget& target) const
{
	target.draw(m_debugShape);
}

sf::Vector2f MapObject::firstPoint() const
{
	if (!m_polypoints.empty())
		return m_polypoints[0] + m_position;
	else
		return sf::Vector2f();
}

sf::Vector2f MapObject::lastPoint() const
{
	if (!m_polypoints.empty())
		return (m_polypoints.back() + m_position);
	else
		return sf::Vector2f();
}

sf::Vector2f MapObject::collisionNormal(const sf::Vector2f& start, const sf::Vector2f& end) const
{
	Segment trajectory(start, end);
	for (auto& s : m_polySegs)
	{
		if (trajectory.intersects(s))
		{
			Vector2 v = s.end - s.start;
			Vector2 n(v.y, -v.x);

			// invert normal if pointing in wrong direction
			float tAngle = Vector2(end - start).angle().valueDegrees();
			float nAngle = n.angle().valueDegrees();
			if (nAngle - tAngle > 90.f)
				n = -n;

			n.normalise();
			return Vector2::toSFVec2(n);		}
	}

	Vector2 rs(end - start);
	rs.normalise();
	return Vector2::toSFVec2(rs);
}

void MapObject::createSegments()
{
	if (m_polypoints.size() == 0)
	{
		PRINT_ERROR << "Unable to object segments, object data missing." << std::endl;
		PRINT_ERROR << "Check image file paths referenced by tmx file." << std::endl;
		return;
	}

	for (auto i = 0u; i < m_polypoints.size() - 1; ++i)
		m_polySegs.push_back(Segment(m_polypoints[i], m_polypoints[i+1]));

	if (m_shape != Polyline) // close shape
		m_polySegs.push_back(Segment(*(m_polypoints.end() - 1), *m_polypoints.begin()));

	PRINT_ERROR << "Added " << m_polySegs.size() << " segments to Map Object" << std::endl;
}

bool MapObject::convex() const
{
	if (m_shape == MapObjectShape::Polyline)
		return false;

	bool negative = false;
	bool positive = false;

	sf::Uint16 a, b, c, n = m_polypoints.size();
	for (a = 0u; a < n; ++a)
	{
		b = (a + 1) % n;
		c = (b + 1) % n;

		float cross = Vector2(m_polypoints[a]).crossProduct(Vector2(m_polypoints[b]), Vector2(m_polypoints[c]));

		if (cross < 0.f)
			negative = true;
		else if (cross > 0.f)
			positive = true;

		if (positive && negative)
			return false;
	}

	return true;
}

const std::vector<sf::Vector2f>& MapObject::polyPoints() const
{
	return m_polypoints;
}

void MapObject::reverseWinding()
{
	std::reverse(m_polypoints.begin(), m_polypoints.end());
}

void MapObject::setQuad(std::shared_ptr<TileQuad> quad)
{
	m_tileQuad = quad;
}

sf::Vector2f MapObject::_calcCentre() const
{
	if (m_shape == Polyline)
		return sf::Vector2f();

	if (m_shape == Rectangle || m_polypoints.size() < 3)
	{
		// we don't have a triangle so use bounding box
		return sf::Vector2f(m_position.x + (m_size.x / 2.f), m_position.y + (m_size.y / 2.f));
	}

	// calculate centroid of poly shape
	sf::Vector2f centroid;
	float signedArea = 0.f;
	float x0 = 0.f;
	float y0 = 0.f;
	float x1 = 0.f;
	float y1 = 0.f;
	float a = 0.f;	// partial signed area

	unsigned i;
	for (i = 0; i < m_polypoints.size() - 1; ++i)
	{
		x0 = m_polypoints[i].x;
		y0 = m_polypoints[i].y;
		x1 = m_polypoints[i+1].x;
		y1 = m_polypoints[i+1].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid.x += (x0 + x1) * a;
		centroid.y += (y0 + y1) * a;
	}

	x0 = m_polypoints[i].x;
	y0 = m_polypoints[i].y;
	x1 = m_polypoints[0].x;
	y1 = m_polypoints[0].y;
	a = x0 * y1 - x1 * y0;
	signedArea += a;
	centroid.x += (x0 + x1) * a;
	centroid.y += (y0 + y1) * a;

	signedArea *= 0.5f;
	centroid.x /= (6 * signedArea);
	centroid.y /= (6 * signedArea);

	centroid += m_position;
	return centroid;
}

void MapObject::_calcTestValues()
{
	m_centrePoint = _calcCentre();
	for (auto i = m_polypoints.cbegin(); i != m_polypoints.cend(); ++i)
	{
		float length = Vector2(*i - m_centrePoint).length();
		if (m_furthestPoint < length)
		{
			m_furthestPoint = length;

			if (m_shape == Polyline)
				m_centrePoint = *i / 2.f;
		}
	}

	// polyline centre ought to be half way between the start point and the furthest vertex
	if (m_shape == Polyline)
		m_furthestPoint /= 2.f;
}

void MapObject::_createAABB()
{
	if (!m_polypoints.empty())
	{
		m_AABB = sf::FloatRect(m_polypoints[0], m_polypoints[0]);
		for (auto point = m_polypoints.cbegin(); point != m_polypoints.cend(); ++point)
		{
			if (point->x < m_AABB.left) m_AABB.left = point->x;
			if (point->x > m_AABB.width) m_AABB.width = point->x;
			if (point->y < m_AABB.top) m_AABB.top = point->y;
			if (point->y > m_AABB.height) m_AABB.height = point->y;
		}

		// calculate true width and height by distance between points
		m_AABB.width -= m_AABB.left;
		m_AABB.height -= m_AABB.top;

		// offset into world position
		m_AABB.left += m_position.x;
		m_AABB.top += m_position.y;

		//debug draw AABB
		//m_debugShape.append(sf::Vector2f(m_AABB.left, m_AABB.top));
		//m_debugShape.append(sf::Vector2f(m_AABB.left + m_AABB.width, m_AABB.top));
		//m_debugShape.append(sf::Vector2f(m_AABB.left + m_AABB.width, m_AABB.top + m_AABB.height));
		//m_debugShape.append(sf::Vector2f(m_AABB.left, m_AABB.top + m_AABB.height));
	}
}
