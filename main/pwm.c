#include <errno.h>

#include "pwm.h"

bool pwm_check(pwm_t *pwm)
{
        if (pwm == NULL) {
                errno = EFAULT;
                return -1;
        }

        if (pwm->duty_cycle == 0) {
                errno = EFAULT;
                return -1;
        }

        if (pwm->duty_cycle >= 100) {
                return true;
        }

        if (pwm->duty_cycle != pwm->prev_duty_cycle) {
                pwm->prev_duty_cycle = pwm->duty_cycle;
                pwm->on_per_off = (double) pwm->duty_cycle / (100 - pwm->duty_cycle);
                pwm->counter = 0;
        }

        if (pwm->counter < 1) {
                pwm->counter += pwm->on_per_off;
        }

        if (pwm->counter >= 1) {
                pwm->counter--;
                return true;
        } else {
                return false;
        }
}

