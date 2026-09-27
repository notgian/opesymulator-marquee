#include "MarqueeText.h"
#include <algorithm>
#include <sstream>

MarqueeText::MarqueeText(int boundsRow, int boundsCol, int boundsWidth, int boundsHeight, const std::string& marqueeText, int speedMs)
    : Component(boundsRow, boundsCol, 0, 0),
      marqueeText_(marqueeText),
      boundsRow_(boundsRow),
      boundsCol_(boundsCol),
      boundsWidth_(std::max(0, boundsWidth)),
      boundsHeight_(std::max(0, boundsHeight)),
      directionX_(1),
      directionY_(1),
      speedMs_(std::max(1, speedMs)),
      running_(false),
      lastStepTime_(Clock::now()) {
    rebuildText();
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
    rebuildText();
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

void MarqueeText::setBounds(int boundsRow, int boundsCol, int boundsWidth, int boundsHeight) {
    boundsRow_ = boundsRow;
    boundsCol_ = boundsCol;
    boundsWidth_ = std::max(0, boundsWidth);
    boundsHeight_ = std::max(0, boundsHeight);
    clampIntoBounds();
}

int MarqueeText::getBoundsRow() const {
    return boundsRow_;
}

int MarqueeText::getBoundsCol() const {
    return boundsCol_;
}

int MarqueeText::getBoundsWidth() const {
    return boundsWidth_;
}

int MarqueeText::getBoundsHeight() const {
    return boundsHeight_;
}

bool MarqueeText::update() {
    if (!running_) return false;

    Clock::time_point now = Clock::now();
    long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStepTime_).count();
    if (elapsedMs < speedMs_) return false;

    long long stepsToTake = elapsedMs / speedMs_;
    lastStepTime_ += std::chrono::milliseconds(stepsToTake * speedMs_);

    for (long long step = 0; step < stepsToTake; ++step) {
        moveOneStep();
    }
    return true;
}

void MarqueeText::rebuildText() {
    std::vector<std::string> lines;
    std::stringstream textStream(marqueeText_);
    std::string line;

    while (std::getline(textStream, line)) {
        lines.push_back(line);
    }
    if (lines.empty()) lines.push_back("");

    size_t longestLine = 0;
    for (const std::string& textLine : lines) {
        longestLine = std::max(longestLine, textLine.size());
    }
    for (std::string& textLine : lines) {
        textLine.resize(longestLine, ' ');
    }

    text_ = lines;
    width_ = static_cast<int>(longestLine);
    height_ = static_cast<int>(lines.size());
    clampIntoBounds();
}

void MarqueeText::clampIntoBounds() {
    int maxX = std::max(boundsCol_, boundsCol_ + boundsWidth_ - width_);
    int maxY = std::max(boundsRow_, boundsRow_ + boundsHeight_ - height_);

    x_ = std::clamp(x_, boundsCol_, maxX);
    y_ = std::clamp(y_, boundsRow_, maxY);
}

void MarqueeText::moveOneStep() {
    int maxX = boundsCol_ + boundsWidth_ - width_;
    int maxY = boundsRow_ + boundsHeight_ - height_;

    if (maxX > boundsCol_) {
        if (x_ + directionX_ < boundsCol_ || x_ + directionX_ > maxX) directionX_ = -directionX_;
        x_ += directionX_;
    } else {
        x_ = boundsCol_;
    }

    if (maxY > boundsRow_) {
        if (y_ + directionY_ < boundsRow_ || y_ + directionY_ > maxY) directionY_ = -directionY_;
        y_ += directionY_;
    } else {
        y_ = boundsRow_;
    }
}
