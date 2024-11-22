#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool event_observer_fcn(void *data);
typedef void event_handler_fcn(void *data);

typedef struct {
        event_observer_fcn *observer;
        event_handler_fcn *handler;
        void *data;
} event_type_t;

extern void event_type_register(event_type_t event_type);
extern void event_type_deregister_all();

extern void event_loop();

extern long event_now();

#ifdef __cplusplus
}
#endif

