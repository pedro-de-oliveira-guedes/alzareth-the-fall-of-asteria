#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "Actors/Enemies/Enemy.h"
#include "Actors/Player/Player.h"
#include "Systems/Audio/AudioSystem.h"
#include "UIElements/UIScreen.h"
#include "Utils/Math.h"
#include "Utils/SpatialHashing.h"

class Game {
public:
    enum class GameState {
        PLAYING,
        PAUSED,
        QUITTING
    };

    static const int SCREEN_WIDTH = 1280;
    static const int SCREEN_HEIGHT = 720;

    static const int SPRITE_SIZE = 100;

    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mGameState = GameState::QUITTING; }
    void Win();
    void Lose();
    GameState GetGameState() const { return mGameState; }
    void SetGameState(const GameState state) { mGameState = state; }
    std::pair<int, int> GetEnemiesCount() const;

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(Actor *actor);
    void ReinsertActor(Actor* actor);

    // Draw functions
    void SetBackgroundImage(const std::string& texturePath, const Vector2& position, const Vector2& size);

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Audio functions
    AudioSystem* GetAudioSystem() { return mAudio; }

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
    Player* GetPlayer() { return mPlayer; }
    const Player* GetPlayer() const { return mPlayer; }
    std::vector<Actor*> GetNearbyActors(const Vector2& position, int range = 1);
    std::vector<AABBColliderComponent*> GetNearbyColliders(const Vector2& position, int range = 2);

    // Scene manager auxiliary functions
    class SceneManagerSystem* GetSceneManager() const { return mSceneManager; }
    void BuildPlayer(Vector2 position);
    void BuildSpatialHashing();
    void AddEnemy(Enemy* enemy) { mEnemies.emplace_back(enemy); }
    void ClearGameScene(bool shouldDeletePlayer = false);

    bool GetMagicTokenInWorld() const { return mMagicTokenInWorld; }
    void SetMagicTokenInWorld(bool inWorld) { mMagicTokenInWorld = inWorld; }

    SDL_Renderer* GetRenderer() const { return mRenderer; }

private:
    void ProcessInput();
    void HandleKeyPressActors(const int key, const bool isPressed) const;
    void ProcessInputActors();
    void UpdateGame();
    void UpdateCamera();
    void GenerateOutput() const;

    // Game-specific
    GameState mGameState;
    Player* mPlayer;
    std::vector<Enemy*> mEnemies;

    // All the draw components
    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundPosition;
    Vector2 mBackgroundSize;

    // All the collision components
    std::vector<AABBColliderComponent*> mColliders;

    // Scenes related attributes
    class SceneManagerSystem *mSceneManager;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Camera position
    Vector2 mCameraPos;

    // Game audio system
    AudioSystem* mAudio;

    // All the UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;

    // Level data
    SpatialHashing *mSpatialHashing;

    bool mMagicTokenInWorld = false;

    // Window dimensions
    int mWindowWidth;
    int mWindowHeight;
};
