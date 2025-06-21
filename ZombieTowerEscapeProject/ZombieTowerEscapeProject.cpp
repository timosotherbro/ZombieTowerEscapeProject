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
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>




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
    
    
    
    if (!al_install_audio()) {
        std::cerr << "Failed to install audio!\n";
        return -1;
    }
    if (!al_init_acodec_addon()) {
        std::cerr << "Failed to initialize acodec!\n";
        return -1;
    }
    if (!al_reserve_samples(8)) {
        std::cerr << "Failed to reserve samples!\n";
        return -1;
    }


    ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
    if (!display) {
        al_show_native_message_box(NULL, "Error", "Display Error", "Failed to create display!", NULL, 0);
        return -1;
    }

    ALLEGRO_SAMPLE* bgMusic = al_load_sample("backgroundmusic.ogg");
    ALLEGRO_SAMPLE_ID bgMusicID;

    


    
    if (!bgMusic) {
        al_show_native_message_box(display, "Error", "Audio Load", "Could not load backgroundmusic.ogg", NULL, 0);
        return -1;
    }

    bool success = al_play_sample(bgMusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &bgMusicID);
    if (!success) {
        std::cout << "Failed to play sample\n";
    }


    

    ALLEGRO_SAMPLE* victorySound = al_load_sample("victory.wav");
    if (!victorySound) {
        al_show_native_message_box(display, "Error", "Audio Load", "Could not load victory.wav", NULL, 0);
        return -1;
    }

    ALLEGRO_SAMPLE* doorSound = al_load_sample("door.wav");
    if (!doorSound) {
        al_show_native_message_box(display, "Error", "Audio Load", "Could not load door.wav", NULL, 0);
        return -1;
    }

    ALLEGRO_SAMPLE* keySound = al_load_sample("key.wav");
    if (!keySound) {
        al_show_native_message_box(display, "Error", "Audio Load", "Could not load key.wav", NULL, 0);
        return -1;
    }















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

    int playerHealth = 5;

    
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

    bool gameCompleted = false;
    std::vector<double> levelTimes;


    bool hasKey = false;
    double startTime = al_get_time();

    const double TIME_LIMIT = 60.0;
    ALLEGRO_FONT* font = al_load_ttf_font("toxia.ttf", 24, 0);
    if (!font) {
        al_show_native_message_box(display, "Error", "Font Load", "Could not load font!", NULL, 0);
        return -1;
    }



    bool inIntro = true;

    while (inIntro) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 80, ALLEGRO_ALIGN_CENTER, "TOWER ESCAPE");
        al_draw_text(font, al_map_rgb(180, 180, 180), WIDTH / 2, 140, ALLEGRO_ALIGN_CENTER, "Controls:");
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 180, ALLEGRO_ALIGN_CENTER, "Left Arrow     Move Left");
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 220, ALLEGRO_ALIGN_CENTER, "Right Arrow     Move Right");
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 260, ALLEGRO_ALIGN_CENTER, "Up Arrow     Jump or Ascend Ladder");
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 300, ALLEGRO_ALIGN_CENTER, "Down Arrow     Descend Ladder");
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 360, ALLEGRO_ALIGN_CENTER, "Collect the key and escape the tower!");
        al_draw_text(font, al_map_rgb(255, 255, 0), WIDTH / 2, 420, ALLEGRO_ALIGN_CENTER, "Press ENTER to Start");

        al_flip_display();
        al_rest(0.1);

        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
            inIntro = false;
            al_start_timer(timer); // start the game timer AFTER intro
            startTime = al_get_time(); // reset timing

        }
    }


    

    

    

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {



            running = false;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Movement

            MapUpdateAnims();

            auto isLadderTile = [](float px, float py) -> bool {
                int tileX = px / mapblockwidth;
                int tileY = py / mapblockheight;
                BLKSTR* b = MapGetBlock(tileX, tileY);
                return b && b->user1 == 5;
                };

            

            float x = player.getX();
            float y = player.getY();

            bool isMoving = false;
            bool faceRight = true;

            float newX = x;
            float newY = y;

            const float MOVE_SPEED = 4.5f;

            if (keys[LEFT]) {
                newX -= MOVE_SPEED;
                isMoving = true;
                faceRight = false;
            }
            if (keys[RIGHT]) {
                newX += MOVE_SPEED;
                isMoving = true;
                faceRight = true;
            }

            if (keys[UP]) {
               
                
            }
            if (keys[DOWN]) {
                
            }

            if (player.isOnLadder()) {
                // Check if still on ladder
                float px = player.getX() + player.getWidth() / 2;
                float pyTop = player.getY();
                float pyBottom = player.getY() + player.getHeight();

                // Exit ladder if top and bottom both aren't on ladder tiles
                if (!isLadderTile(px, pyTop) && !isLadderTile(px, pyBottom)) {
                    player.setOnLadder(false);
                }


                player.setYVelocity(0); // Cancel gravity
                if (keys[UP]) {
                    newY -= MOVE_SPEED;
                }
                if (keys[DOWN]) {
                    newY += MOVE_SPEED;
                }
                player.setY(newY); // Apply vertical movement
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
            // Apply vertical movement with proper collision
            if (velocityY < 0) {
                // HEAD COLLISION CHECK (moving upward)
                bool hitCeiling =
                    isBlockedAt(player.getX() + 2, predictedY) ||
                    isBlockedAt(player.getX() + player.getWidth() - 3, predictedY);

                if (hitCeiling) {
                    player.setYVelocity(0);

                    // Snap to just below the ceiling tile
                    float ceilingTileY = ((int)((predictedY) / mapblockheight) + 1) * mapblockheight;
                    player.setY(ceilingTileY);
                    player.setJumping(false);
                }
                else {
                    player.setY(predictedY);
                    player.setOnGround(false);
                }
            }
            else if (!player.isOnGround() && !player.isOnLadder()) {
                // FALLING CHECK
                bool canFall =
                    !isBlockedAt(player.getX() + 2, predictedY + player.getHeight()) &&
                    !isBlockedAt(player.getX() + player.getWidth() - 3, predictedY + player.getHeight());

                if (canFall) {
                    player.setY(predictedY);
                    player.setOnGround(false);
                }
                else {
                    // LANDING: Snap to top of tile
                    player.setOnGround(true);
                    player.setYVelocity(0);

                    float snappedY = ((int)((predictedY + player.getHeight()) / mapblockheight)) * mapblockheight - player.getHeight();
                    player.setY(snappedY);
                    player.setJumping(false);
                }
            }

            // Falling or on air
            else if (!player.isOnGround() && !player.isOnLadder()) {
                bool canFall =
                    !isBlockedAt(player.getX() + 2, predictedY + player.getHeight()) &&
                    !isBlockedAt(player.getX() + player.getWidth() - 3, predictedY + player.getHeight());

                if (canFall) {
                    player.setY(predictedY);
                    player.setOnGround(false);
                }
                else {
                    player.setOnGround(true);
                    player.setYVelocity(0);

                    float snappedY = floorf((predictedY + player.getHeight()) / mapblockheight) * mapblockheight - player.getHeight();
                    const float SNAP_TOLERANCE = 3.0f;

                    if (fabs(predictedY - snappedY) <= SNAP_TOLERANCE) {
                        player.setY(snappedY);
                    }
                    else {
                        player.setY(predictedY);
                    }

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

            


            auto getUser1At = [](float px, float py) -> int {
                int tileX = px / mapblockwidth;
                int tileY = py / mapblockheight;
                BLKSTR* b = MapGetBlock(tileX, tileY);
                if (b) return b->user1;
                return -1;  // means no block or no user1
            };


            // check all 4 corners of sprite
            float centerX = player.getX() + player.getWidth() / 2;
            float bottomY = player.getY() + player.getHeight();

            int user = getUser1At(centerX, bottomY);

            switch (user) {
            case 3: // Spike
                std::cout << "Player hit spike! Losing health...\n";
                playerHealth--;

                if (playerHealth <= 0) {
                    std::cout << "Player died! Game Over.\n";
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    al_draw_text(font, al_map_rgb(255, 0, 0), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "YOU DIED");
                    al_flip_display();
                    al_rest(2.0);
                    running = false;
                    break;
                }

                // Respawn
                player.setX(startX);
                player.setY(startY);
                startTime = al_get_time();

                break;

            case 5: // Ladder
                
                player.setOnLadder(true);
                break;

            case 8: { // Door to next level
                if (!hasKey) {
                    std::cout << "Door is locked! Find the key.\n";
                    break;
                }

                std::cout << "Reached exit!\n";
                double time = al_get_time() - startTime;
                std::cout << "Level " << currentLevel << " Complete in " << time << " seconds!\n";
                al_play_sample(doorSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
                al_rest(3.0);
                levelTimes.push_back(time);

                currentLevel++;
                if (currentLevel > 3) {
                    gameCompleted = true;
                    running = false;
                    break;
                }

                std::stringstream ss;
                ss << "zombietower" << currentLevel << ".fmp";
                std::string nextMap = ss.str();

                if (MapLoad(const_cast<char*>(nextMap.c_str()), 1)) {
                    al_show_native_message_box(display, "Error", "Map Load", "Could not load next map", NULL, 0);
                    running = false;
                    break;
                }

                int spawnTileX = 2;
                int spawnTileY = 22;

                if (currentLevel == 2) {
                    spawnTileX = 2;
                    spawnTileY = 22;
                }
                else if (currentLevel == 3) {
                    spawnTileX = 2;
                    spawnTileY = 22;
                }

                int spawnX = spawnTileX * TILE_SIZE;
                int spawnY = spawnTileY * TILE_SIZE;

                player.setX(spawnX);
                player.setY(spawnY);


                startTime = al_get_time();
                hasKey = false; // Reset key for next level
                break;
            }

            case 9: // Key
                std::cout << "Picked up key!\n";
                hasKey = true;

                al_play_sample(keySound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);


                // Clear the key tile visually and logically
                {
                    int tileX = centerX / mapblockwidth;
                    int tileY = bottomY / mapblockheight;
                    BLKSTR* block = MapGetBlock(tileX, tileY);
                    if (block) {
                        block->user1 = 0;
                    }
                }
                break;


            default:
                player.setOnLadder(false);
                break;
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


            // ----- Status Bar Background -----
            al_draw_filled_rectangle(0, 0, WIDTH, 60, al_map_rgb(30, 30, 30));

            // ----- Time Left -----
            double timeLeft = TIME_LIMIT - (al_get_time() - startTime);
            if (timeLeft < 0) timeLeft = 0;

            float barWidth = (WIDTH - 400) * (timeLeft / TIME_LIMIT); // Shrink bar over time
            al_draw_filled_rectangle(10, 10, 10 + barWidth, 30, al_map_rgb(0, 200, 0));
            al_draw_rectangle(10, 10, WIDTH - 390, 30, al_map_rgb(255, 255, 255), 2);

            std::stringstream timeText;
            timeText << "Time: " << static_cast<int>(timeLeft) << "s";
            al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH - 360, 10, 0, timeText.str().c_str());

            //  Key Status 
            std::string keyStatus = hasKey ? "Key: YES" : "Key: NO";
            al_draw_text(font, al_map_rgb(255, 255, 0), WIDTH - 240, 10, 0, keyStatus.c_str());

            // Health Bar
            int maxHealth = 5;
            int healthBarWidth = 150;
            int barHeight = 15;
            int barX = 10;
            int barY = 40;

            float healthRatio = static_cast<float>(playerHealth) / maxHealth;
            float currentBarWidth = healthBarWidth * healthRatio;

            al_draw_filled_rectangle(barX, barY, barX + currentBarWidth, barY + barHeight, al_map_rgb(200, 50, 50));
            al_draw_rectangle(barX, barY, barX + healthBarWidth, barY + barHeight, al_map_rgb(255, 255, 255), 2);

            std::stringstream hpText;
            hpText << "HP: " << playerHealth << "/" << maxHealth;
            al_draw_text(font, al_map_rgb(255, 255, 255), barX + healthBarWidth + 10, barY - 2, 0, hpText.str().c_str());



           

            // Flip the buffer and clear the screen
            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
    }



    if (gameCompleted) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_play_sample(victorySound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);


        al_draw_text(font, al_map_rgb(255, 255, 0), WIDTH / 2, 80, ALLEGRO_ALIGN_CENTER, "CONGRATULATIONS!");
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 140, ALLEGRO_ALIGN_CENTER, "You escaped the tower!");

        double totalTime = 0;
        for (size_t i = 0; i < levelTimes.size(); ++i) {
            std::stringstream ss;
            ss << "Level " << (i + 1) << ": " << static_cast<int>(levelTimes[i]) << " seconds";
            al_draw_text(font, al_map_rgb(200, 200, 200), WIDTH / 2, 200 + i * 40, ALLEGRO_ALIGN_CENTER, ss.str().c_str());

            totalTime += levelTimes[i];
        }

        std::stringstream totalSS;
        totalSS << "Total Time: " << static_cast<int>(totalTime) << " seconds";
        al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 200 + levelTimes.size() * 40 + 20, ALLEGRO_ALIGN_CENTER, totalSS.str().c_str());

        al_draw_text(font, al_map_rgb(255, 255, 0), WIDTH / 2, HEIGHT - 100, ALLEGRO_ALIGN_CENTER, "Press ESC to Exit");

        al_flip_display();

        // Wait for ESC key
        bool waiting = true;
        while (waiting) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(queue, &ev);
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                waiting = false;
            }
        }
    }



    MapFreeMem();
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_font(font);

    al_destroy_sample(victorySound);

    al_stop_sample(&bgMusicID);
    al_destroy_sample(bgMusic);

    al_destroy_sample(keySound);
    al_destroy_sample(doorSound);

    


    return 0;
}
