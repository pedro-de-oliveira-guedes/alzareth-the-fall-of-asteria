#include "Game.h"
#include "Actors/Items/Collectible/CollectibleItem.h"
#include "Actors/Actor.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "UIElements/UIScreen.h"
#include "Systems/SceneManager/SceneManagerSystem.h"

#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

Game::Game() {
    mWindow = nullptr;
    mRenderer = nullptr;
    mTicksCount = 0;
    mGameState = GameState::PAUSED;
    mWindowWidth = SCREEN_WIDTH;
    mWindowHeight = SCREEN_HEIGHT;
    mBackgroundTexture = nullptr;
    mBackgroundPosition = Vector2::Zero;
    mBackgroundSize = Vector2(SCREEN_WIDTH, SCREEN_HEIGHT);
    mPlayer = nullptr;
    mSpatialHashing = nullptr;
    mCameraPos = Vector2::Zero;
    mAudio = nullptr;
    mEnemies.clear();
    mSceneManager = nullptr;
    mMagicTokenInWorld = false;
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

    mAudio = new AudioSystem(8);
    mTicksCount = SDL_GetTicks();

    mSceneManager = new SceneManagerSystem(this, mAudio);
    mSceneManager->SetGameScene(SceneManagerSystem::GameScene::MainMenu);

    return true;
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

void Game::Win() {
    mSceneManager->SetGameScene(SceneManagerSystem::GameScene::Win);
    mGameState = GameState::PAUSED;
}

void Game::Lose() {
    mSceneManager->SetGameScene(SceneManagerSystem::GameScene::Lose);
    mGameState = GameState::PAUSED;
}

void Game::RunLoop() {
    while (mGameState != GameState::QUITTING) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed) const {
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
                    mSceneManager->TogglePause();
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

    mAudio->Update(deltaTime);

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
    mSceneManager->UpdateSceneManager(deltaTime);
}

void Game::UpdateCamera() {
    if (!mPlayer || mGameState != GameState::PLAYING) return;

    const auto[level_width, level_height] = mSceneManager->GetLevelSize();

    int cameraX = mPlayer->GetPosition().x - (mWindowWidth / 2);
    cameraX = std::max({ cameraX, 0 }); // Locks camera to the left of the screen
    cameraX = std::min(cameraX, level_width * SceneManagerSystem::TILE_SIZE - mWindowWidth); // Locks camera to the right of the screen

    int cameraY = mPlayer->GetPosition().y - (mWindowHeight / 2);
    cameraY = std::max(cameraY, -(mWindowHeight / 6)); // Leaves some space above the screen for the HUD
    cameraY = std::min(
        cameraY,
        (level_height * SceneManagerSystem::TILE_SIZE - mWindowHeight) + (mWindowHeight / 6) // Leaves some space below the screen for the HUD
    );

    SetCameraPos(Vector2(cameraX, cameraY));
}

void Game::UpdateActors(const float deltaTime) {
    if (mEnemies.size() > 0) {
        int defeatedEnemies = 0;
        for (const auto enemy : mEnemies) {
            if (!enemy->IsAlive()) {
                defeatedEnemies++;
            }
        }
        if (defeatedEnemies == mEnemies.size()) {
            mGameState = GameState::PAUSED;
            if (mSceneManager->GetCurrentScene() == SceneManagerSystem::GameScene::Level1) {
                mSceneManager->SetGameScene(SceneManagerSystem::GameScene::Level2);
            } else if (mSceneManager->GetCurrentScene() == SceneManagerSystem::GameScene::Level2) {
                mSceneManager->SetGameScene(SceneManagerSystem::GameScene::Level3);
            } else if (mSceneManager->GetCurrentScene() == SceneManagerSystem::GameScene::Level3) {
                mSceneManager->SetGameScene(SceneManagerSystem::GameScene::Win);
            }
        }
    }

    const std::vector<Actor*> allActors = mSpatialHashing->Query(mPlayer->GetPosition(), 1000);

    bool isPlayerOnCamera = false;
    for (const auto actor : allActors) {
        actor->Update(deltaTime);
        if (actor == mPlayer) {
            isPlayerOnCamera = true;
        }
    }

    if (!isPlayerOnCamera && mPlayer) {
        mPlayer->Update(deltaTime);
    }

    for (const auto actor : allActors) {
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

    if (mSpatialHashing && mGameState != GameState::QUITTING) {
        std::vector<DrawComponent*> drawComponents;

        const std::vector<Actor*> actorsOnCamera = mSpatialHashing->QueryOnCamera(mCameraPos, mWindowWidth, mWindowHeight);
        for (const auto actor : actorsOnCamera) {
            const auto drawable = actor->GetComponent<DrawComponent>();
            if (drawable && drawable->IsVisible()) {
                drawComponents.emplace_back(drawable);
            }
        }

        if (mSceneManager->GetAlzarethShieldDrawComponent())
            drawComponents.emplace_back(mSceneManager->GetAlzarethShieldDrawComponent());

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

    mSceneManager->DrawSceneTransition(mRenderer);

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath, const float alphaChannel) const {
    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
        return nullptr;
    }
    SDL_SetSurfaceAlphaMod(surface, alphaChannel);

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

void Game::ClearGameScene(const bool shouldDeletePlayer) {
    for (const auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    if (mSpatialHashing) {
        const std::vector<Actor*> allActors = mSpatialHashing->Query(mPlayer->GetPosition(), 1000);
        const std::vector<Item*> inventoryItems = mPlayer->GetInventory().GetItems();
        for (auto *actor : allActors) {
            if (actor != mPlayer && std::find(inventoryItems.begin(), inventoryItems.end(), actor) == inventoryItems.end()) {
                delete actor;
            }
        }

        if (shouldDeletePlayer) {
            for (const auto *item : mPlayer->GetInventory().GetItems()) {
                delete item;
            }

            delete mPlayer;
            mPlayer = nullptr;
        }

        delete mSpatialHashing;
        mSpatialHashing = nullptr;
    }

    mEnemies.clear();

    mAudio->StopAllSounds();
}

void Game::Shutdown() {
    ClearGameScene(true);

    for (const auto &font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;
    Mix_CloseAudio();

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

std::pair<int, int> Game::GetEnemiesCount() const {
    int defeatedEnemies = 0;
    for (const auto enemy : mEnemies) {
        if (!enemy->IsAlive()) {
            defeatedEnemies++;
        }
    }

    return {defeatedEnemies, mEnemies.size()};
}

void Game::BuildPlayer(const Vector2 position) {
    if (mPlayer) {
        mPlayer->SetCurrentHealth(mPlayer->GetMaxHealth());
        mPlayer->SetCurrentEnergy(mPlayer->GetMaxEnergy());
        mPlayer->SetPosition(position);
    } else {
        mPlayer = new Player(this);
        mPlayer->SetPosition(position);
    }
}

void Game::BuildSpatialHashing() {
    delete mSpatialHashing;

    const auto [level_width, level_height] = mSceneManager->GetLevelSize();
    mSpatialHashing = new SpatialHashing(
        SceneManagerSystem::TILE_SIZE * 4.0f,
        level_width * SceneManagerSystem::TILE_SIZE,
        level_height * SceneManagerSystem::TILE_SIZE
    );
}
