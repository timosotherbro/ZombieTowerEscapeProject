#pragma once
#include <string>  
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

    void setOnLadder(bool value) {
        onLadder = value; 
    }

    bool isOnLadder() const { 
        return onLadder;
    }





private:
    float x, y;
    bool isMoving;
    bool facingRight;
    bool isJumping;

    bool onLadder = false;

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
    float gravity = 0.45f;
    float maxFallSpeed = 12.0f;
    bool onGround = false;

    float climbSpeed = 2.0f;


    int currentFrame = 0;
    int animCounter = 0;
    std::string state;

    int mapxoff = 0;
    int mapyoff = 0;

    enum KeyIndex {
        LEFT = 0,
        RIGHT = 1,
        UP = 2,
        DOWN = 3
    };


    bool keys[4] = { false, false, false, false };

    std::vector<ALLEGRO_BITMAP*> rightSlashFrames;
    std::vector<ALLEGRO_BITMAP*> leftSlashFrames;

    std::vector<ALLEGRO_BITMAP*> rightJumpFrames;
    std::vector<ALLEGRO_BITMAP*> leftJumpFrames;

    std::vector<ALLEGRO_BITMAP*> leftIdleFrames;
    std::vector<ALLEGRO_BITMAP*> rightIdleFrames;

    std::vector<ALLEGRO_BITMAP*> climbFrames;


    SpriteGrabber* combatIdleGrabber;
    SpriteGrabber* jumpGrabber;
    
    



    ALLEGRO_BITMAP* runImage;
    ALLEGRO_BITMAP* jumpImage;
    ALLEGRO_BITMAP* slashImage;
    ALLEGRO_BITMAP* slashReverseImage;
    ALLEGRO_BITMAP* hurtImage;


    SpriteGrabber* runGrabber;
};
