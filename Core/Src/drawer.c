/*
 * drawer.c
 *
 *  Created on: May 28, 2025
 *      Author: UnikoZera
 */

#include "drawer.h"

// 创建变换矩阵
OLED_Matrix OLED_CreateTransformMatrix(OLED_Transform *transform)
{
    OLED_Matrix matrix = {0};

    float cos_a = cosf(transform->angle);
    float sin_a = sinf(transform->angle);

    // 先平移到原点
    // 然后缩放
    // 然后旋转
    // 最后平移回去

    matrix.m[0][0] = cos_a * transform->scale_x;
    matrix.m[0][1] = -sin_a * transform->scale_y;
    matrix.m[0][2] = transform->center_x - (transform->center_x * cos_a * transform->scale_x - transform->center_y * sin_a * transform->scale_y);

    matrix.m[1][0] = sin_a * transform->scale_x;
    matrix.m[1][1] = cos_a * transform->scale_y;
    matrix.m[1][2] = transform->center_y - (transform->center_x * sin_a * transform->scale_x + transform->center_y * cos_a * transform->scale_y);

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = 1;

    return matrix;
}

// 应用变换
void OLED_TransformPoint(OLED_Matrix *matrix, int16_t x_in, int16_t y_in, int16_t *x_out, int16_t *y_out)
{
    float x = (float)x_in;
    float y = (float)y_in;

    *x_out = (int16_t)(matrix->m[0][0] * x + matrix->m[0][1] * y + matrix->m[0][2]);
    *y_out = (int16_t)(matrix->m[1][0] * x + matrix->m[1][1] * y + matrix->m[1][2]);
}

