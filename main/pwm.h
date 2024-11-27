#ifndef PWM_H
#define PWM_H

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        uint8_t duty_cycle;

        uint8_t adjust_min;
        uint8_t adjust_max;

        double counter;
} pwm_t;

extern void pwm_init(pwm_t *pwm);
extern bool pwm_check(pwm_t *pwm);

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */

