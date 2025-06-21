#pragma once
#include <allegro5/allegro.h>
#include <vector>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


class SpriteGrabber {
public:
    SpriteGrabber(ALLEGRO_BITMAP* sheet, int frameWidth, int frameHeight, int columns, int totalFrames,
        const std::vector<int>& allowedRows);

    
    SpriteGrabber(ALLEGRO_BITMAP* sheet, int frameWidth, int frameHeight, int columns, int totalFrames,
        const std::vector<int>& allowedRows, const std::vector<int>& allowedCols);

    ~SpriteGrabber();

    int getTotalFrames() const { 
        return static_cast<int>(frames.size());
    }



    ALLEGRO_BITMAP* getFrame(int index);

private:
    std::vector<ALLEGRO_BITMAP*> frames;
};
