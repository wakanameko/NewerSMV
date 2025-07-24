#include "my_mathlib.h"

extern "C" u32 GenerateRandomNumber(int max);

float GenerateRandomNumberFloat(float min, float max){
    float randomNumber = GenerateRandomNumber(0xFFFFFFFF) / ((float)0xFFFFFFFF);
    return min + (max - min) * randomNumber;
}

float getDelta(float a, float b){
    return abs(a - b);
}

float getRotationByVector(float x, float y){//ラジアンで返す
    float absX = abs(x);
    float absY = abs(y);
    if(abs(x) < 0.000001){
        if(y < 0.0f){
            return M_PI * 1.5f;
        }else{
            return M_PI_2;
        }
    }
    float atanResult = atan(absY/absX);
    if(x > 0.0f && y > 0.0f){//第一象限
        return atanResult;
    }
    if(x > 0.0f && y <= 0.0f){//第四象限
        return (2 * M_PI) - atanResult;
    }
    if(x < 0.0f && y > 0.0f){//第二象限
        return M_PI - atanResult;
    }
    //第三象限
    return M_PI + atanResult;
}

float normalizeRotation(float src){
    if(src >= (2.0f * M_PI)){
        while(src >= (2.0f * M_PI)){
            src -= (2.0f * M_PI);
        }
        return src;
    }
    if(src < 0.0f){
        while(src < 0.0f){
            src += (2.0f * M_PI);
        }
        return src;
    }
    return src;
}

void normalizeVector(Vec2* src, Vec2* dest){
    float ookisa = sqrtf(src->x * src->x + src->y * src->y);
    dest->x = src->x / ookisa;
    dest->y = src->y / ookisa;
    return;
}

void getUnitVectorByRotation(float rotationNotN, Vec2* dest){
    float rotation = normalizeRotation(rotationNotN);
    dest->x = 1 * cos(rotation);
    dest->y = 1 * sin(rotation);
    return;
}

float nsmbwRotToMathRot(s16 rot){
    float dest = (((float)rot) / ((float)0x10000)) * M_PI * 2.0f;
    return normalizeRotation(dest);
}

s16 mathRotToNsmbwRot(float rotation){
    return (s16)(((int)((normalizeRotation(rotation) / M_PI) * ((float)0x8000) + 0.5f)) & 0xFFFF);
}