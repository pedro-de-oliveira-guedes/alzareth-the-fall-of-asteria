#include "Texture.h"

#include "../../Game.h"
#include "../../Components/DrawComponents/DrawSpriteComponent.h"

Texture::Texture(Game *game, const std::string &texturePath, const Vector2 &startPosition) : Actor(game) {
    new DrawSpriteComponent(this, texturePath, Game::TILE_SIZE, Game::TILE_SIZE, 99);

    SetPosition(startPosition);
}
