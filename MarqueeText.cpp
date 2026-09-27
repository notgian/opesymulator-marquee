#include "MarqueeText.h"
#include <algorithm>

MarqueeText::MarqueeText(int row, int col, int width, int height, const std::string& marqueeText, int speedMs)
    : Component(row, col, width, height),
      marqueeText_(marqueeText),
      scrollOffset_(0),
      speedMs_(std::max(1, speedMs)),
      running_(false),
      lastStepTime_(Clock::now()) {
    rebuildFrame();
}

void MarqueeText::start() {
    if (running_) return;

    running_ = true;
    lastStepTime_ = Clock::now();
}

void MarqueeText::stop() {
    running_ = false;
}

bool MarqueeText::isRunning() const {
    return running_;
}

void MarqueeText::setMarqueeText(const std::string& marqueeText) {
    marqueeText_ = marqueeText;
    scrollOffset_ = 0;
    rebuildFrame();
}

std::string MarqueeText::getMarqueeText() const {
    return marqueeText_;
}

void MarqueeText::setSpeed(int speedMs) {
    speedMs_ = std::max(1, speedMs);
}

int MarqueeText::getSpeed() const {
    return speedMs_;
}

bool MarqueeText::update() {
    if (!running_) return false;

    size_t trackLength = getTrackLength();
    if (trackLength == 0) return false;

    Clock::time_point now = Clock::now();
    long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStepTime_).count();
    if (elapsedMs < speedMs_) return false;

    long long stepsToTake = elapsedMs / speedMs_;
    lastStepTime_ += std::chrono::milliseconds(stepsToTake * speedMs_);
    scrollOffset_ = (scrollOffset_ + stepsToTake) % trackLength;

    rebuildFrame();
    return true;
}

void MarqueeText::rebuildFrame() {
    int safeWidth = std::max(0, width_);
    int safeHeight = std::max(0, height_);

    text_.assign(safeHeight, std::string(safeWidth, ' '));
    if (safeWidth == 0 || safeHeight == 0) return;

    std::string track = std::string(safeWidth, ' ') + marqueeText_;
    size_t trackLength = track.size();
    int marqueeRow = safeHeight / 2;

    for (int column = 0; column < safeWidth; ++column) {
        text_[marqueeRow][column] = track[(scrollOffset_ + column) % trackLength];
    }
}

size_t MarqueeText::getTrackLength() const {
    return static_cast<size_t>(std::max(0, width_)) + marqueeText_.size();
}
