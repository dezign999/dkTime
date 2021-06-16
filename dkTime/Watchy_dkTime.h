#ifndef WATCHY_DKTIME_H
#define WATCHY_DKTIME_H

#include "Watchy_Base.h"
#include "dkong.h"

class WatchyDkTime : public WatchyBase{
    public:
        WatchyDkTime();
        void drawWatchFace();
        void drawAnim();
};

#endif
