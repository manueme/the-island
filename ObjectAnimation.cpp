#include "ObjectAnimation.h"
#include "GameObject.h"
#include "Transform.h"


ObjectAnimation::ObjectAnimation(aiNodeAnim* animationData, const double duration, const double ticksPerSecond, const shared_ptr<GameObject>& parent, const bool startPlaying)
    : Component("object_animation", parent)
{
    this->duration = duration;
    this->ticksPerSecond = ticksPerSecond > 0 ? ticksPerSecond : 25.0;
    this->aiAnimationData = animationData;

    loop = true;
    currentAnimationTime = 0.0;
    started = false;
    play = startPlaying;

    animationTimeStart = 0.0;
    animationTimeEnd = duration;
}

ComponentKey ObjectAnimation::getComponentKey()
{
    return ObjectAnimationComponent;
}


ObjectAnimation::~ObjectAnimation() = default;

void ObjectAnimation::update()
{
    if (play && aiAnimationData) {
        if (!started) {
            timeStamp = clock();
            started = true;
        }
        const auto elapsedMillis = double(clock() - timeStamp);
        const auto elapsedSecs = elapsedMillis / CLOCKS_PER_SEC;
        const auto currentTicks = elapsedSecs * ticksPerSecond;
        const auto animLength = animationTimeEnd - animationTimeStart;
        if (!loop && currentTicks >= animLength) {
            started = false;
            play = false;
        } else {
            currentAnimationTime = fmod(currentTicks, animLength) + animationTimeStart;

            const auto currentRotation = calcInterpolatedRotation(currentAnimationTime);
            const auto currentPosition = calcInterpolatedPosition(currentAnimationTime);
            auto t = parent->getTransform();
            t->setLocalRotation(currentRotation);
            t->setLocalPosition(currentPosition);
        }
    }
}

glm::quat ObjectAnimation::calcInterpolatedRotation(double animationTime) const
{
    // we need at least two values to interpolate...
    if (aiAnimationData->mNumRotationKeys == 1) {
        const auto aiQuat = aiAnimationData->mRotationKeys[0].mValue;
        glm::quat retQuat;
        retQuat.x = aiQuat.x;
        retQuat.y = aiQuat.y;
        retQuat.z = aiQuat.z;
        retQuat.w = aiQuat.w;
        return retQuat;
    }

    const auto rotationIndex = findRotation(animationTime);
    if (rotationIndex >= 0) {
        const auto nextRotationIndex = static_cast<unsigned>(rotationIndex + 1);
        if (nextRotationIndex < aiAnimationData->mNumRotationKeys) {
            const auto deltaTime = aiAnimationData->mRotationKeys[nextRotationIndex].mTime - aiAnimationData->mRotationKeys[rotationIndex].mTime;
            if (deltaTime > 0) {
                const auto factor = static_cast<float>((animationTime - aiAnimationData->mRotationKeys[rotationIndex].mTime) / deltaTime);
                if (factor >= 0.0f && factor <= 1.0f) {
                    const auto startRotationQ = aiAnimationData->mRotationKeys[rotationIndex].mValue;
                    const auto endRotationQ = aiAnimationData->mRotationKeys[nextRotationIndex].mValue;
                    aiQuaternion aiQuat;
                    aiQuaternion::Interpolate(aiQuat, startRotationQ, endRotationQ, factor);
                    aiQuat = aiQuat.Normalize();
                    glm::quat retQuat;
                    retQuat.x = aiQuat.x;
                    retQuat.y = aiQuat.y;
                    retQuat.z = aiQuat.z;
                    retQuat.w = aiQuat.w;
                    return retQuat;
                }
            }

        }

    }
    return parent->getTransform()->getRotation();
}

glm::vec3 ObjectAnimation::calcInterpolatedScaling(double animationTime) const
{
    if (aiAnimationData->mNumScalingKeys == 1) {
        const auto aiVec3d = aiAnimationData->mScalingKeys[0].mValue;
        return glm::vec3(aiVec3d.x, aiVec3d.y, aiVec3d.z);
    }
    const auto scalingIndex = findScaling(animationTime);
    const auto nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < aiAnimationData->mNumScalingKeys);
    const auto deltaTime = static_cast<float>(aiAnimationData->mScalingKeys[nextScalingIndex].mTime - aiAnimationData->mScalingKeys[
                                                  scalingIndex].mTime);
    const auto factor = static_cast<float>(animationTime - aiAnimationData->mScalingKeys[scalingIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const auto start = aiAnimationData->mScalingKeys[scalingIndex].mValue;
    const auto end = aiAnimationData->mScalingKeys[nextScalingIndex].mValue;
    const auto delta = end - start;
    const auto aiVec3d = start + factor * delta;
    return glm::vec3(aiVec3d.x, aiVec3d.y, aiVec3d.z);
}

glm::vec3 ObjectAnimation::calcInterpolatedPosition(const double animationTime) const
{
    if (aiAnimationData->mNumPositionKeys == 1) {
        const auto aiVec3d = aiAnimationData->mPositionKeys[0].mValue;
        return glm::vec3(aiVec3d.x, aiVec3d.y, aiVec3d.z);
    }

    const auto positionIndex = findPosition(animationTime);
    const auto nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < aiAnimationData->mNumPositionKeys);
    const auto deltaTime = static_cast<float>(aiAnimationData->mPositionKeys[nextPositionIndex].mTime - aiAnimationData->mPositionKeys[
                                                  positionIndex].mTime);
    const auto factor = static_cast<float>(animationTime - aiAnimationData->mPositionKeys[positionIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const auto start = aiAnimationData->mPositionKeys[positionIndex].mValue;
    const auto end = aiAnimationData->mPositionKeys[nextPositionIndex].mValue;
    const auto delta = end - start;
    const auto aiVec3d = start + factor * delta;
    return glm::vec3(aiVec3d.x, aiVec3d.y, aiVec3d.z);
}

int ObjectAnimation::findRotation(const double animationTime) const
{
    if (aiAnimationData->mNumRotationKeys > 0) {
        for (auto i = 0u; i < aiAnimationData->mNumRotationKeys - 1; i++) {
            if (animationTime < aiAnimationData->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }
    }
    return -1;
}

unsigned int ObjectAnimation::findScaling(const double animationTime) const
{
    for (unsigned int i = 0; i < aiAnimationData->mNumScalingKeys - 1; i++) {
        if (animationTime < aiAnimationData->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

unsigned int ObjectAnimation::findPosition(const double animationTime) const
{
    for (unsigned int i = 0; i < aiAnimationData->mNumPositionKeys - 1; i++) {
        if (animationTime > 0 && animationTime < aiAnimationData->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

double ObjectAnimation::getCurrentAnimationTime() const
{
    return currentAnimationTime;
}

bool ObjectAnimation::isPlaying() const
{
    return play;
}

void ObjectAnimation::pauseAnimation()
{
    play = false;
}

void ObjectAnimation::playAnimation()
{
    play = true;
}

void ObjectAnimation::setLoop(const bool loop)
{
    this->loop = loop;
}

void ObjectAnimation::setAnimationTime(const float start, const float end)
{
    animationTimeStart = start;
    animationTimeEnd = end;
}
