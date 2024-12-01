#include <errno.h>

#include "ticker.h"
#include "button.h"

static inline int16_t button_get_hold_time(button_t *button)
{
        if (button == NULL) {
                errno = EFAULT;
                return -1;
        }

        if (button->status == BUTTON_STATUS_RELEASE) {
                return 0;
        }

        if (button->status == BUTTON_STATUS_PRESS
            && button->hold_time.init > 0) {
                return button->hold_time.init;
        }

        return button->hold_time.repeat;
}

bool button_check(button_t *button)
{
        int level;
        int16_t new_status;
        int32_t diff;
        clock_t now;

        if (button == NULL) {
                errno = EFAULT;
                return -1;
        }

        now = ticker_now();
        if (now == -1) {
                return -1;
        }

        if (button->timestamp == 0) { // Init
                gpio_set_direction(button->pin, GPIO_MODE_INPUT);
                gpio_set_pull_mode(button->pin, GPIO_PULLDOWN_ONLY);
                gpio_wakeup_enable(button->pin, GPIO_INTR_HIGH_LEVEL);

                button->status = BUTTON_STATUS_RELEASE;
                button->timestamp = now;
        }

        level = gpio_get_level(button->pin);
        diff = now - button->timestamp;

        new_status = -1;
        if (level == BUTTON_STATUS_PRESS) {
                if (button->status == BUTTON_STATUS_RELEASE) {
                        new_status = BUTTON_STATUS_PRESS;
                } else { // PRESS or HOLD
                        int16_t hold_time;

                        hold_time = button_get_hold_time(button);
                        if (hold_time == -1) {
                                return -1;
                        }
                        if (hold_time != 0 && diff >= hold_time) {
                                new_status = BUTTON_STATUS_HOLD;
                        }
                }
        } else if (level == BUTTON_STATUS_RELEASE) {
                if (button->status != BUTTON_STATUS_RELEASE) {
                        new_status = BUTTON_STATUS_RELEASE;
                }
        }

        if (new_status == -1) {
                return false;
        }

        button->status = new_status;
        button->timestamp = now;
        return true;
}

