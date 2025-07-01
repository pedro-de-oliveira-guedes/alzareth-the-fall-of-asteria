#include "Game.h"
#include "Actors/Items/Collectible/CollectibleItem.h"
#include "Actors/Actor.h"
#include "Actors/Golem//Golem.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "UIElements/UIScreen.h"
#include "Utils/Random.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>

Game::Game() {
    mWindow = nullptr;
    mRenderer = nullptr;
    mTicksCount = 0;
    mGameState = GameState::PAUSED;
    mUpdatingActors = false;
    mWindowWidth = Game::SCREEN_WIDTH;
    mWindowHeight = Game::SCREEN_HEIGHT;
    mBackgroundTexture = nullptr;
    mBackgroundPosition = Vector2::Zero;
    mBackgroundSize = Vector2(SCREEN_WIDTH, SCREEN_HEIGHT);
    mPlayer = nullptr;
    mPauseMenu = nullptr;
    mSpatialHashing = nullptr;
    mCameraPos = Vector2::Zero;
    mSceneManagerState = SceneManagerState::None;
    mSceneManagerTimer = 0.0f;
    mSceneManagerTransitionTime = 0.0f;
    mGameScene = GameScene::MainMenu;
    mNextScene = GameScene::MainMenu;
}

bool Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Alzareth: The Fall of Asteria", 25, 25, mWindowWidth, mWindowHeight, 0);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() != 0) {
        SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
        return false;
    }

    Random::Init();

    mTicksCount = SDL_GetTicks();

    SetGameScene(GameScene::MainMenu, TRANSITION_TIME);

    return true;
}

void Game::SetGameScene(GameScene scene, const float transitionTime) {
    if (mSceneManagerState == SceneManagerState::None) {
        if (scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2) {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
            mSceneManagerTransitionTime = transitionTime;
        } else {
            SDL_Log("Invalid game scene: %d", static_cast<int>(scene));
        }
    } else {
        SDL_Log("Cannot change scene state while SceneManager is not in None state");
    }
}

void Game::ResetGameScene(const float transitionTime) {
    SetGameScene(mGameScene, transitionTime);
}

