/*
 * pid.h
 *
 *  Created on: May 3, 2025
 *      Author: UnikoZera
 */

#ifndef INC_PID_H_
#define INC_PID_H_

float pid_calculate(float targetValue, float currentValue, float Kp, float Ki, float Kd);

#endif /* INC_PID_H_ */
