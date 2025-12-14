#pragma once
#include <cmath>
#include <SDL3/SDL.h>

class Vector2D
{
public:
	Vector2D(float setX, float setY) : x(setX), y(setY) {}
	Vector2D(const Vector2D& other) : x(other.x), y(other.y) {}
	Vector2D(float angleRad) : x(cos(angleRad)), y(sin(angleRad)) {}
	Vector2D() : x(0.0f), y(0.0f) {}

	float angle() { return atan2(y, x); }

	float magnitude() { return sqrt(x * x + y * y); }
	Vector2D getNegativeReciprocal() { return Vector2D(-y, x); }

	float dot(const Vector2D& other) { return x * other.x + y * other.y; }
	float cross(const Vector2D& other) { return x * other.y - y * other.x; }
	float angle_between(const Vector2D& other) { return atan2(cross(other), dot(other)); }

	static float radians_to_degrees(float radians) {
		// Standard Pi constant (may require #define _USE_MATH_DEFINES on some compilers)
		return radians * (180.0 / M_PI);
	}

	Vector2D operator+(const float amount) { return Vector2D(x + amount, y + amount); }
	Vector2D operator-(const float amount) { return Vector2D(x - amount, y - amount); }
	Vector2D operator*(const float amount) { return Vector2D(x * amount, y * amount); }
	Vector2D operator/(const float amount) { return Vector2D(x / amount, y / amount); }

	Vector2D operator+(const Vector2D& other) { return Vector2D(x + other.x, y + other.y); }
	Vector2D operator-(const Vector2D& other) const { return Vector2D(x - other.x, y - other.y); }
	Vector2D operator*(const Vector2D& other) { return Vector2D(x * other.x, y * other.y); }
	Vector2D operator/(const Vector2D& other) { return Vector2D(x / other.x, y / other.y); }

	Vector2D& operator+=(const float amount) { x += amount; y += amount; return *this; }
	Vector2D& operator-=(const float amount) { x -= amount; y -= amount; return *this; }
	Vector2D& operator*=(const float amount) { x *= amount; y *= amount; return *this; }
	Vector2D& operator/=(const float amount) { x /= amount; y /= amount; return *this; }

	Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
	Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
	Vector2D& operator*=(const Vector2D& other) { x *= other.x; y *= other.y; return *this; }
	Vector2D& operator/=(const Vector2D& other) { x /= other.x; y /= other.y; return *this; }
    
    Vector2D normalize() {
        float magnitudeStored = magnitude();
        if (magnitudeStored > 0.0f) {
            x /= magnitudeStored;
            y /= magnitudeStored;
        }

        return *this;
    }
	SDL_FPoint get_sdl_point() const { return {x, y}; }
    float x, y;
};