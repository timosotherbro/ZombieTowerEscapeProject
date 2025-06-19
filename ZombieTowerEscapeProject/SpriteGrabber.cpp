#include "SpriteGrabber.h"
#include <allegro5/allegro.h>
#include <iostream>

SpriteGrabber::SpriteGrabber(ALLEGRO_BITMAP* sheet, int frameWidth, int frameHeight, int columns, int totalFrames, const std::vector<int>& allowedRows) {
    int totalCols = columns;
    int framesAdded = 0;

    for (int i = 0; i < totalFrames; ++i) {
        int col = i % totalCols;
        int row = i / totalCols;

        // Only proceed if the current row is allowed
        if (std::find(allowedRows.begin(), allowedRows.end(), row) == allowedRows.end())
            continue;

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
