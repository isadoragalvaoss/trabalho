#ifndef PID_H_
#define PID_H_

void pid_config_const(double Kp_, double Ki_, double Kd_);
void pid_atual_ref(float ref_);
double pid_control(double saida_medida);

#endif /* PID_H_ */