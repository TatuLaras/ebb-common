#ifndef _GAPI_MATH
#define _GAPI_MATH

#include "gapi_types.h"
#include "types.h"

typedef struct {
    vec3 point;
    float distance;
} RayHitInfo;

int gm_ray_plane_intersect(Ray *ray, Plane *plane, RayHitInfo *out_hit_info);
void gm_view_projection(GapiCamera *camera,
                        float aspect_ratio,
                        mat4 *out_view,
                        mat4 *out_projection);
Ray gm_screen_to_world_ray(GapiCamera *camera, float aspect_ratio, vec2 point);
int gm_get_grid_cell(GapiCamera *camera,
                     uint32_t window_width,
                     uint32_t window_height,
                     double mouse_x,
                     double mouse_y,
                     float grid_height,
                     float grid_density,
                     uint32_t *out_x,
                     uint32_t *out_y);

#endif
