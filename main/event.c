#include <unistd.h>
#include <stdbool.h>
#include <malloc.h>

#include "esp_task_wdt.h"

#include "event.h"

typedef struct {
        event_type_t *list;
        uint16_t length;
} event_config_t;

static event_config_t event_config = { .list = NULL, .length = 0 };

bool event_true(void *data)
{
        return true;
}

void event_type_register(event_type_t event_type)
{
        size_t new_size;
        void *new_address;

        new_size = sizeof(event_type) * (event_config.length + 1);
        new_address = realloc(event_config.list, new_size);
        if (new_address == NULL)
                return;

        event_config.list = new_address;
        event_config.list[event_config.length] = event_type;
        event_config.length++;
}

void event_type_deregister_all()
{
        if (event_config.list == NULL)
                return;

        free(event_config.list);

        event_config.list = NULL;
        event_config.length = 0;
}

void event_loop()
{
	esp_task_wdt_config_t esp_task_wdt_config;

        if (event_config.list == NULL)
                return;

        if (event_config.length == 0)
                return;

	esp_task_wdt_config.timeout_ms = 20000;
	esp_task_wdt_config.idle_core_mask = 0b00; // TODO: how can we enable this?
	esp_task_wdt_config.trigger_panic = false;
	esp_task_wdt_reconfigure(&esp_task_wdt_config);
        esp_task_wdt_add(NULL);

        while (true) {
                for (int i = 0; i < event_config.length; i++) {
                        int has_events = 0;
                        event_type_t this = event_config.list[i];

                        if (this.observer == NULL)
                                continue;

                        has_events = this.observer(this.data);
                        if (has_events && this.handler != NULL) {
                                this.handler(this.data);
                        }
                }
                usleep(100);
		esp_task_wdt_reset();
        }
}

