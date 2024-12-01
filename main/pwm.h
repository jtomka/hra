#ifndef PWM_H
#define PWM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        uint8_t duty_cycle;

        uint8_t prev_duty_cycle;
        double on_per_off;
        double counter;
} pwm_t;

extern bool pwm_check(pwm_t *pwm);

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */

