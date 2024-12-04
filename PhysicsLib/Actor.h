// actor.h

#pragma once

#include "../UnitLib/Vector.h"
// #include "../GraphicsLib/graphics.h"
#include "Collision.h"
#include <iostream>
#include <iomanip>
#include <cmath>

enum class ActorState
{
    Moving,
    Colliding,
    Idle
};

template <typename T>
class Actor
{
public:
    Vector2<T> position;
    Vector2<T> velocity;
    AABB boundingBox;
    T rotation;
    T rotationSpeed;
    ActorState state;

    Actor(Vector2<T> pos, Vector2<T> vel, T rot, T rotSpeed)
        : position(pos), velocity(vel), rotation(rot), rotationSpeed(rotSpeed), state(ActorState::Idle)
    {
        // Ensure UpdateBoundingBox is accessible
        boundingBox.width = 1.0;
        boundingBox.height = 1.0;
        boundingBox.x = position.x() - 1.0 / 2;
        boundingBox.y = position.y() - 1.0 / 2;
    }

    /* Graphics

     void Render() const {
         Graphics::DrawRotatedRectangle(
             position.x(), position.y(),
             boundingBox.width, boundingBox.height,
             rotation
         );
     }

     */

    void Update(T deltaTime)
    {
        ApplyRotation(deltaTime);
        Vector2<T> rotatedVelocity = RotateVector(velocity, rotation);
        position += rotatedVelocity * deltaTime;
        UpdateBoundingBox(1.0, 1.0);
        state = ActorState::Moving;
    }

    void ApplyRotation(T deltaTime)
    {
        rotation = std::fmod(rotation + rotationSpeed * deltaTime, 2 * M_PI);
        if (rotation < 0)
            rotation += 2 * M_PI;
    }

    void UpdateBoundingBox(T width, T height)
    {
        boundingBox.width = width;
        boundingBox.height = height;
        boundingBox.x = position.x() - width / 2;
        boundingBox.y = position.y() - height / 2;
    }

    void SetState(ActorState newState)
    {
        state = newState;
        if (state == ActorState::Colliding)
        {
            OnCollision();
        }
    }

    void OnCollision()
    {
        velocity = -velocity * 0.8; // Reverse and dampen
        std::cout << "Collision detected. Velocity reversed and damped.\n";
    }

    void PrintState(int frame) const
    {
        std::cout << "Frame " << frame
                  << ": Actor position: (" << std::fixed << std::setprecision(6)
                  << position.x() << ", " << position.y() << "), "
                  << "rotation: " << std::setprecision(6) << rotation << "\n";
    }

private:
    template <typename U>
    inline Vector2<U> RotateVector(const Vector2<U> &vec, U theta)
    {
        U cosTheta = std::cos(theta);
        U sinTheta = std::sin(theta);

        // Adjust small near-zero values to avoid floating-point errors
        if (std::abs(cosTheta) < 1e-6)
            cosTheta = 0.0;
        if (std::abs(sinTheta) < 1e-6)
            sinTheta = 0.0;

        return Vector2<U>{
            vec.x() * cosTheta - vec.y() * sinTheta,
            vec.x() * sinTheta + vec.y() * cosTheta};
    }
};
