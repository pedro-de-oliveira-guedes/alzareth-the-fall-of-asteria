#pragma once
#include "../../Actors/Alzareth/Alzareth.h"
#include "../../Game.h"

class SceneManagerSystem {
    public:
        static const int TRANSITION_TIME = 1; // seconds

        enum class SceneManagerState {
            None,
            Entering,
            Active,
            Exiting
        };

        enum class GameScene {
            MainMenu,
            Level1,
            Level2,
            Level3,
            Win,
            Lose
        };

        static const int TILE_SIZE = 32;
        SceneManagerSystem(Game *game, AudioSystem *audioSystem);
        ~SceneManagerSystem();

        void SetGameScene(GameScene scene);
        void ResetGameScene();
        void UnloadScene() const;
        void ChangeScene();
        void UpdateSceneManager(float deltaTime);

        void TogglePause() const;

        std::pair<int, int> GetLevelSize() const;

        void DrawSceneTransition(SDL_Renderer *renderer) const;
        DrawAnimatedComponent* GetAlzarethShieldDrawComponent() const;

        GameScene GetCurrentScene() const { return mGameScene; }

    private:
        Game *mGame;
        AudioSystem *mAudio;

        UIScreen *mPauseMenu;
        SoundHandle mMainMusicHandle;

        SceneManagerState mSceneManagerState;
        float mSceneManagerTimer;
        float mSceneManagerTransitionTime;
        GameScene mGameScene;
        GameScene mNextScene;

        void BuildMainMenu();

        void BuildPauseMenu();

        static constexpr int FIRST_SECOND_LEVEL_WIDTH = 115;
        static constexpr int FIRST_SECOND_LEVEL_HEIGHT = 32;
        static constexpr int THIRD_LEVEL_WIDTH = 40;
        static constexpr int THIRD_LEVEL_HEIGHT = 22;
        void BuildFirstLevel();
        void BuildSecondLevel();
        void BuildThirdLevel();
        Alzareth *mAlzareth = nullptr;

        void BuildWinScreen();

        void BuildLoseScreen();
};
