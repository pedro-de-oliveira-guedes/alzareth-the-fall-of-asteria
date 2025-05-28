#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include "Utils/Math.h"
#include "Actors/Player/Player.h"

class Game {
    public:
        enum class GameState {
            RUNNING,
            QUITTING
        };

        static const int SCREEN_WIDTH = 1280;
        static const int SCREEN_HEIGHT = 720;

        static const int LEVEL_WIDTH = 150;
        static const int LEVEL_HEIGHT = 40;
        static const int TILE_SIZE = 32;
        static const int SPRITE_SIZE = 100;

        Game();

        bool Initialize();
        void RunLoop();
        void Shutdown() const;
        void Quit() { mIsRunning = false; }

        // Actor functions
        void InitializeActors();
        void UpdateActors(float deltaTime);
        void AddActor(class Actor *actor);
        void RemoveActor(const Actor *actor);

        // Draw functions
        void AddDrawable(class DrawComponent *drawable);
        void RemoveDrawable(const DrawComponent *drawable);

        // Collider functions
        void AddCollider(class AABBColliderComponent *collider);
        void RemoveCollider(const AABBColliderComponent *collider);
        std::vector<AABBColliderComponent*>& GetColliders() { return mColliders; }

        Vector2& GetCameraPos() { return mCameraPos; };
        void SetCameraPos(const Vector2 &position) { mCameraPos = position; };

        // Window functions
        int GetWindowWidth() const { return mWindowWidth; }
        int GetWindowHeight() const { return mWindowHeight; }

        int** GetLevelData() const { return mLevelData; }

        SDL_Texture* LoadTexture(const std::string &texturePath) const;

    private:
        void ProcessInput();
        void UpdateGame();
        void UpdateCamera();
        void GenerateOutput() const;

        // Game-specific
        Actor *mPlayer;

        // Load the level from a CSV file as a 2D array
        void LoadLevel(const std::string &fileName, int width, int height);
        void BuildLevel(int width, int height);

        // All the actors in the game
        std::vector<Actor*> mActors;
        std::vector<Actor*> mPendingActors;

        // All the draw components
        std::vector<DrawComponent*> mDrawables;

        // All the collision components
        std::vector<AABBColliderComponent*> mColliders;

        // SDL stuff
        SDL_Window *mWindow;
        SDL_Renderer *mRenderer;

        // Window properties
        int mWindowWidth;
        int mWindowHeight;

        // Track elapsed time since game start
        Uint32 mTicksCount;

        // Track if we're updating actors right now
        bool mIsRunning;
        bool mUpdatingActors;

        Vector2 mCameraPos;

        // Level data
        int **mLevelData;
        std::unordered_map<std::string, SDL_Texture *> *mTextures;
};
