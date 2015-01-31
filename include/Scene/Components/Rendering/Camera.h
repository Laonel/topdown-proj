#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Scene/Component.h"

#include <SFML/Graphics.hpp>

class Camera : public Component
{
	friend class Scene;

public:

	inline static Component* onBuildComponent() { return new Camera(); }

	Camera();

	virtual ~Camera();

	sf::View* getView();

	sf::FloatRect getViewport();

	void setViewport(sf::FloatRect);

	sf::Vector2f getSize();

	void setSize(sf::Vector2f);

	float getZoom();

	void setZoom(float value);

	float getZoomOut();

	void setZoomOut(float value);

	sf::RenderTexture* getTargetTexture();

	void setTargetTexture(sf::RenderTexture* tex);

	bool isApplyViewToRenderTexture();

	void setApplyViewToRenderTexture(bool value);

protected:

	virtual void onCreate();
	virtual void onDuplicate(Component* dest);
	virtual void onUpdate(const sf::Time& dt);
	virtual void onRender(sf::RenderTarget*& target);

private:

	static sf::RenderTexture* s_currentRT;

	sf::View* m_view;
	sf::Vector2f m_size;
	float m_zoom;
	float m_zoomOut;

	sf::RenderTexture* m_renderTexture;
	bool m_applyViewToRT;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const;

	template <class Archive>
	void load(Archive& ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(Camera, 1)

BOOST_CLASS_EXPORT_KEY(Camera)

#endif
