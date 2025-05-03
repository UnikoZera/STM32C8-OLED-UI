/*
 * pid.c
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */
// PID控制器的参数设置
// Kp: 比例增益
// Ki: 积分增益
// Kd: 微分增益
float pid_calculate(float targetValue, float currentValue, float Kp, float Ki, float Kd)
{
    static float previous_error = 0.0f;
    static float integral = 0.0f;

    float error = targetValue - currentValue;
    integral += error;
    float derivative = (error - previous_error);
    float output = Kp * error + Ki * integral + Kd * derivative;
    // 更新上一个误差
    previous_error = error;
    return output;
}

