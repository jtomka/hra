#include <math.h>

#include "object.h"

#define x2(x) ((x) * (x))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
        double width;
        double height;
} rectangle_t;

/*
 * 1. Combine the two rectangles into one large rectangle
 * 2. Subtract from the large rectangle the first rectangle and the second
 *    rectangle
 * 3. What is left after the subtraction is a rectangle between the two
 *    rectangles, the diagonal of this rectangle is the distance between the
 *    two rectangles.
 */
void object_get_distance_rectangle(rectangle_t *rect, object_t *object_1, object_t *object_2)
{
        double x, y, width, height;

        if (rect == NULL)
                return;
        if (object_1 == NULL)
                return;
        if (object_2 == NULL)
                return;

        x = min(object_1->x, object_2->x);
        width = max(object_1->x + object_1->width, object_2->x + object_2->width);
        width -= x;
        width -= object_1->width + object_2->width;
        width = max(0, width);

        y = min(object_1->y, object_2->y);
        height = max(object_1->y + object_1->height, object_2->y + object_2->height);
        height -= y;
        height -= object_1->height + object_2->height;
        height = max(0, height);

        rect->width = width;
        rect->height = height;
}

double object_get_distance(object_t *object_1, object_t *object_2)
{
        rectangle_t rect;

        if (object_1 == NULL)
                return 0;
        if (object_2 == NULL)
                return 0;

        object_get_distance_rectangle(&rect, object_1, object_2);

        return sqrt(x2(rect.width) + x2(rect.height));
}

double object_get_direction(object_t *object_1, object_t *object_2)
{
        rectangle_t rect;
        double diagonal;

        if (object_1 == NULL)
                return 0;
        if (object_2 == NULL)
                return 0;

        object_get_distance_rectangle(&rect, object_1, object_2);

        diagonal = sqrt(x2(rect.width) + x2(rect.height));

        // TODO: adjust direction relative to object_1
        return asin(rect.width / diagonal);
}

double object_get_deflection_speed(object_t *object_1, object_t *object_2)
{
        return 0;
}

double object_get_deflection_direction(object_t *object_1, object_t *object_2)
{
        return 0;
}

void object_update_location(object_t *object)
{
}

