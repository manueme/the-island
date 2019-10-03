#pragma once
#include "Component.h"
#include <ctime>
#include <glm/gtc/quaternion.hpp>
#include <assimp/anim.h>

class ObjectAnimation : public Component {
public:
    ObjectAnimation(aiNodeAnim* animationData, double duration, double ticksPerSecond, const shared_ptr<GameObject>& parent, bool startPlaying = true);
    ComponentKey getComponentKey() override;
    ~ObjectAnimation();
    double duration;
    double ticksPerSecond;
    aiNodeAnim* aiAnimationData;

    void update() override;

    glm::quat calcInterpolatedRotation(double animationTime) const;
    glm::vec3 calcInterpolatedScaling(double animationTime) const;
    glm::vec3 calcInterpolatedPosition(double animationTime) const;
    int findRotation(double animationTime) const;
    unsigned int findScaling(double animationTime) const;
    unsigned int findPosition(double animationTime) const;
    double getCurrentAnimationTime() const;
    bool isPlaying() const;
    void pauseAnimation();
    void playAnimation();
    void setLoop(bool loop);
    void setAnimationTime(float start, float end);
private:
    bool started = false;
    clock_t timeStamp;
    bool loop;
    double currentAnimationTime;
    bool play;
    double animationTimeStart;
    double animationTimeEnd;
};
