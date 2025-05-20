#pragma once

#include "../Actor.h"
#include <string>

class Texture : public Actor {
    public:
        explicit Texture(Game *game, const std::string &texturePath, const Vector2 &startPosition);
};
