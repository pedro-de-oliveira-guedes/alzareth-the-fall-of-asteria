#pragma once

#include "DrawSpriteComponent.h"
#include <unordered_map>
#include <vector>

class DrawAnimatedComponent : public DrawSpriteComponent {
    public:
        // (Lower draw order corresponds with further back)
        DrawAnimatedComponent(
            Actor* owner,
            const std::string &spriteSheetPath,
            const std::string &spriteSheetData,
            int drawOrder = 100
        );
        ~DrawAnimatedComponent() override;

        void Draw(SDL_Renderer *renderer) override;
        void Update(float deltaTime) override;

        // Use to change the FPS of the animation
        void SetAnimFPS(float fps) { mAnimFPS = fps; }

        // Set the current active animation
        void SetAnimation(const std::string &name);

        // Use to pause/unpause the animation
        void SetIsPaused(bool pause) { mIsPaused = pause; }

        // Add an animation of the corresponding name to the animation map
        void AddAnimation(const std::string &name, const std::vector<int> &images);

        // Necessary time to play the animation completely
        float GetAnimTime(const std::string &animName) const {
            return (1.f / mAnimFPS) * static_cast<float>(mAnimations.at(animName).size());
        }

        void SetRotation(float angle) { mRotation = angle; }

    private:
        void LoadSpriteSheet(const std::string &texturePath, const std::string &dataPath);

        // Vector of sprites
        std::vector<SDL_Rect*> mSpriteSheetData;

        // Map of animation name to vector of textures corresponding to the animation
        std::unordered_map<std::string, std::vector<int>> mAnimations;

        // Name of current animation
        std::string mAnimName;

        // Tracks current elapsed time in animation
        float mAnimTimer = 0.0f;

        // The frames per second the animation should run at
        float mAnimFPS = 10.0f;

        // Whether the animation is paused (defaults to false)
        bool mIsPaused = false;

        float mRotation = 0.0f;
};
