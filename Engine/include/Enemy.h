#ifndef ENEMY_H
#define ENEMY_H

#include <Model.h>
#include <vector>
#include <Timer.h>
#include <DeltaTime.h>
#include <AudioSource.h>
#include <algorithm>
#include <Player.h>

class Enemy : public Model
{
    public:
        Enemy();
        Enemy(double newX, double newY, double newWidth, double newHeight, string newName, string newTag);
        virtual ~Enemy();

        void Update();
        virtual void InitEnemy() = 0; // will be used by children classes so they have different sprites
        void DrawEnemy();

        void TakeDamage(int damageTaken);

    protected:
        bool jump, slowDown, moving, ignoreGravity, falling;
        float jumpVelocity, fallVelocity;
        float initialY;
        float xDirection, prevXDirection, yDirection;
        float prevXPos, prevYPos;
        float acceleration, accelRate, maxAcceleration, deceleration;
        float gravity;

        double aggroRadius; // Will be used to initiate enemy AI sequence when player gets close enough
        double meleeRange;

        // x Movement
        void StartMove(float dir);
        void SlowDown();

        // These functions may be different between different types of enemies. So we'll make them virtual for overriding.
        virtual void MoveLeft() = 0;
        virtual void MoveRight() = 0;
        virtual void StopMove() = 0;


        // y movement
        void ApplyGravity();
        void Jump();
        void StartJump();

        int hp;
        bool isDying;
        void Die();

        bool isAttacking;

        virtual void AIRoutine() = 0;

        int idleFrame, dyingFrame, jumpFrame, moveFrame, attackFrame;
        int actionTrigger;
        int moveSpeed, jumpSpeed;
        Timer* frameTimer;
        TextureLoader moveAnim[10];
        TextureLoader idleAnim[10];
        TextureLoader jumpAnim[10];
        TextureLoader deathAnim[10];
        TextureLoader attackAnim[10];
        int maxIdleFrame, maxMoveFrame, maxJumpFrame, maxDeathFrame, maxAttackFrame;
        void Actions(int);

        virtual bool CheckCollision() = 0;
        virtual bool CheckCircleCollision() = 0;
        virtual bool CheckCircleSquareCollision() = 0;
        virtual bool CheckForwardCollision() = 0;
        virtual bool CheckForPit() = 0; // will be implemented in melee player for checking if they should jump over a pit.

        bool PlayerInRange(double checkRadius); // pass radius values into this function to determine if player is within that radius. E.G: Aggro or melee range

        AudioSource* sound;
    private:



};

#endif // ENEMY_H
