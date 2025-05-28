/*
 * drawer.h
 *
 *  Created on: May 28, 2025
 *      Author: UnikoZera
 */

#ifndef INC_DRAWER_H_
#define INC_DRAWER_H_

#include <stdint.h>
#include <math.h>
#include <string.h>
#include "oled.h"

// 旋转和缩放结构体
typedef struct
{
    float angle;      // 旋转角度 (弧度)
    float scale_x;    // X轴缩放因子
    float scale_y;    // Y轴缩放因子
    int16_t center_x; // 旋转中心X
    int16_t center_y; // 旋转中心Y
} OLED_Transform;

// 2D变换矩阵结构
typedef struct
{
    float m[3][3]; // 3x3变换矩阵
} OLED_Matrix;

// 变换矩阵函数
OLED_Matrix OLED_CreateTransformMatrix(OLED_Transform *transform);
void OLED_TransformPoint(OLED_Matrix *matrix, int16_t x_in, int16_t y_in, int16_t *x_out, int16_t *y_out);

// 绘制函数
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void OLED_DrawRotatedRect(int16_t x, int16_t y, uint8_t width, uint8_t height, OLED_Transform *transform);
void OLED_DrawRotatedCircle(int16_t center_x, int16_t center_y, uint8_t radius, OLED_Transform *transform);

// 文字渲染函数
void OLED_DisplayStringRotated(int16_t x, int16_t y, char *str, OLED_Transform *transform);
void OLED_DisplayCharTransformed(int16_t center_x, int16_t center_y, char ch, OLED_Matrix *matrix);

// 便捷函数
OLED_Transform OLED_CreateTransform(float angle_degrees, float scale, int16_t center_x, int16_t center_y);
OLED_Transform OLED_CreateTransformAdvanced(float angle_degrees, float scale_x, float scale_y, int16_t center_x, int16_t center_y);

// 3D立方体相关结构体和函数
typedef struct {
    float x, y, z;
} OLED_Point3D;

typedef struct {
    float angle_x;    // 绕X轴旋转角度 (弧度)
    float angle_y;    // 绕Y轴旋转角度 (弧度)
    float angle_z;    // 绕Z轴旋转角度 (弧度)
    float scale;      // 缩放因子
    int16_t center_x; // 屏幕中心X坐标
    int16_t center_y; // 屏幕中心Y坐标
    float distance;   // 观察者距离 (影响透视效果)
} OLED_Cube3D_Transform;

typedef struct {
    OLED_Point3D vertices[8];     // 立方体的8个顶点
    int edges[12][2];             // 立方体的12条边 (每条边连接两个顶点)
    float size;                   // 立方体大小
    OLED_Cube3D_Transform transform; // 变换参数
} OLED_Cube3D;

// 3D立方体函数
void OLED_InitCube3D(OLED_Cube3D *cube, float size, int16_t center_x, int16_t center_y);
void OLED_UpdateCubeRotation(OLED_Cube3D *cube, float delta_x, float delta_y, float delta_z);
void OLED_SetCubeScale(OLED_Cube3D *cube, float scale);
void OLED_Project3DTo2D(OLED_Point3D *point3d, OLED_Cube3D_Transform *transform, int16_t *x2d, int16_t *y2d);
void OLED_DrawCube3D(OLED_Cube3D *cube);

// 3D变换函数
OLED_Point3D OLED_RotatePointX(OLED_Point3D point, float angle);
OLED_Point3D OLED_RotatePointY(OLED_Point3D point, float angle);
OLED_Point3D OLED_RotatePointZ(OLED_Point3D point, float angle);
OLED_Point3D OLED_TransformPoint3D(OLED_Point3D point, OLED_Cube3D_Transform *transform);

#endif /* INC_DRAWER_H_ */
