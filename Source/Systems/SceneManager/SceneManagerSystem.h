#pragma once
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

        static constexpr int FIRST_LEVEL_WIDTH = 115;
        static constexpr int FIRST_LEVEL_HEIGHT = 32;
        void BuildFirstLevel();

        void BuildWinScreen();

        void BuildLoseScreen();
};
