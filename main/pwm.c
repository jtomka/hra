
#include "pwm.h"

void pwm_init(pwm_t *pwm)
{
        if (pwm == NULL)
                return;

        pwm->counter = 0;
}

bool pwm_check(pwm_t *pwm)
{
        double adj_duty_cycle;
        double on_per_off;

        if (pwm == NULL)
                return false;

        if (pwm->adjust_max > 100)
                return false;

        if (pwm->adjust_min > pwm->adjust_max)
                return false;

        if (pwm->duty_cycle == 0)
                return false;

        if (pwm->duty_cycle >= 100)
                return true;

        adj_duty_cycle = pwm->duty_cycle;
        if (pwm->adjust_min > 0 || pwm->adjust_max < 100) {
                adj_duty_cycle = (double) pwm->duty_cycle / 100;
                adj_duty_cycle *= pwm->adjust_max - pwm->adjust_min;
                adj_duty_cycle += pwm->adjust_min;
        }

        on_per_off = adj_duty_cycle / (100 - adj_duty_cycle);
        if (pwm->counter < 1) {
                pwm->counter += on_per_off;
        }

        if (pwm->counter >= 1) {
                pwm->counter--;
                return true;
        } else {
                return false;
        }
}

