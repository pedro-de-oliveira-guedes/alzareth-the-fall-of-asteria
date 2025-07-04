#include "SceneManagerSystem.h"
#include "../../Utils/Random.h"
#include "../../Actors/Golem/Golem.h"
#include "../../Actors/Skeleton/Skeleton.h"

SceneManagerSystem::SceneManagerSystem(Game *game, AudioSystem *audioSystem) {
    mGame = game;
    mAudio = audioSystem;

    mSceneManagerState = SceneManagerState::None;
    mSceneManagerTimer = 0.0f;
    mSceneManagerTransitionTime = 0.0f;
    mGameScene = GameScene::MainMenu;
    mNextScene = GameScene::MainMenu;

    Random::Init();
}

SceneManagerSystem::~SceneManagerSystem() {
    UnloadScene();
}

void SceneManagerSystem::SetGameScene(const GameScene scene) {
    if (mSceneManagerState == SceneManagerState::None) {
        mNextScene = scene;
        mSceneManagerState = SceneManagerState::Entering;
        mSceneManagerTimer = TRANSITION_TIME;
        mSceneManagerTransitionTime = TRANSITION_TIME;
    } else {
        SDL_Log("Cannot change scene state while SceneManager is not in None state");
    }
}

void SceneManagerSystem::ResetGameScene() {
    SetGameScene(mGameScene);
}

void SceneManagerSystem::UnloadScene() const {
    mGame->ClearGameScene();
}

void SceneManagerSystem::ChangeScene() {
    UnloadScene();

    if (mNextScene == GameScene::MainMenu) {
        BuildMainMenu();
        mGame->SetGameState(Game::GameState::PAUSED);
    }
    else if (mNextScene == GameScene::Level1) {
        BuildFirstLevel();
        mGame->SetGameState(Game::GameState::PLAYING);
    }
    else if (mNextScene == GameScene::Win) {
        BuildWinScreen();
        mGame->SetGameState(Game::GameState::PAUSED);
    }
    else if (mNextScene == GameScene::Lose) {
        BuildLoseScreen();
        mGame->SetGameState(Game::GameState::PAUSED);
    }
    else {
        SDL_Log("Invalid game scene: %d", static_cast<int>(mNextScene));
        return;
    }

    mGameScene = mNextScene;
}

void SceneManagerSystem::UpdateSceneManager(const float deltaTime) {
    if (mSceneManagerState == SceneManagerState::Entering) {
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f) {
            ChangeScene();
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerTransitionTime = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active;
        }
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        mSceneManagerTimer -= deltaTime;
        if (mSceneManagerTimer <= 0.0f) {
            mSceneManagerState = SceneManagerState::None;
        }
    }
}

void SceneManagerSystem::BuildMainMenu() {
    mGame->SetCameraPos(Vector2::Zero);
    mGame->SetBackgroundImage(
        "../Assets/Sprites/Menus/MainMenuBKG.png",
        Vector2::Zero,
        Vector2(mGame->GetWindowWidth(), mGame->GetWindowHeight())
    );

    const auto mainMenu = new UIScreen(mGame, "../Assets/Fonts/PixelifySans.ttf");

    const auto playButton = mainMenu->AddButton(
        "Novo Jogo",
        Vector2(mGame->GetWindowWidth() / 2.0f - 120.0f, mGame->GetWindowHeight() / 2.0f + 50.0f),
        Vector2(240.0f, 50.0f),
        [this]() { SetGameScene(GameScene::Level1); }
    );

    mainMenu->AddButton(
        "Sair",
        Vector2(
            mGame->GetWindowWidth() / 2.0f - 120.0f,
            playButton->GetPosition().y + playButton->GetSize().y + 10.0f
        ),
        Vector2(240.0f, 50.0f),
        [this]() { mGame->Quit(); }
    );
}

void SceneManagerSystem::TogglePause() const {
    if (mGameScene != GameScene::MainMenu) {
        if (mGame->GetGameState() == Game::GameState::PLAYING) {
            mGame->SetGameState(Game::GameState::PAUSED);
            mPauseMenu->SetState(UIScreen::UIState::Active);

            mAudio->PauseSound(mMainMusicHandle);
            mAudio->PlaySound("menu_click.ogg", false);
        } else if (mGame->GetGameState() == Game::GameState::PAUSED) {
            mGame->SetGameState(Game::GameState::PLAYING);
            mPauseMenu->SetState(UIScreen::UIState::Idle);

            mAudio->PlaySound("menu_click.ogg", false);
            mAudio->ResumeSound(mMainMusicHandle);
        }
    }
}

