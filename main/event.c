#include <malloc.h>

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

int8_t event_type_register(event_type_t event_type)
{
        size_t new_size;
        void *new_address;

        new_size = sizeof(event_type) * (event_config.length + 1);
        new_address = realloc(event_config.list, new_size);
        if (new_address == NULL) {
                return -1;
        }

        event_config.list = new_address;
        event_config.list[event_config.length] = event_type;
        event_config.length++;

        return 0;
}

void event_type_deregister_all()
{
        if (event_config.list == NULL) {
                return;
        }

        free(event_config.list);

        event_config.list = NULL;
        event_config.length = 0;
}

void event_loop()
{
        while (true) {
                for (int i = 0; i < event_config.length; i++) {
                        event_type_t this;
                        int has_events;

                        if (event_config.list == NULL) {
                                continue;
                        }

                        this = event_config.list[i];

                        if (this.observer == NULL) {
                                continue;
                        }

                        has_events = this.observer(this.data);
                        if (has_events && this.handler != NULL) {
                                this.handler(this.data);
                        }
                }
        }
}

