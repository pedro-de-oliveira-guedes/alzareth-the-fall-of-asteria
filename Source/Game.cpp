#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include "Utils/CSV.h"
#include "Utils/Random.h"
#include "Game.h"
#include "Actors/Actor.h"
#include "Actors/Background/Texture.h"
#include "Components/DrawComponents/DrawComponent.h"

Game::Game(const int windowWidth, const int windowHeight) {
    mWindow = nullptr;
    mRenderer = nullptr;
    mLevelData = nullptr;
    mTicksCount = 0;
    mIsRunning = true;
    mUpdatingActors = false;
    mWindowWidth = windowWidth;
    mWindowHeight = windowHeight;
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

    Random::Init();

    mTicksCount = SDL_GetTicks();

    // Init all game actors
    InitializeActors();

    return true;
}

void Game::InitializeActors() {
    LoadLevel("../Assets/Levels/Level-1/level_1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);

    if (!mLevelData) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load level data");
        return;
    }

    BuildLevel(LEVEL_WIDTH, LEVEL_HEIGHT);

    mPlayer = new Player(this);
    mPlayer->SetPosition(Vector2(200.f, 200.f));
}

void Game::BuildLevel(const int width, const int height) {
    for (int row = 0; row < height; row++) {
        for (int tile = 0; tile < width; tile++) {
            std::string texturePath;
            if (mLevelData[row][tile] == 0) {
                texturePath = "../Assets/Textures/Border/border-0.png";
            } else if (mLevelData[row][tile] == 1) {
                texturePath = "../Assets/Textures/Finish/finish-0.png";
            } else if (mLevelData[row][tile] == 2) {
                texturePath = "../Assets/Textures/Sand/sand-0.png";
            } else if (mLevelData[row][tile] == 3) {
                texturePath = "../Assets/Textures/Border/border-1.png";
            } else if (mLevelData[row][tile] == 4) {
                texturePath = "../Assets/Textures/Sand/sand-1.png";
            } else if (mLevelData[row][tile] == 6) {
                texturePath = "../Assets/Textures/Finish/finish-1.png";
            } else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown tile type: %d", mLevelData[row][tile]);
            }

            new Texture(this, texturePath, Vector2(tile * TILE_SIZE, row * TILE_SIZE));
        }
    }
}

void Game::LoadLevel(const std::string &fileName, const int width, const int height) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open level file: %s", fileName.c_str());
        return;
    }

    mLevelData = new int*[height];
    for (int i = 0; i < height; i++) mLevelData[i] = new int[width];

    int rowCount = 0;
    std::string row;
    while (std::getline(file, row)) {
        const std::vector<int> rowData = CSVHelper::Split(row);
        if (rowData.size() != width) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse level data. Wrong file width");
            return;
        }

        rowCount++;
        if (rowCount > height) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse level data. Wrong file height (+)");
            return;
        }

        for (int block = 0; block < width; block++)
            mLevelData[rowCount-1][block] = rowData[block];
    }

    if (rowCount != height) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse level data. Wrong file height (-)");
    }
}

void Game::RunLoop() {
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Quit();
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    for (const auto actor : mActors) {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame() {
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    mTicksCount = SDL_GetTicks();

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateCamera() {
    int cameraX = mPlayer->GetPosition().x - (mWindowWidth / 2);
    cameraX = std::max({cameraX, 0}); // Locks camera to the left of the screen
    cameraX = std::min(cameraX, LEVEL_WIDTH * TILE_SIZE - mWindowWidth); // Locks camera to the right of the screen

    int cameraY = mPlayer->GetPosition().y - (mWindowHeight / 2);
    cameraY = std::max(cameraY, 0); // Locks camera to the top of the screen
    cameraY = std::min(cameraY, LEVEL_HEIGHT * TILE_SIZE - mWindowHeight); // Locks camera to the bottom of the screen

    SetCameraPos(Vector2(cameraX, cameraY));
}

void Game::UpdateActors(const float deltaTime) {
    mUpdatingActors = true;
    for (const auto actor : mActors) {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors) {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors) {
        if (actor->GetState() == ActorState::Destroy) {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors) {
        delete actor;
    }
}

void Game::AddActor(Actor *actor) {
    if (mUpdatingActors)
        mPendingActors.emplace_back(actor);
    else
        mActors.emplace_back(actor);
}

void Game::RemoveActor(const Actor *actor) {
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(DrawComponent *drawable) {
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](const DrawComponent* a, const DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(const DrawComponent *drawable) {
    const auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(AABBColliderComponent *collider) {
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(const AABBColliderComponent *collider) {
    const auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput() const {
    // Set draw color to black
    SDL_SetRenderDrawColor(mRenderer, 107, 140, 255, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    for (const auto drawable : mDrawables) {
        if (drawable->IsVisible())
            drawable->Draw(mRenderer);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const std::string &texturePath) const {
    SDL_Surface *surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);
    if (texture) return texture;

    return nullptr;
}

void Game::Shutdown() const {
    // Delete actors
    while (!mActors.empty())
        delete mActors.back();

    // Delete level data
    if (mLevelData != nullptr) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            if (mLevelData[i] != nullptr)
                delete[] mLevelData[i];
        }
    }
    delete[] mLevelData;

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
