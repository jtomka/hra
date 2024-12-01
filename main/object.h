#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJECT_WEIGHT_MAX -1

#define OBJECT_OVERFLOW_HIDE 0
#define OBJECT_OVERFLOW_WRAP 1
// TODO: #define OBJECT_OVERFLOW_BOUNCE 2
// TODO: #define OBJECT_OVERFLOW_DELETE 3

typedef struct {
        bool status;
        // TODO: color/brightness
} object_pixel_t;

typedef struct {
        double x;
        double y;

        uint8_t width;
        uint8_t height;
        object_pixel_t *pixels;

        uint8_t overflow;

        double direction;
        double speed;

        double weight;
} object_t;

extern int8_t object_init_pixels(object_t *object);

extern int8_t object_distance(object_t *object_1, object_t *object_2, double *result);
extern int8_t object_direction(object_t *object_1, object_t *object_2, double *result);

extern int8_t object_deflection_speed(object_t *object_1, object_t *object_2, double *result);
extern int8_t object_deflection_direction(object_t *object_1, object_t *object_2, double *result);

extern int8_t object_update_location(object_t *object);

#ifdef __cplusplus
}
#endif

#endif /* OBJECT_H */

