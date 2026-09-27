#pragma once

#include "Component.h"
#include <chrono>
#include <string>

class MarqueeText : public Component {
    public:
        MarqueeText(int row, int col, int width, int height, const std::string& marqueeText = "", int speedMs = 100);

        void start();
        void stop();
        bool isRunning() const;

        void setMarqueeText(const std::string& marqueeText);
        std::string getMarqueeText() const;

        void setSpeed(int speedMs);
        int getSpeed() const;

        bool update();
        void rebuildFrame();

    private:
        using Clock = std::chrono::steady_clock;

        size_t getTrackLength() const;

        std::string marqueeText_;
        size_t scrollOffset_;
        int speedMs_;
        bool running_;
        Clock::time_point lastStepTime_;
};
