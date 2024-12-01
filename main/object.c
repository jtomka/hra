#include <math.h>
#include <sys/errno.h>

#include "object.h"

#ifndef x2
#define x2(x) ((x) * (x))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

typedef struct {
        double width;
        double height;
} rectangle_t;

int8_t object_init_pixels(object_t *object)
{
        size_t new_size;

        if (object == NULL) {
                errno = EFAULT;
                return -1; 
        }

        new_size = sizeof(object_pixel_t) * object->width * object->height;
        object->pixels = (object_pixel_t *) realloc(object->pixels, new_size);

        if (object->pixels == NULL) {
                object->width = 0;
                object->height = 0;

                return -1;
        }

        return new_size;
}

/*
 * 1. Combine the two rectangles into one large rectangle
 * 2. Subtract from the large rectangle the first rectangle and the second
 *    rectangle
 * 3. What is left after the subtraction is a rectangle between the two
 *    rectangles, the diagonal of this rectangle is the distance between the
 *    two rectangles.
 */
static inline int8_t object_distance_rectangle(
                object_t *object_1, object_t *object_2, rectangle_t *result)
{
        double x, y, width, height;

        if (object_1 == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (object_2 == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (result == NULL) {
                errno = EFAULT;
                return -1;
        }

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

        result->width = width;
        result->height = height;

        return 0;
}

int8_t object_distance(object_t *object_1, object_t *object_2, double *result)
{
        rectangle_t rect;
        int8_t retval;

        if (object_1 == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (object_2 == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (result == NULL) {
                errno = EFAULT;
                return -1;
        }

        retval = object_distance_rectangle(object_1, object_2, &rect);
        if (retval == -1) {
                return -1;
        }

        *result = sqrt(x2(rect.width) + x2(rect.height));
        return 0;
}

int8_t object_direction(object_t *object_1, object_t *object_2, double *result)
{
        rectangle_t rect;
        int8_t retval;
        double diagonal;

        if (object_1 == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (object_2 == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (result == NULL) {
                errno = EFAULT;
                return -1;
        }

        retval = object_distance_rectangle(object_1, object_2, &rect);
        if (retval == -1) {
                return -1;
        }

        diagonal = sqrt(x2(rect.width) + x2(rect.height));

        // TODO: adjust direction relative to object_1
        *result = asin(rect.width / diagonal);
        return 0;
}

int8_t object_deflection_speed(object_t *object_1, object_t *object_2, double *result)
{
        errno = ENOTSUP;
        return -1;
}

int8_t object_deflection_direction(object_t *object_1, object_t *object_2, double *result)
{
        errno = ENOTSUP;
        return -1;
}

int8_t object_update_location(object_t *object)
{
        errno = ENOTSUP;
        return -1;
}

