#pragma once
#include <allegro5/allegro.h>
#include "SpriteGrabber.h"

class Sprite {
public:
    Sprite();
    ~Sprite();

    void Init(float startX, float startY);
    void Update(bool isMoving, bool facingRight);
    void Draw(int xOffset, int yOffset);

    void setJumping(bool jumping) { 
        isJumping = jumping; 
    }

    


    void setAttacking(bool a) { 
        isAttacking = a;
    }



    float getX() const {
        return x; 
    }

    float getY() const {
        return y;
    }

    void setX(float newX) {
        x = newX; 
    }

    void setY(float newY) {
        y = newY; 
    }


    int getWidth() const {
        return frameWidth;
    }

    int getHeight() const {
        return frameHeight; 
    }


    void setOnGround(bool grounded) {
        onGround = grounded;
        if (grounded) yVelocity = 0;  // stop falling if grounded
    }

    void startJump() {
        if (onGround) {
            yVelocity = jumpStrength;
            onGround = false;
            isJumping = true;
        }
    }


    float getYVelocity() const { 
        return yVelocity;

    }
    void setYVelocity(float vy) {
        yVelocity = vy;
    }

    bool isOnGround() const { 
        return onGround; 
    }

    




private:
    float x, y;
    bool isMoving;
    bool facingRight;
    bool isJumping;

    // In private section
    bool jumpPressed = false;
    float jumpStrength = -16; // Negative = upward in screen coords


    int frameWidth, frameHeight;
    int curFrame, maxFrame;
    int frameCount, frameDelay;
    int animationColumns;

    bool lastDirectionRight;

    bool isAttacking;

    float yVelocity = 0.0f;
    float gravity = 0.3f;
    float maxFallSpeed = 12.0f;
    bool onGround = false;


    std::vector<ALLEGRO_BITMAP*> rightSlashFrames;
    std::vector<ALLEGRO_BITMAP*> leftSlashFrames;

    std::vector<ALLEGRO_BITMAP*> rightJumpFrames;
    std::vector<ALLEGRO_BITMAP*> leftJumpFrames;



    SpriteGrabber* combatIdleGrabber;
    SpriteGrabber* jumpGrabber;
    
    



    ALLEGRO_BITMAP* runImage;
    ALLEGRO_BITMAP* jumpImage;
    ALLEGRO_BITMAP* slashImage;
    ALLEGRO_BITMAP* slashReverseImage;
    ALLEGRO_BITMAP* hurtImage;


    SpriteGrabber* runGrabber;
};