void Game::UnloadScene() {
    delete mSpatialHashing;
    mSpatialHashing = nullptr;

    for (const auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::ChangeScene() {
    UnloadScene();

    if (mNextScene == GameScene::MainMenu) {
        BuildMainMenu();
        mGameState = GameState::PAUSED;
    }
    else if (mNextScene == GameScene::Level1) {
        BuildFirstLevel();
        mGameState = GameState::PLAYING;
    }

    mGameScene = mNextScene;
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2& position, const Vector2& size) {
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    mBackgroundPosition.Set(position.x, position.y);
    mBackgroundSize.Set(size.x, size.y);
}

void Game::BuildMainMenu() {
    SetCameraPos(Vector2::Zero);
    SetBackgroundImage(
        "../Assets/Sprites/Menus/MainMenuBkg.png",
        Vector2::Zero,
        Vector2(SCREEN_WIDTH, SCREEN_HEIGHT)
    );

    const auto mainMenu = new UIScreen(this, "../Assets/Fonts/PixelifySans.ttf");

    const auto playButton = mainMenu->AddButton(
        "Novo Jogo",
        Vector2(mWindowWidth / 2.0f - 120.0f, mWindowHeight / 2.0f + 50.0f),
        Vector2(240.0f, 50.0f),
        [this]() { SetGameScene(GameScene::Level1, TRANSITION_TIME); }
    );

    const auto exitButton = mainMenu->AddButton(
        "Sair",
        Vector2(mWindowWidth / 2.0f - 120.0f, playButton->GetPosition().y + playButton->GetSize().y + 10.0f),
        Vector2(240.0f, 50.0f),
        [this]() { Quit(); }
    );
}

void Game::BuildFirstLevel() {
    SetBackgroundImage(
        "../Assets/Levels/Level-1/level_1_map.png",
        Vector2::Zero,
        Vector2(TILE_SIZE * LEVEL_WIDTH, TILE_SIZE * LEVEL_HEIGHT)
    );

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    mPlayer = new Player(this);
    mPlayer->SetPosition(Vector2(200.f, 200.f));

    for (int i = 0; i < 25; i++) {
        const float offsetX = Random::GetFloatRange(250, 1600);
        const float offsetY = Random::GetFloatRange(250, 1600);
        new Golem(this, Vector2(offsetX, offsetY));
    }

    new Sword(
        this,
        "Sword",
        "../Assets/Sprites/Weapons/Sword/sword.png",
        "../Assets/Sprites/Weapons/Sword/sword_inv.png",
        "../Assets/Sprites/Weapons/Sword/sword.json",
        Vector2(300.0f, 300.0f) ,
        1
    );

    mPauseMenu = BuildPauseMenu();
}

UIScreen* Game::BuildPauseMenu() {
    const auto pauseMenu = new UIScreen(this, "../Assets/Fonts/PixelifySans.ttf");
    pauseMenu->SetState(UIScreen::UIState::Idle);

    const auto menuSize = Vector2(720.f, 480.f);
    const auto screenSize = Vector2(SCREEN_WIDTH, SCREEN_HEIGHT);

    pauseMenu->AddImage(
        "../Assets/Sprites/Menus/PauseMenuBkg.png",
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
        [this]() { SetGameScene(GameScene::MainMenu, TRANSITION_TIME); }
    );
    mainMenuButton->SetDrawOrder(1);

    const auto exitButton = pauseMenu->AddButton(
        "Sair",
        Vector2(screenSize.x / 2.0f - 80.0f, mainMenuButton->GetPosition().y + mainMenuButton->GetSize().y + 15.0f),
        Vector2(160.0f, 50.0f),
        [this]() { Quit(); }
    );
    exitButton->SetDrawOrder(1);

    return pauseMenu;
}

void Game::RunLoop() {
    while (mGameState != GameState::QUITTING) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed) {
    if (mGameState == GameState::PLAYING) {
        const std::vector<Actor*> actorsOnCamera = mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);

        bool isPlayerOnCamera = false;
        for (const auto actor : actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mPlayer) {
                isPlayerOnCamera = true;
            }
        }

        if (!isPlayerOnCamera && mPlayer) {
            mPlayer->HandleKeyPress(key, isPressed);
        }
    }
}

void Game::TogglePause() {
    if (mGameScene != GameScene::MainMenu) {
        if (mGameState == GameState::PLAYING) {
            mGameState = GameState::PAUSED;
            mPauseMenu->SetState(UIScreen::UIState::Active);
            // TODO: Handle sounds
        } else if (mGameState == GameState::PAUSED) {
            mGameState = GameState::PLAYING;
            mPauseMenu->SetState(UIScreen::UIState::Idle);
            // TODO: Handle sounds
        }
    }
}

void Game::ProcessInputActors() {
    if(mGameState == GameState::PLAYING) {
        // Get actors on camera
        const std::vector<Actor*> actorsOnCamera = mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isMarioOnCamera = false;
        for (const auto actor: actorsOnCamera) {
            actor->ProcessInput(state);

            if (actor == mPlayer) {
                isMarioOnCamera = true;
            }
        }

        // If Mario is not on camera, process input for him
        if (!isMarioOnCamera && mPlayer) {
            mPlayer->ProcessInput(state);
        }
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    TogglePause();
                }
                break;
            default: ;
        }
    }

    ProcessInputActors();
}

