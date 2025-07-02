#include "Enemy.h"
#include "./../Game.h"
#include "./../Components/PhysicsComponents/RigidBodyComponent.h"
#include "./../Components/DrawComponents/DrawAnimatedComponent.h"
#include "./../Components/ColliderComponents/AABBColliderComponent.h"


Enemy::Enemy(Game *game) : Actor(game) {}

void Enemy::OnUpdate(float deltaTime) {
}

void Enemy::ManageAnimations() const {}

void Enemy::OnCollision(float minOverlap, AABBColliderComponent *other) {}

void Enemy::Kill() {

}

void Enemy::TakeDamage(const float damage) {
    mCurrentHealth -= damage;

    if (mGame->GetAudioSystem()->GetSoundState(mDamageSound) != SoundState::Playing)
        mDamageSound = mGame->GetAudioSystem()->PlaySound("enemy_damage.mp3", false);

    if (mCurrentHealth <= 0.0f) {
        Kill();
    }
}


