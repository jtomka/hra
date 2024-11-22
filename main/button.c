#include "driver/gpio.h"

#include "event.h"
#include "button.h"

void button_init(button_t *button)
{
        if (button == NULL)
                return;

        gpio_set_direction(button->pin, GPIO_MODE_INPUT);
        gpio_set_pull_mode(button->pin, GPIO_PULLDOWN_ONLY);
        gpio_wakeup_enable(button->pin, GPIO_INTR_HIGH_LEVEL);

        int init_status = 0;
        long init_timestamp = event_now();

        button->status = init_status;
        button->prev_status = init_status;

        button->timestamp = init_timestamp;
        button->prev_timestamp = init_timestamp;
}

bool button_status_has_changed(button_t *button)
{
        bool has_changed = false;

        if (button == NULL)
                return false;

        button->prev_status = button->status;
        button->status = gpio_get_level(button->pin);

        has_changed = (button->status != button->prev_status);
        if (has_changed) {
                button->prev_timestamp = button->timestamp;
                button->timestamp = event_now();
        }

        return has_changed;
}

