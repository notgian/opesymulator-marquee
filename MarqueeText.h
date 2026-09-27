#pragma once

#include "Component.h"
#include <chrono>
#include <string>

class MarqueeText : public Component {
    public:
        MarqueeText(int boundsRow, int boundsCol, int boundsWidth, int boundsHeight, const std::string& marqueeText = "", int speedMs = 100);

        void start();
        void stop();
        bool isRunning() const;

        void setMarqueeText(const std::string& marqueeText);
        std::string getMarqueeText() const;

        void setSpeed(int speedMs);
        int getSpeed() const;

        void setBounds(int boundsRow, int boundsCol, int boundsWidth, int boundsHeight);
        int getBoundsRow() const;
        int getBoundsCol() const;
        int getBoundsWidth() const;
        int getBoundsHeight() const;

        bool update();

    private:
        using Clock = std::chrono::steady_clock;

        void rebuildText();
        void clampIntoBounds();
        void moveOneStep();

        std::string marqueeText_;
        int boundsRow_;
        int boundsCol_;
        int boundsWidth_;
        int boundsHeight_;
        int directionX_;
        int directionY_;
        int speedMs_;
        bool running_;
        Clock::time_point lastStepTime_;
};
