#ifndef _CORE_H_
#define _CORE_H_

#include "Utils.h"
#include "Math/Math.h"

bool init(const std::string& config);

bool isInit() const;

void handleEvent();

void update(const sf::Time& dt);

void draw();

void shutdown();

void applyConfig();

#endif
