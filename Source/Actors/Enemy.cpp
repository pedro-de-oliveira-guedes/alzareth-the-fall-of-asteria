#include "Enemy.h"
#include "./../Game.h"
#include "./../Components/PhysicsComponents/RigidBodyComponent.h"
#include "./../Components/DrawComponents/DrawAnimatedComponent.h"
#include "./../Components/ColliderComponents/AABBColliderComponent.h"


Enemy::Enemy(Game *game) : Actor(game) {


    SDL_Log("Enemy created");

}

void Enemy::OnUpdate(float deltaTime) {
}

void Enemy::ManageAnimations() const {}

void Enemy::OnCollision(float minOverlap, AABBColliderComponent *other) {}

void Enemy::Kill() {

}


float Enemy::GetDamageAttack() {}

void Enemy::TakeDamage(float damage) {
    mCurrentHealth -= damage;
    if (mCurrentHealth <= 0.0f) {
        Kill();
    }
}


