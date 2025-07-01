#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "Utils/Math.h"
#include "Actors/Player/Player.h"
#include "UIElements/UIScreen.h"
#include "Utils/SpatialHashing.h"

class Game {
public:
    enum class GameState {
        PLAYING,
        PAUSED,
        QUITTING
    };

    enum class SceneManagerState {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GameScene {
        MainMenu,
        Level1,
        Level2
    };

    static const int SCREEN_WIDTH = 1280;
    static const int SCREEN_HEIGHT = 720;

    static const int LEVEL_WIDTH = 150;
    static const int LEVEL_HEIGHT = 40;
    static const int TILE_SIZE = 32;
    static const int SPRITE_SIZE = 100;

    static const int TRANSITION_TIME = 1; // seconds

    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mGameState = GameState::QUITTING; }
    GameState GetGameState() const { return mGameState; }

    // Scene management
    void SetGameScene(GameScene scene, float transitionTime = 0.0f);
    void ResetGameScene(float transitionTime = 0.0f);
    void UnloadScene();
    void ChangeScene();
    void BuildMainMenu();
    UIScreen* BuildPauseMenu();
    void BuildFirstLevel();

    // Actor functions
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(Actor *actor);
    void ReinsertActor(Actor* actor);

    // Draw functions
    void SetBackgroundImage(const std::string& texturePath, const Vector2& position, const Vector2& size);

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
    Player* GetPlayer() { return mPlayer; }
    const Player* GetPlayer() const { return mPlayer; }
    std::vector<Actor*> GetNearbyActors(const Vector2& position, int range = 1);
    std::vector<AABBColliderComponent*> GetNearbyColliders(const Vector2& position, int range = 2);

    SDL_Renderer* GetRenderer() const { return mRenderer; }

private:
    void ProcessInput();
    void TogglePause();
    void HandleKeyPressActors(const int key, const bool isPressed);
    void ProcessInputActors();
    void UpdateGame();
    void UpdateCamera();
    void GenerateOutput() const;

    // Game-specific
    Player* mPlayer;

    // All the draw components
    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundPosition;
    Vector2 mBackgroundSize;

    // All the collision components
    std::vector<AABBColliderComponent*> mColliders;

    // Scenes related attributes
    void UpdateSceneManager(float deltaTime);
    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;
    float mSceneManagerTransitionTime;
    GameScene mGameScene;
    GameScene mNextScene;

    // SDL stuff
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    // Track if we're updating actors right now
    GameState mGameState;
    bool mUpdatingActors;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Camera position
    Vector2 mCameraPos;

    // All the UI elements
    UIScreen *mPauseMenu;
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class UIFont*> mFonts;

    // Level data
    SpatialHashing *mSpatialHashing;

    // Window dimensions
    int mWindowWidth;
    int mWindowHeight;
};
