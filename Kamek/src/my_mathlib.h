#ifndef _MY_MATHLIB_H_
#define _MY_MATHLIB_H_

#include <common.h>
#include <game.h>

float GenerateRandomNumberFloat(float min, float max);
float getRotationByVector(float x, float y);
float normalizeRotation(float src);
void getUnitVectorByRotation(float rotationNotN, Vec2* dest);
void normalizeVector(Vec2* src, Vec2* dest);
float nsmbwRotToMathRot(s16 rot);
s16 mathRotToNsmbwRot(float rotation);

#endif//_MY_MATHLIB_H_