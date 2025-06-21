#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "Utils/Math.h"
#include "Actors/Player/Player.h"
#include "Actors/Golem/Golem.h"
#include "Menus/BaseMenu.h"

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
    void Quit() { mGameState = GameState::QUITTING; }
    GameState GetGameState() const { return mGameState; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(const Actor* actor);

    // Draw functions
    void AddDrawable(DrawComponent* drawable);
    void RemoveDrawable(const DrawComponent* drawable);

    // Menu functions
    void InitializeMenus();
    void UpdateMenus() const;
    void AddMenu(BaseMenu* menu);
    void RemoveMenu(const BaseMenu* menu);
    BaseMenu* GetMenu(BaseMenu::MenuType menuType) const;

    // Collider functions
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(const AABBColliderComponent* collider);
    std::vector<AABBColliderComponent*>& GetColliders() { return mColliders; }

    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // UI functions
    void PushUI(class UIScreen* screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }

    // Loading functions
    class UIFont* LoadFont(const std::string& fileName);
    SDL_Texture* LoadTexture(const std::string& texturePath);

    // Getters
    Actor* GetPlayer() { return mPlayer; }
    const Actor* GetPlayer() const { return mPlayer; }

    int** GetLevelData() const { return mLevelData; }

private:
    void ProcessInput();
    void UpdateGame();
    void UpdateCamera();
    void GenerateOutput() const;

    // Game-specific
    Actor* mPlayer;

    Actor* mEnemy;

    // Load the level from a CSV file as a 2D array
    void LoadLevel(const std::string& fileName, int width, int height);
    void BuildLevel(int width, int height);

    // All the actors in the game
    std::vector<Actor*> mActors;
    std::vector<Actor*> mPendingActors;

    // All the draw components
    std::vector<DrawComponent*> mDrawables;

    // All the collision components
    std::vector<AABBColliderComponent*> mColliders;

    // All the menus
    std::vector<BaseMenu*> mMenus;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    // Track if we're updating actors right now
    GameState mGameState;
    bool mUpdatingActors;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if the game is running
    bool mIsRunning;

    // Camera position
    Vector2 mCameraPos;

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;

    // Level data
    int** mLevelData;
    std::unordered_map<std::string, SDL_Texture*>* mTextures;

    // Window dimensions
    int mWindowWidth;
    int mWindowHeight;
};
