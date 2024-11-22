#include <stdbool.h>
#include <malloc.h>
#include <time.h>

#include "event.h"

typedef struct {
        event_type_t *list;
        int length;
} event_types_t;

static event_types_t event_types = { .list = NULL, .length = 0 };

void event_type_register(event_type_t event_type)
{
        size_t new_size;
        void *new_address;

        new_size = sizeof(event_type) * (event_types.length + 1);
        new_address = realloc(event_types.list, new_size);
        if (new_address == NULL)
                return;

        event_types.list = new_address;
        event_types.list[event_types.length] = event_type;
        event_types.length++;
}

void event_type_deregister_all()
{
        if (event_types.list == NULL)
                return;

        free(event_types.list);

        event_types.list = NULL;
        event_types.length = 0;
}

void event_loop()
{
        if (event_types.list == NULL)
                return;

        if (event_types.length == 0)
                return;

        while (true) {
                for (int i = 0; i < event_types.length; i++) {
                        int has_events = 0;
                        event_type_t this = event_types.list[i];

                        if (this.observer == NULL)
                                continue;

                        has_events = this.observer(this.data);
                        if (has_events && this.handler != NULL) {
                                this.handler(this.data);
                        }
                }
        }
}

long event_now() {
        return clock() / (CLOCKS_PER_SEC / 1000);
}

