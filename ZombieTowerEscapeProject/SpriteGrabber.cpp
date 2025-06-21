#include "SpriteGrabber.h"
#include <allegro5/allegro.h>
#include <iostream>
#include <algorithm>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


SpriteGrabber::SpriteGrabber(ALLEGRO_BITMAP* sheet, int frameWidth, int frameHeight, int columns, int totalFrames,
    const std::vector<int>& allowedRows)
    : SpriteGrabber(sheet, frameWidth, frameHeight, columns, totalFrames, allowedRows, std::vector<int>()) {
}


SpriteGrabber::SpriteGrabber(ALLEGRO_BITMAP* sheet, int frameWidth, int frameHeight, int columns, int totalFrames,
    const std::vector<int>& allowedRows, const std::vector<int>& allowedCols)
{
    // existing constructor code


    int totalCols = columns;
    int framesAdded = 0;

    for (int i = 0; i < totalFrames; ++i) {
        int col = i % totalCols;
        int row = i / totalCols;

        if (std::find(allowedRows.begin(), allowedRows.end(), row) == allowedRows.end()) {
            continue;
        }
            
        if (!allowedCols.empty() && std::find(allowedCols.begin(), allowedCols.end(), col) == allowedCols.end()) {
            continue;
        }


        int x = col * frameWidth;
        int y = row * frameHeight;

        ALLEGRO_BITMAP* frame = al_create_bitmap(frameWidth, frameHeight);
        if (!frame) {
            std::cerr << "Failed to create frame bitmap at index " << i << "\n";
            continue;
        }

        al_set_target_bitmap(frame);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0)); // transparent background
        al_draw_bitmap_region(sheet, x, y, frameWidth, frameHeight, 0, 0, 0);

        frames.push_back(frame);
        framesAdded++;
    }

    al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
    std::cout << "SpriteGrabber: " << framesAdded << " frame(s) loaded from allowed rows.\n";
}

SpriteGrabber::~SpriteGrabber() {
    for (auto f : frames) {
        al_destroy_bitmap(f);
    }
}


ALLEGRO_BITMAP* SpriteGrabber::getFrame(int index) {
    if (index >= 0 && index < static_cast<int>(frames.size())) {
        return frames[index];
    }
    return nullptr;
}