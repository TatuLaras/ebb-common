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

#endif
