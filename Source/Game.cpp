#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <memory>
#include <SDL_image.h>
#include "Utils/CSV.h"
#include "Utils/Random.h"
#include "Game.h"
#include "UIElements/UIScreen.h"
#include "Actors/Actor.h"
#include "Actors/Background/Texture.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Menus/BaseMenu.h"
#include "Menus/Pause/PauseMenu.h"
#include "Items/CollectibleItem.h"

Game::Game() {
    mWindow = nullptr;
    mRenderer = nullptr;
    mLevelData = nullptr;
    mTicksCount = 0;
    mGameState = GameState::RUNNING;
    mUpdatingActors = false;
    mWindowWidth = Game::SCREEN_WIDTH;
    mWindowHeight = Game::SCREEN_HEIGHT;
    mTextures = new std::unordered_map<std::string, SDL_Texture*>();
}

bool Game::Initialize() {
    auto start_time = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);

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

    Random::Init();

    mTicksCount = SDL_GetTicks();

    InitializeActors();
    InitializeMenus();

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

    // TODO 1:
    // Remove this hardcoded items initialization
    Player* player = static_cast<Player*>(mPlayer);
    // for (int i = 0; i < 5; ++i) {
    //     float offsetX = static_cast<float>(Random::GetIntRange(-500, 500));
    //     float offsetY = static_cast<float>(Random::GetIntRange(-500, 500));

    //     float spawnX = player->GetPosition().x + offsetX;
    //     float spawnY = player->GetPosition().y + offsetY;

    //     spawnX = Math::Clamp(spawnX, 0.0f, static_cast<float>(LEVEL_WIDTH * TILE_SIZE - TILE_SIZE));
    //     spawnY = Math::Clamp(spawnY, 0.0f, static_cast<float>(LEVEL_HEIGHT * TILE_SIZE - TILE_SIZE));

    //     // new CollectibleItem(this, "Energy Potion", ItemType::Consumable,
    //     //     "../Assets/Sprites/Items/Energy/energy_potion.png",
    //     //     "../Assets/Sprites/Items/Energy/energy_potion_inventory.png",
    //     //     "../Assets/Sprites/Items/Energy/energy_potion.json",
    //     //     1, Vector2(spawnX, spawnY));

    //     // new CollectibleItem(this, "Health_Potion", ItemType::Consumable,
    //     //     "../Assets/Sprites/Items/Health/health_potion.png",
    //     //     "../Assets/Sprites/Items/Health/health_potion_inventory.png",
    //     //     "../Assets/Sprites/Items/Health/health_potion.json",
    //     //     1, Vector2(spawnX, spawnY));
    // }
}

void Game::InitializeMenus() {
    new PauseMenu(this);
}

void Game::BuildLevel(const int width, const int height) {
    for (int row = 0; row < height; row++) {
        for (int tile = 0; tile < width; tile++) {
            std::string texturePath;
            if (mLevelData[row][tile] == 0) {
                texturePath = "../Assets/Textures/Border/border-0.png";
            }
            else if (mLevelData[row][tile] == 1) {
                texturePath = "../Assets/Textures/Finish/finish-0.png";
            }
            else if (mLevelData[row][tile] == 2) {
                texturePath = "../Assets/Textures/Sand/sand-0.png";
            }
            else if (mLevelData[row][tile] == 3) {
                texturePath = "../Assets/Textures/Border/border-1.png";
            }
            else if (mLevelData[row][tile] == 4) {
                texturePath = "../Assets/Textures/Sand/sand-1.png";
            }
            else if (mLevelData[row][tile] == 6) {
                texturePath = "../Assets/Textures/Finish/finish-1.png";
            }
            else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unknown tile type: %d", mLevelData[row][tile]);
            }

            new Texture(this, texturePath, Vector2(tile * TILE_SIZE, row * TILE_SIZE));
        }
    }
}

void Game::LoadLevel(const std::string& fileName, const int width, const int height) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open level file: %s", fileName.c_str());
        return;
    }

    mLevelData = new int* [height];
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
            mLevelData[rowCount - 1][block] = rowData[block];
    }

    if (rowCount != height) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse level data. Wrong file height (-)");
    }
}

void Game::RunLoop() {
    while (mGameState != GameState::QUITTING) {
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
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                const auto menu = GetMenu(BaseMenu::PAUSE_MENU);
                menu->SetState(BaseMenu::RUNNING);
            }
            break;
        default:
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
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

    // Update all menus
    UpdateMenus();

    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateMenus() const {
    for (const auto& menu : mMenus) {
        menu->MenuLoop(mRenderer);
    }
}

void Game::UpdateCamera() {
    int cameraX = mPlayer->GetPosition().x - (mWindowWidth / 2);
    cameraX = std::max({ cameraX, 0 }); // Locks camera to the left of the screen
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

void Game::AddActor(Actor* actor) {
    if (mUpdatingActors)
        mPendingActors.emplace_back(actor);
    else
        mActors.emplace_back(actor);
}

void Game::RemoveActor(const Actor* actor) {
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

void Game::AddDrawable(DrawComponent* drawable) {
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(), [](const DrawComponent* a, const DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
        });
}

void Game::RemoveDrawable(const DrawComponent* drawable) {
    const auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(AABBColliderComponent* collider) {
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(const AABBColliderComponent* collider) {
    const auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::AddMenu(BaseMenu* menu) {
    mMenus.emplace_back(menu);
}

void Game::RemoveMenu(const BaseMenu* menu) {
    const auto iter = std::find(mMenus.begin(), mMenus.end(), menu);
    if (iter != mMenus.end()) {
        mMenus.erase(iter);
    }
}

BaseMenu* Game::GetMenu(const BaseMenu::MenuType menuType) const {
    for (const auto menu : mMenus) {
        if (menu->GetType() == menuType)
            return menu;
    }

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "The queried menu type was not found %d.", menuType);
    return nullptr;
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

SDL_Texture* Game::LoadTexture(const std::string& texturePath) {
    if (mTextures->find(texturePath) != mTextures->end()) {
        return mTextures->at(texturePath);
    }

    SDL_Surface* surface = IMG_Load(texturePath.c_str());
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);
    mTextures->insert(std::make_pair(texturePath, texture));

    return texture;
}

UIFont* Game::LoadFont(const std::string& fileName)
{
    if (mFonts.find(fileName) != mFonts.end())
    {
        return mFonts[fileName];
    }
    else {
        UIFont* font = new UIFont(mRenderer);
        if (font->Load(fileName)) {
            mFonts[fileName] = font;
            return font;
        }
        else {
            font->Unload();
            delete font;
            return nullptr;
        }
    }
}

void Game::Shutdown() const {
    // Delete actors
    while (!mActors.empty())
        delete mActors.back();

    while (!mMenus.empty())
        delete mMenus.back();

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