void SceneManagerSystem::BuildPauseMenu() {
    const auto pauseMenu = new UIScreen(mGame, "../Assets/Fonts/PixelifySans.ttf");
    pauseMenu->SetState(UIScreen::UIState::Idle);

    const auto menuSize = Vector2(720.f, 480.f);
    const auto screenSize = Vector2(mGame->GetWindowWidth(), mGame->GetWindowHeight());

    pauseMenu->AddImage(
        "../Assets/Sprites/Menus/PauseMenuBKG.png",
        0.5f * (screenSize - menuSize),
        menuSize
    );

    const auto continueButton = pauseMenu->AddButton(
        "Continuar",
        Vector2(screenSize.x / 2.0f - 120.0f, screenSize.y / 2.0f - 80.0f),
        Vector2(240.0f, 50.0f),
        [this]() { TogglePause(); }
    );
    continueButton->SetDrawOrder(1);

    const auto mainMenuButton = pauseMenu->AddButton(
        "Menu Principal",
        Vector2(screenSize.x / 2.0f - 180.0f, continueButton->GetPosition().y + continueButton->GetSize().y + 15.0f),
        Vector2(360.0f, 50.0f),
        [this]() { SetGameScene(GameScene::MainMenu); }
    );
    mainMenuButton->SetDrawOrder(1);

    const auto exitButton = pauseMenu->AddButton(
        "Sair",
        Vector2(screenSize.x / 2.0f - 80.0f, mainMenuButton->GetPosition().y + mainMenuButton->GetSize().y + 15.0f),
        Vector2(160.0f, 50.0f),
        [this]() { mGame->Quit(); }
    );
    exitButton->SetDrawOrder(1);

    mPauseMenu = pauseMenu;
}

void SceneManagerSystem::BuildFirstLevel() {
    mGame->SetBackgroundImage(
        "../Assets/Levels/Level-1/level_1_no_border.png",
        Vector2::Zero,
        Vector2(TILE_SIZE * FIRST_LEVEL_WIDTH, TILE_SIZE * FIRST_LEVEL_HEIGHT)
    );

    mGame->BuildSpatialHashing();
    mGame->BuildPlayer(Vector2(200.0f, 200.0f));

    for (int i = 0; i < 1; i++) {
        const float offsetX = Random::GetFloatRange(250, FIRST_LEVEL_WIDTH * TILE_SIZE - 250);
        const float offsetY = Random::GetFloatRange(250, FIRST_LEVEL_HEIGHT * TILE_SIZE - 250);
        //mGame->AddEnemy(new Golem(mGame, Vector2(offsetX, offsetY)));

        mGame->AddEnemy(new Skeleton(mGame, Vector2(offsetX, offsetY)));

    }

    new Sword(
        mGame,
        "Sword",
        "../Assets/Sprites/Weapons/Sword/sword.png",
        "../Assets/Sprites/Weapons/Sword/sword_inv.png",
        "../Assets/Sprites/Weapons/Sword/sword.json",
        Vector2(300.0f, 300.0f) ,
        1
    );

    BuildPauseMenu();
    mMainMusicHandle = mAudio->PlaySound("level1.wav", true);
}

void SceneManagerSystem::BuildWinScreen() {
    mGame->SetCameraPos(Vector2::Zero);
    mGame->SetBackgroundImage(
        "../Assets/Sprites/Menus/WinScreenBKG.png",
        Vector2::Zero,
        Vector2(mGame->GetWindowWidth(), mGame->GetWindowHeight())
    );

    const auto winScreen = new UIScreen(mGame, "../Assets/Fonts/PixelifySans.ttf");

    auto *button = winScreen->AddButton(
        "Pressione Enter!",
        Vector2(mGame->GetWindowWidth() / 2.0f - 300.0f, mGame->GetWindowHeight() - 100.0f),
        Vector2(600.0f, 50.0f),
        [this]() { SetGameScene(GameScene::MainMenu); }
    );
    button->SetHighlighted(false);
}

void SceneManagerSystem::BuildLoseScreen() {
    mGame->SetCameraPos(Vector2::Zero);
    mGame->SetBackgroundImage(
        "../Assets/Sprites/Menus/LoseScreenBkg.png",
        Vector2::Zero,
        Vector2(mGame->GetWindowWidth(), mGame->GetWindowHeight())
    );

    const auto loseScreen = new UIScreen(mGame, "../Assets/Fonts/PixelifySans.ttf");

    auto *button = loseScreen->AddButton(
        "Pressione Enter!",
        Vector2(mGame->GetWindowWidth() / 2.0f - 300.0f, mGame->GetWindowHeight() - 100.0f),
        Vector2(600.0f, 50.0f),
        [this]() { SetGameScene(GameScene::MainMenu); }
    );
    button->SetHighlighted(false);
}

std::pair<int, int> SceneManagerSystem::GetLevelSize() const {
    if (mGameScene == GameScene::Level1 || mNextScene == GameScene::Level1) {
        return {FIRST_LEVEL_WIDTH, FIRST_LEVEL_HEIGHT};
    }

    return {0, 0};
}

void SceneManagerSystem::DrawSceneTransition(SDL_Renderer *renderer) const {
    if (mSceneManagerState == SceneManagerState::Entering) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>((1 - mSceneManagerTimer / mSceneManagerTransitionTime) * 255));
        const SDL_Rect rect = { 0, 0, mGame->GetWindowWidth(), mGame->GetWindowHeight() };
        SDL_RenderFillRect(renderer, &rect);
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>((mSceneManagerTimer / mSceneManagerTransitionTime) * 255));
        const SDL_Rect rect = { 0, 0, mGame->GetWindowWidth(), mGame->GetWindowHeight() };
        SDL_RenderFillRect(renderer, &rect);
    }
}
