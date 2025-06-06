cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "alzareth-the-fall-of-asteria")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Utils/CSV.h
        Source/Utils/Json.h
        Source/Utils/Math.cpp
        Source/Utils/Math.h
        Source/Utils/Random.cpp
        Source/Utils/Random.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/ColliderComponents/AABBColliderComponent.cpp
        Source/Components/ColliderComponents/AABBColliderComponent.h
        Source/Components/ColliderComponents/CircleColliderComponent.cpp
        Source/Components/ColliderComponents/CircleColliderComponent.h
        Source/Components/DrawComponents/DrawAnimatedComponent.cpp
        Source/Components/DrawComponents/DrawAnimatedComponent.h
        Source/Components/DrawComponents/DrawComponent.cpp
        Source/Components/DrawComponents/DrawComponent.h
        Source/Components/DrawComponents/DrawPolygonComponent.cpp
        Source/Components/DrawComponents/DrawPolygonComponent.h
        Source/Components/DrawComponents/DrawSpriteComponent.cpp
        Source/Components/DrawComponents/DrawSpriteComponent.h
        Source/Components/PhysicsComponents/RigidBodyComponent.cpp
        Source/Components/PhysicsComponents/RigidBodyComponent.h
        Source/Game.cpp
        Source/Main.cpp
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Player/Player.cpp
        Source/Actors/Player/Player.h
        Source/Actors/Background/Texture.cpp
        Source/Actors/Background/Texture.h
        Source/Components/UI/HUDComponent.cpp
        Source/Components/UI/HUDComponent.h
        Source/Menus/BaseMenu.cpp
        Source/Menus/BaseMenu.h
        Source/Menus/Pause/PauseMenu.cpp
        Source/Menus/Pause/PauseMenu.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
