#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>

#define OBJECT_WEIGHT_MAX -1

typedef struct {
        double x;
        double y;
        double width;
        double height;

        double direction;
        double speed;

        double weight;
} object_t;

extern double object_get_distance(object_t *object_1, object_t *object_2);
extern double object_get_direction(object_t *object_1, object_t *object_2);

extern double object_get_deflection_speed(object_t *object_1, object_t *object_2);
extern double object_get_deflection_direction(object_t *object_1, object_t *object_2);

extern void object_update_location(object_t *object);

#endif /* OBJECT_H */