void Game::UpdateGame() {
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    mTicksCount = SDL_GetTicks();

    if (mGameState == GameState::PLAYING) {
        UpdateActors(deltaTime);
    }

    for (const auto ui: mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    UpdateCamera();
    UpdateSceneManager(deltaTime);
}

void Game::UpdateSceneManager(const float deltaTime) {
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

void Game::UpdateCamera() {
    if (!mPlayer || mGameState != GameState::PLAYING) return;

    int cameraX = mPlayer->GetPosition().x - (mWindowWidth / 2);
    cameraX = std::max({ cameraX, 0 }); // Locks camera to the left of the screen
    cameraX = std::min(cameraX, LEVEL_WIDTH * TILE_SIZE - mWindowWidth); // Locks camera to the right of the screen

    int cameraY = mPlayer->GetPosition().y - (mWindowHeight / 2);
    cameraY = std::max(cameraY, -(mWindowHeight / 6)); // Leaves some space above the screen for the HUD
    cameraY = std::min(
        cameraY,
        (LEVEL_HEIGHT * TILE_SIZE - mWindowHeight) + (mWindowHeight / 6) // Leaves some space below the screen for the HUD
    );

    SetCameraPos(Vector2(cameraX, cameraY));
}

void Game::UpdateActors(const float deltaTime) {
    const std::vector<Actor*> actorsOnCamera = mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);

    bool isPlayerOnCamera = false;
    for (const auto actor : actorsOnCamera) {
        actor->Update(deltaTime);
        if (actor == mPlayer) {
            isPlayerOnCamera = true;
        }
    }

    if (!isPlayerOnCamera && mPlayer) {
        mPlayer->Update(deltaTime);
    }

    for (const auto actor : actorsOnCamera) {
        if (actor->GetState() == ActorState::Destroy) {
            RemoveActor(actor);
            delete actor;
            if (actor == mPlayer) {
                mPlayer = nullptr;
            }
        }
    }
}

void Game::AddActor(Actor* actor) {
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor) {
    mSpatialHashing->Remove(actor);
}

void Game::ReinsertActor(Actor* actor) {
    mSpatialHashing->Reinsert(actor);
}

void Game::GenerateOutput() const {
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mRenderer);

    if (mBackgroundTexture) {
        const SDL_Rect dstRect = {
            static_cast<int>(mBackgroundPosition.x - mCameraPos.x),
            static_cast<int>(mBackgroundPosition.y - mCameraPos.y),
            static_cast<int>(mBackgroundSize.x),
            static_cast<int>(mBackgroundSize.y)
        };
        SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &dstRect);
    }

    if (mSpatialHashing) {
        std::vector<DrawComponent*> drawComponents;

        const std::vector<Actor*> actorsOnCamera = mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);
        for (const auto actor : actorsOnCamera) {
            const auto drawable = actor->GetComponent<DrawComponent>();
            if (drawable && drawable->IsVisible()) {
                drawComponents.emplace_back(drawable);
            }
        }
        drawComponents.emplace_back(mPlayer->GetHUDComponent());

        std::sort(
            drawComponents.begin(),
            drawComponents.end(),
            [](const DrawComponent* a, const DrawComponent* b) { return a->GetDrawOrder() < b->GetDrawOrder(); }
        );
        for (const auto drawComponent : drawComponents) {
            drawComponent->Draw(mRenderer);
        }
    }

    for (const auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Draw(mRenderer);
        }
    }

    if (mSceneManagerState == SceneManagerState::Entering) {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, static_cast<Uint8>((1 - mSceneManagerTimer / mSceneManagerTransitionTime) * 255));
        const SDL_Rect rect = { 0, 0, mWindowWidth, mWindowHeight };
        SDL_RenderFillRect(mRenderer, &rect);
    }

    if (mSceneManagerState == SceneManagerState::Active) {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, static_cast<Uint8>((mSceneManagerTimer / mSceneManagerTransitionTime) * 255));
        const SDL_Rect rect = { 0, 0, mWindowWidth, mWindowHeight };
        SDL_RenderFillRect(mRenderer, &rect);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath) {
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

UIFont* Game::LoadFont(const std::string& fileName) {
    if (mFonts.find(fileName) != mFonts.end()) {
        return mFonts[fileName];
    }

    auto *font = new UIFont(mRenderer);
    if (font->Load(fileName)) {
        mFonts[fileName] = font;
        return font;
    }

    font->Unload();
    delete font;
    return nullptr;
}

void Game::Shutdown() {
    UnloadScene();

    for (const auto &font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    // TODO: Clean up audio system

    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

std::vector<Actor*> Game::GetNearbyActors(const Vector2& position, const int range) {
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent*> Game::GetNearbyColliders(const Vector2& position, const int range) {
    return mSpatialHashing->QueryColliders(position, range);
}
