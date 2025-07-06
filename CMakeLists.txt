cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "alzareth-the-fall-of-asteria")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h

        Source/Actors/Items/Item.h
        Source/Actors/Items/Item.cpp
        Source/Actors/Items/Weapons/Weapon.h
        Source/Actors/Items/Weapons/Weapons.cpp
        Source/Actors/Items/Weapons/Melee/Sword.h
        Source/Actors/Items/Weapons/Melee/Sword.cpp
        Source/Actors/Items/Collectible/CollectibleItem.cpp
        Source/Actors/Items/Collectible/CollectibleItem.h

        Source/Actors/Player/Player.cpp
        Source/Actors/Player/Player.h

        Source/Actors/Enemies/Enemy.h
        Source/Actors/Enemies/Enemy.cpp
        Source/Actors/Enemies/Golem/Golem.h
        Source/Actors/Enemies/Golem/Golem.cpp
        Source/Actors/Enemies/Skeleton/Skeleton.h
        Source/Actors/Enemies/Skeleton/Skeleton.cpp

        Source/Actors/Projectile/Projectile.h
        Source/Actors/Projectile/Projectile.cpp
        Source/Actors/Projectile/Fireball/Fireball.h
        Source/Actors/Projectile/Fireball/Fireball.cpp

        Source/Utils/CSV.h
        Source/Utils/Json.h
        Source/Utils/Math.cpp
        Source/Utils/Math.h
        Source/Utils/Random.cpp
        Source/Utils/Random.h
        Source/Utils/SpatialHashing.cpp
        Source/Utils/SpatialHashing.h

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
        Source/Components/UI/HUDComponent.cpp
        Source/Components/UI/HUDComponent.h

        Source/Inventory/Inventory.cpp
        Source/Inventory/Inventory.h

        Source/UIElements/UIText.cpp
        Source/UIElements/UIText.h
        Source/UIElements/UIScreen.cpp
        Source/UIElements/UIScreen.h
        Source/UIElements/UIImage.cpp
        Source/UIElements/UIImage.h
        Source/UIElements/UIFont.cpp
        Source/UIElements/UIFont.h
        Source/UIElements/UIElement.cpp
        Source/UIElements/UIElement.h
        Source/UIElements/UIButton.cpp
        Source/UIElements/UIButton.h

        Source/Systems/Audio/AudioSystem.cpp
        Source/Systems/Audio/AudioSystem.h

        Source/Game.cpp
        Source/Game.h

        Source/Main.cpp
        Source/Systems/SceneManager/SceneManagerSystem.cpp
        Source/Systems/SceneManager/SceneManagerSystem.h
        Source/Actors/Alzareth/Alzareth.cpp
        Source/Actors/Alzareth/Alzareth.h
        Source/Actors/Alzareth/Attacks/DeathRay.cpp
        Source/Actors/Alzareth/Attacks/DeathRay.h
)

# Linkar com as bibliotecas SDL2_ttf
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)
