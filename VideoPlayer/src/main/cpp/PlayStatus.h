//
// Created by Dwayne on 20/11/24.
//

#ifndef AUDIO_PRACTICE_PLAYSTATUS_H
#define AUDIO_PRACTICE_PLAYSTATUS_H


class PlayStatus {

public:
    bool exit;
    bool load = true;
    bool seek = false;
    bool pause = false;

public:
    PlayStatus();
    ~PlayStatus();
};


#endif //AUDIO_PRACTICE_PLAYSTATUS_H
