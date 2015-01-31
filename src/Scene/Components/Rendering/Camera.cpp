#include "Scene/Components/Rendering/Camera.h"
#include "Scene/Components/Transform.h"
#include "Scene/GameObject.h"
#include "Video/VideoManager.h"

sf::RenderTexture* Camera::s_currentRT = 0;

Camera::Camera() :
	m_zoom(0.f),
	m_zoomOut(0.f),
	m_renderTexture(0),
	m_applyViewToRT(false)
{
	m_view = new sf::View();
}

Camera::~Camera()
{
	DELETE_OBJECT(m_view);
}

sf::View* Camera::getView()
{
	return m_view;
}

sf::FloatRect Camera::getViewport()
{
	return m_view->getViewport();
}

void Camera::setViewport(sf::FloatRect port)
{
	m_view->setViewport(port);
}

sf::Vector2f Camera::getSize()
{
	return m_size;
}

void Camera::setSize(sf::Vector2f size)
{
	sf::RenderWindow* wnd = VideoManager::getWindowHandle();
	if (wnd)
	{
		if (size.x < 0.f)
			size.x = (float)(wnd->getSize().x);
		if (size.y < 0.f)
			size.y = (float)(wnd->getSize().y);
	}

	m_size = size;
	m_view->setSize(size);
	m_view->zoom(m_zoomOut);
}

float Camera::getZoom()
{
	return m_zoom;
}

void Camera::setZoom(float value)
{
	m_zoom = value;
	m_zoomOut = value == 0.f ? 0.f : 1.f / value;
	m_view->setSize(m_size);
	m_view->zoom(m_zoomOut);
}

float Camera::getZoomOut()
{
	return m_zoomOut;
}

void Camera::setZoomOut(float value)
{
	m_zoomOut = value;
	m_zoom = value == 0.f ? 0.f : 1.f / value;
	m_view->setSize(m_size);
	m_view->zoom(m_zoomOut);
}

sf::RenderTexture* Camera::getTargetTexture()
{
	return m_renderTexture;
}

void Camera::setTargetTexture(sf::RenderTexture* tex)
{
	m_renderTexture = tex;
}

bool Camera::isApplyViewToRenderTexture()
{
	return m_applyViewToRT;
}

void Camera::setApplyViewToRenderTexture(bool value)
{
	m_applyViewToRT = value;
}

void Camera::onCreate()
{
	if (!getOwner() || getOwner()->isPrefab())
		return;

	setSize(m_size);
}

void Camera::onDuplicate(Component* dest)
{
	if (!dest)
		return;

	Component::onDuplicate(dest);
	if (PTR_TYPEID(dest) != typeid(Camera))
		return;

	Camera* c = (Camera*)(dest);
	c->setSize(getSize());
	c->setZoomOut(getZoomOut());
	c->setZoom(getZoom());
	c->setViewport(getViewport());
	c->setApplyViewToRenderTexture(isApplyViewToRenderTexture());
}

void Camera::onUpdate(const sf::Time& dt)
{
	Transform* trans = getOwner()->getComponent<Transform>();
	if (trans)
	{
		m_view->setCenter(trans->getPosition());
		m_view->setRotation(trans->getRotation());
	}
}

void Camera::onRender(sf::RenderTarget*& target)
{
	if (s_currentRT)
		s_currentRT->display();
	if (m_renderTexture)
	{
		s_currentRT = m_renderTexture;
		target = m_renderTexture;

		if (m_applyViewToRT)
			target->setView(*m_view);
	}
	else
	{
		s_currentRT = 0;
		target->setView(*m_view);
	}
}

template <class Archive>
void Camera::save(Archive& ar, const unsigned int version) const
{
	ar & boost::serialization::base_object<const Component>(*this);

	ar & boost::serialization::make_nvp("size", m_size);
	ar & boost::serialization::make_nvp("zoom", m_zoom);
	ar & boost::serialization::make_nvp("zoomout", m_zoomOut);

	sf::FloatRect v = m_view->getViewport();
	ar & boost::serialization::make_nvp("viewport", v);

	ar & boost::serialization::make_nvp("applyViewToRT", m_applyViewToRT);
}

template <class Archive>
void Camera::load(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Component>(*this);

	ar & boost::serialization::make_nvp("size", m_size);
	ar & boost::serialization::make_nvp("zoom", m_zoom);
	ar & boost::serialization::make_nvp("zoomout", m_zoomOut);

	sf::FloatRect v;
	ar & BOOST_SERIALIZATION_NVP(v);
	setViewport(v);

	ar & boost::serialization::make_nvp("applyViewToRT", m_applyViewToRT);
}

DECLARE_BINARY_SAVE(Camera)
DECLARE_BINARY_LOAD(Camera)

BOOST_CLASS_EXPORT_IMPLEMENT(Camera)
