//
// Created by Dwayne on 20/11/24.
//

#include "Audio.h"

Audio::Audio(PlayStatus *playStatus) {
    this->playStatus = playStatus;
    queue = new Queue(playStatus);
}

Audio::~Audio() {

}