// 画线函数（用于绘制旋转矩形的边） //! UPDATEDISPLAY REQUIRED
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1)
    {
        OLED_WritePixel(x0, y0, 1);

        if (x0 == x1 && y0 == y1)
            break;

        int16_t e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// 旋转矩形 //! UPDATEDISPLAY REQUIRED
void OLED_DrawRotatedRect(int16_t x, int16_t y, uint8_t width, uint8_t height, OLED_Transform *transform)
{
    if (width == 0 || height == 0)
        return;

    OLED_Matrix matrix = OLED_CreateTransformMatrix(transform);

    // 定义矩形的四个角点（相对于中心）
    int16_t corners[4][2] = {
        {x - width / 2, y - height / 2}, // 左上
        {x + width / 2, y - height / 2}, // 右上
        {x + width / 2, y + height / 2}, // 右下
        {x - width / 2, y + height / 2}  // 左下
    };

    int16_t transformed_corners[4][2];

    // 变换四个角点
    for (int i = 0; i < 4; i++)
    {
        OLED_TransformPoint(&matrix, corners[i][0], corners[i][1],
                            &transformed_corners[i][0], &transformed_corners[i][1]);
    }

    // 绘制四条边
    for (int i = 0; i < 4; i++)
    {
        int next = (i + 1) % 4;
        OLED_DrawLine(transformed_corners[i][0], transformed_corners[i][1],
                      transformed_corners[next][0], transformed_corners[next][1]);
    }
}

// 旋转圆形 //! UPDATEDISPLAY REQUIRED
void OLED_DrawRotatedCircle(int16_t center_x, int16_t center_y, uint8_t radius, OLED_Transform *transform)
{
    OLED_Matrix matrix = OLED_CreateTransformMatrix(transform);

    // 使用中点圆算法生成圆周点，然后变换
    int16_t x = 0;
    int16_t y = radius;
    int16_t d = 1 - radius;

    // 绘制8个对称点
    while (x <= y)
    {
        // 8个对称位置
        int16_t points[8][2] = {
            {center_x + x, center_y + y}, {center_x - x, center_y + y}, {center_x + x, center_y - y}, {center_x - x, center_y - y}, {center_x + y, center_y + x}, {center_x - y, center_y + x}, {center_x + y, center_y - x}, {center_x - y, center_y - x}};

        for (int i = 0; i < 8; i++)
        {
            int16_t tx, ty;
            OLED_TransformPoint(&matrix, points[i][0], points[i][1], &tx, &ty);
            OLED_WritePixel(tx, ty, 1);
        }

        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// 变换后的字符渲染 //! UPDATEDISPLAY REQUIRED
void OLED_DisplayCharTransformed(int16_t center_x, int16_t center_y, char ch, OLED_Matrix *matrix)
{
    const uint8_t font_width = 6;
    const uint8_t font_height = 8;

    if (ch < 32)
        return;

    uint8_t c = ch - 32;

    // 声明外部字体数组
    extern const uint8_t OLED_FONT_6x8[];

    // 遍历字符的每个像素
    for (uint8_t char_col = 0; char_col < font_width; char_col++)
    {
        if ((c * font_width + char_col) >= 576)
            break; // 防止越界 (96字符 * 6宽度)

        uint8_t font_data_col = OLED_FONT_6x8[c * font_width + char_col];

        for (uint8_t char_row = 0; char_row < font_height; char_row++)
        {
            if ((font_data_col >> char_row) & 0x01)
            {
                // 计算像素相对于字符中心的位置
                int16_t pixel_x = center_x - font_width / 2 + char_col;
                int16_t pixel_y = center_y - font_height / 2 + char_row;

                // 应用变换
                int16_t transformed_x, transformed_y;
                OLED_TransformPoint(matrix, pixel_x, pixel_y, &transformed_x, &transformed_y);

                // 绘制变换后的像素
                OLED_WritePixel(transformed_x, transformed_y, 1);
            }
        }
    }
}

// 旋转文字 //! UPDATEDISPLAY REQUIRED
void OLED_DisplayStringRotated(int16_t x, int16_t y, char *str, OLED_Transform *transform)
{
    const uint8_t font_width = 6;
    const uint8_t font_height = 8;

    OLED_Matrix matrix = OLED_CreateTransformMatrix(transform);

    uint8_t str_len = strlen(str);
    int16_t total_width = str_len * font_width;

    // 为每个字符计算变换后的位置
    for (uint8_t i = 0; i < str_len; i++)
    {
        if (str[i] == '\0')
            break;

        // 字符相对于字符串中心的位置
        int16_t char_x = x - total_width / 2 + i * font_width + font_width / 2;
        int16_t char_y = y;

        // 渲染变换后的字符
        OLED_DisplayCharTransformed(char_x, char_y, str[i], &matrix);
    }
}

// 便捷函数：创建变换参数
OLED_Transform OLED_CreateTransform(float angle_degrees, float scale, int16_t center_x, int16_t center_y)
{
    OLED_Transform transform;
    transform.angle = angle_degrees * 3.14159f / 180.0f; // 转换为弧度
    transform.scale_x = scale;
    transform.scale_y = scale;
    transform.center_x = center_x;
    transform.center_y = center_y;
    return transform;
}

// 便捷函数：创建非等比缩放变换
OLED_Transform OLED_CreateTransformAdvanced(float angle_degrees, float scale_x, float scale_y, int16_t center_x, int16_t center_y)
{
    OLED_Transform transform;
    transform.angle = angle_degrees * 3.14159f / 180.0f;
    transform.scale_x = scale_x;
    transform.scale_y = scale_y;
    transform.center_x = center_x;
    transform.center_y = center_y;
    return transform;
}

// =============================================================================
// 3D 立方体实现
// =============================================================================

// 初始化3D立方体
void OLED_InitCube3D(OLED_Cube3D *cube, float size, int16_t center_x, int16_t center_y)
{
    if (!cube) return;
    
    cube->size = size;
    float half_size = size / 2.0f;
    
    // 定义立方体的8个顶点 (以原点为中心)
    cube->vertices[0] = (OLED_Point3D){-half_size, -half_size, -half_size}; // 后左下
    cube->vertices[1] = (OLED_Point3D){ half_size, -half_size, -half_size}; // 后右下
    cube->vertices[2] = (OLED_Point3D){ half_size,  half_size, -half_size}; // 后右上
    cube->vertices[3] = (OLED_Point3D){-half_size,  half_size, -half_size}; // 后左上
    cube->vertices[4] = (OLED_Point3D){-half_size, -half_size,  half_size}; // 前左下
    cube->vertices[5] = (OLED_Point3D){ half_size, -half_size,  half_size}; // 前右下
    cube->vertices[6] = (OLED_Point3D){ half_size,  half_size,  half_size}; // 前右上
    cube->vertices[7] = (OLED_Point3D){-half_size,  half_size,  half_size}; // 前左上
    
    // 定义立方体的12条边 (连接的顶点索引)
    // 后面的4条边
    cube->edges[0][0] = 0; cube->edges[0][1] = 1;
    cube->edges[1][0] = 1; cube->edges[1][1] = 2;
    cube->edges[2][0] = 2; cube->edges[2][1] = 3;
    cube->edges[3][0] = 3; cube->edges[3][1] = 0;
    
    // 前面的4条边
    cube->edges[4][0] = 4; cube->edges[4][1] = 5;
    cube->edges[5][0] = 5; cube->edges[5][1] = 6;
    cube->edges[6][0] = 6; cube->edges[6][1] = 7;
    cube->edges[7][0] = 7; cube->edges[7][1] = 4;
    
    // 连接前后面的4条边
    cube->edges[8][0] = 0; cube->edges[8][1] = 4;
    cube->edges[9][0] = 1; cube->edges[9][1] = 5;
    cube->edges[10][0] = 2; cube->edges[10][1] = 6;
    cube->edges[11][0] = 3; cube->edges[11][1] = 7;
    
    // 初始化变换参数
    cube->transform.angle_x = 0.0f;
    cube->transform.angle_y = 0.0f;
    cube->transform.angle_z = 0.0f;
    cube->transform.scale = 1.0f;
    cube->transform.center_x = center_x;
    cube->transform.center_y = center_y;
    cube->transform.distance = 100.0f; // 观察者距离
}

// 更新立方体旋转角度
void OLED_UpdateCubeRotation(OLED_Cube3D *cube, float delta_x, float delta_y, float delta_z)
{
    if (!cube) return;
    
    cube->transform.angle_x += delta_x;
    cube->transform.angle_y += delta_y;
    cube->transform.angle_z += delta_z;
    
    // 保持角度在 0-2π 范围内
    while (cube->transform.angle_x >= 2 * 3.14159f) cube->transform.angle_x -= 2 * 3.14159f;
    while (cube->transform.angle_x < 0) cube->transform.angle_x += 2 * 3.14159f;
    while (cube->transform.angle_y >= 2 * 3.14159f) cube->transform.angle_y -= 2 * 3.14159f;
    while (cube->transform.angle_y < 0) cube->transform.angle_y += 2 * 3.14159f;
    while (cube->transform.angle_z >= 2 * 3.14159f) cube->transform.angle_z -= 2 * 3.14159f;
    while (cube->transform.angle_z < 0) cube->transform.angle_z += 2 * 3.14159f;
}

// 设置立方体缩放
void OLED_SetCubeScale(OLED_Cube3D *cube, float scale)
{
    if (!cube || scale <= 0) return;
    cube->transform.scale = scale;
}

// 绕X轴旋转点
OLED_Point3D OLED_RotatePointX(OLED_Point3D point, float angle)
{
    OLED_Point3D result;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    result.x = point.x;
    result.y = point.y * cos_a - point.z * sin_a;
    result.z = point.y * sin_a + point.z * cos_a;
    
    return result;
}

// 绕Y轴旋转点
OLED_Point3D OLED_RotatePointY(OLED_Point3D point, float angle)
{
    OLED_Point3D result;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    result.x = point.x * cos_a + point.z * sin_a;
    result.y = point.y;
    result.z = -point.x * sin_a + point.z * cos_a;
    
    return result;
}

// 绕Z轴旋转点
OLED_Point3D OLED_RotatePointZ(OLED_Point3D point, float angle)
{
    OLED_Point3D result;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    result.x = point.x * cos_a - point.y * sin_a;
    result.y = point.x * sin_a + point.y * cos_a;
    result.z = point.z;
    
    return result;
}

// 应用3D变换
OLED_Point3D OLED_TransformPoint3D(OLED_Point3D point, OLED_Cube3D_Transform *transform)
{
    OLED_Point3D result = point;
    
    // 应用缩放
    result.x *= transform->scale;
    result.y *= transform->scale;
    result.z *= transform->scale;
    
    // 应用旋转 (顺序: X -> Y -> Z)
    result = OLED_RotatePointX(result, transform->angle_x);
    result = OLED_RotatePointY(result, transform->angle_y);
    result = OLED_RotatePointZ(result, transform->angle_z);
    
    return result;
}

// 将3D点投影到2D屏幕坐标
void OLED_Project3DTo2D(OLED_Point3D *point3d, OLED_Cube3D_Transform *transform, int16_t *x2d, int16_t *y2d)
{
    if (!point3d || !transform || !x2d || !y2d) return;
    
    // 透视投影
    float perspective_scale = transform->distance / (transform->distance + point3d->z);
    
    *x2d = (int16_t)(transform->center_x + point3d->x * perspective_scale);
    *y2d = (int16_t)(transform->center_y + point3d->y * perspective_scale);
}

// 绘制3D立方体 //! UPDATEDISPLAY REQUIRED
void OLED_DrawCube3D(OLED_Cube3D *cube)
{
    if (!cube) return;
    
    OLED_Point3D transformed_vertices[8];
    int16_t screen_points[8][2];
    
    // 变换所有顶点
    for (int i = 0; i < 8; i++)
    {
        transformed_vertices[i] = OLED_TransformPoint3D(cube->vertices[i], &cube->transform);
        OLED_Project3DTo2D(&transformed_vertices[i], &cube->transform, 
                          &screen_points[i][0], &screen_points[i][1]);
    }
    
    // 绘制所有边
    for (int i = 0; i < 12; i++)
    {
        int v1 = cube->edges[i][0];
        int v2 = cube->edges[i][1];
        
        OLED_DrawLine(screen_points[v1][0], screen_points[v1][1],
                     screen_points[v2][0], screen_points[v2][1]);
    }
}