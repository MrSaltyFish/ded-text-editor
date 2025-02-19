#include "./headers/la.h"

Vec2f vec2fs(float s) { return vec2f(s, s); }

// Function to create a Vec2f from two floats
Vec2f vec2f(float x, float y) {
	return (Vec2f){
		.x = x,
		.y = y,
	};
}

// Function to add two Vec2f vectors
Vec2f vec2f_add(Vec2f a, Vec2f b) { return vec2f(a.x + b.x, a.y + b.y); }

// Function to subtract one Vec2f vector from another
Vec2f vec2f_sub(Vec2f a, Vec2f b) { return vec2f(a.x - b.x, a.y - b.y); }

// Function to multiply a Vec2f vector by a scalar
Vec2f vec2f_mul_scalar(Vec2f v, float scalar) {
	return vec2f(v.x * scalar, v.y * scalar);
}
Vec2f vec2f_mul(Vec2f a, Vec2f b) { return vec2f(a.x * b.x, a.y * b.y); }

// Function to divide a Vec2f vector by a scalar
Vec2f vec2f_div(Vec2f a, Vec2f b) { return vec2f(a.x / b.x, a.y / b.y); }
