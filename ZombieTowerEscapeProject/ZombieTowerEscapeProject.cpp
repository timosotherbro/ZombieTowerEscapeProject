//Charles Stoeter Final Project: Zombie Tower Escape

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include "mappy_A5.h"
#include "Sprite.h"
#include <iostream>
#include <sstream> 
#include "SpriteGrabber.h"



const int WIDTH = 900;
const int HEIGHT = 600;

int mapxoff = 0;
int mapyoff = 0;


int main() {
    if (!al_init()) return -1;
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    Sprite player;

    const int TILE_SIZE = 64;  
    int spawnTileX = 2;
    int spawnTileY = 22;

    int startX = spawnTileX * TILE_SIZE;
    int startY = spawnTileY * TILE_SIZE;

    
    player.Init(startX, startY);


    int currentLevel = 1;
    char levelName[] = "zombietower1.fmp";
    if (MapLoad(levelName, 1)) {
        al_show_native_message_box(display, "Error", "Map Load", "Could not load zombietower1.fmp", NULL, 0);
        return -1;
    }

    bool keys[4] = { false, false, false, false }; // LEFT, RIGHT, UP, DOWN
    enum { LEFT, RIGHT, UP, DOWN };
    bool running = true;
    bool redraw = true;
    double startTime = al_get_time();

    const double TIME_LIMIT = 60.0;
    ALLEGRO_FONT* font = al_load_ttf_font("ARIAL.TTF", 24, 0);
    if (!font) {
        al_show_native_message_box(display, "Error", "Font Load", "Could not load font!", NULL, 0);
        return -1;
    }


    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {



            running = false;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Movement

            MapUpdateAnims();


            float x = player.getX();
            float y = player.getY();

            bool isMoving = false;
            bool faceRight = true;

            float newX = x;
            float newY = y;

            // Predict next position
            if (keys[LEFT]) {
                newX -= 2;
                isMoving = true;
                faceRight = false;
            }
            if (keys[RIGHT]) {
                newX += 2;
                isMoving = true;
                faceRight = true;
            }
            if (keys[UP]) {
               
                
            }
            if (keys[DOWN]) {
                
            }

            MapChangeLayer(1);

            auto isBlockedAt = [](float px, float py) {
                int tileX = px / mapblockwidth;
                int tileY = py / mapblockheight;
                BLKSTR* block = MapGetBlock(tileX, tileY);



                return block && (block->tl || block->tr || block->bl || block->br);
                };

            bool blocked =
                isBlockedAt(newX, newY) ||  // top-left
                isBlockedAt(newX + player.getWidth() - 1, newY) ||  // top-right
                isBlockedAt(newX, newY + player.getHeight() - 1) ||  // bottom-left
                isBlockedAt(newX + player.getWidth() - 1, newY + player.getHeight() - 1);  // bottom-right



            if (!blocked) {
                player.setX(newX);  // Handle horizontal movement only here
            }




            // -- Check if standing on solid ground --
            // Apply gravity logic using predictedY
            // Gravity-based movement
            float velocityY = player.getYVelocity();
            float predictedY = player.getY() + velocityY;

            // Headbutt check (moving up)
            if (velocityY < 0) {
                bool hitCeiling =
                    isBlockedAt(player.getX() + 2, predictedY) ||
                    isBlockedAt(player.getX() + player.getWidth() - 3, predictedY);

                if (hitCeiling) {
                    player.setYVelocity(0);

                    float ceilingY = ((int)(player.getY() / mapblockheight)) * mapblockheight;
                    player.setY(ceilingY + 1);  // Push slightly down so they don't get stuck
                }
                else {
                    player.setY(predictedY);
                    player.setOnGround(false);
                }

            }
            // Falling or on air
            else if (velocityY > 0 || !player.isOnGround()) {
                bool canFall =
                    !isBlockedAt(player.getX() + 2, predictedY + player.getHeight()) &&
                    !isBlockedAt(player.getX() + player.getWidth() - 3, predictedY + player.getHeight());

                if (canFall) {
                    player.setY(predictedY);
                    player.setOnGround(false);
                }
                else {
                    // Landed
                    player.setOnGround(true);
                    player.setYVelocity(0);
                    float snappedY = ((int)((player.getY() + player.getHeight()) / mapblockheight)) * mapblockheight - player.getHeight();
                    player.setY(snappedY);
                    player.setJumping(false);
                }
            }


        


            


            


            mapxoff = player.getX() + player.getWidth() / 2 - WIDTH / 2;
            mapyoff = player.getY() + player.getHeight() / 2 - HEIGHT / 2;

            // Clamp camera to map boundaries
            if (mapxoff < 0) mapxoff = 0;
            if (mapyoff < 0) mapyoff = 0;
            if (mapxoff > mapwidth * mapblockwidth - WIDTH) mapxoff = mapwidth * mapblockwidth - WIDTH;
            if (mapyoff > mapheight * mapblockheight - HEIGHT) mapyoff = mapheight * mapblockheight - HEIGHT;


            

            

            

            // Update animation
            player.Update(isMoving, faceRight);

            double timeElapsed = al_get_time() - startTime;
            if (timeElapsed >= TIME_LIMIT) {
                std::cout << "Time's up! Game Over.\n";
                al_rest(2.0); // short pause to show message

                if (timeElapsed >= TIME_LIMIT) {
                    std::cout << "Time's up! Game Over.\n";
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font, al_map_rgb(255, 0, 0), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                    al_flip_display();
                    al_rest(2.0);
                    running = false;
                    continue;
                }


                running = false;
                continue;
            }


            // Collision with end block


            MapChangeLayer(1);

            auto isAtGoal = [](float px, float py) {
                int tileX = px / mapblockwidth;
                int tileY = py / mapblockheight;
                BLKSTR* b = MapGetBlock(tileX, tileY);
                if (b) {
                    std::cout << "Goal Check: tile (" << tileX << "," << tileY << ") user1=" << b->user1 << std::endl;
                    return b->user1 == 8;
                }
                return false;
                };

            // check all 4 corners of sprite
            float centerX = player.getX() + player.getWidth() / 2;
            float bottomY = player.getY() + player.getHeight(); // instead of -2


            bool atGoal = isAtGoal(centerX, bottomY);



            if (atGoal) {

                double time = al_get_time() - startTime;
                std::cout << "Level " << currentLevel << " Complete in " << time << " seconds!\n";
                al_rest(3.0);

                currentLevel++;
                if (currentLevel > 3) {
                    running = false;
                    continue;
                }

                std::stringstream ss;
                ss << "zombietower" << currentLevel << ".fmp";
                std::string nextMap = ss.str();


                if (MapLoad(const_cast<char*>(nextMap.c_str()), 1)) {
                    al_show_native_message_box(display, "Error", "Map Load", "Could not load next map", NULL, 0);
                    running = false;
                }
                player.setX(100); player.setY(100);
                startTime = al_get_time();
            }

            redraw = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE: running = false; break;
            case ALLEGRO_KEY_LEFT: keys[LEFT] = true; break;
            case ALLEGRO_KEY_RIGHT: keys[RIGHT] = true; break;
            case ALLEGRO_KEY_UP:
                keys[UP] = true;
                player.startJump(); // <-- triggers jump if on ground
                break;
            case ALLEGRO_KEY_DOWN: keys[DOWN] = true; break;
            case ALLEGRO_KEY_SPACE:
                player.setAttacking(true);
                break;
            }
}

        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_LEFT: keys[LEFT] = false; break;
            case ALLEGRO_KEY_RIGHT: keys[RIGHT] = false; break;
            case ALLEGRO_KEY_UP: keys[UP] = false; break;
            case ALLEGRO_KEY_DOWN: keys[DOWN] = false; break;
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            // Draw Layer 0 (dark green background)
            MapChangeLayer(0);
            MapDrawBG(mapxoff, mapyoff, 0, 0, WIDTH, HEIGHT);

            MapChangeLayer(1);
            MapDrawBG(mapxoff, mapyoff, 0, 0, WIDTH, HEIGHT);

            // Restore to Layer 0 for things like collision logic
            MapChangeLayer(0);

            // Draw the player sprite
            player.Draw(mapxoff, mapyoff);


            // Draw the timer
            double timeLeft = TIME_LIMIT - (al_get_time() - startTime);
            if (timeLeft < 0) timeLeft = 0;

            std::stringstream timeText;
            timeText << "Time Left: " << static_cast<int>(timeLeft);
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, timeText.str().c_str());

            // Flip the buffer and clear the screen
            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
    }

    MapFreeMem();
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_font(font);



    return 0;
}
