#include "driver/gpio.h"

#include "ticker.h"
#include "button.h"

#define BUTTON_STATUS_CHANGE_TOLERANCE 0

void button_init(button_t *button)
{
        if (button == NULL)
                return;

        gpio_set_direction(button->pin, GPIO_MODE_INPUT);
        gpio_set_pull_mode(button->pin, GPIO_PULLDOWN_ONLY);
        gpio_wakeup_enable(button->pin, GPIO_INTR_HIGH_LEVEL);

        int init_status = 0;
        clock_t init_timestamp = ticker_now();

        button->status = init_status;
        button->prev_status = init_status;

        button->timestamp = init_timestamp;
        button->prev_timestamp = init_timestamp;
}

bool button_status_has_changed(button_t *button)
{
        int new_status, diff;
        clock_t now;
        bool has_changed, long_enough_ago;

        if (button == NULL)
                return false;

        new_status = gpio_get_level(button->pin);
        has_changed = (new_status != button->status);

        now = ticker_now();
        diff = now - button->timestamp;
        long_enough_ago = diff > BUTTON_STATUS_CHANGE_TOLERANCE;

        if (has_changed && long_enough_ago) {
                button->prev_status = button->status;
                button->status = new_status;

                button->prev_timestamp = button->timestamp;
                button->timestamp = now;
        }

        return (has_changed && long_enough_ago);
}

