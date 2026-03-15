#include "gapi_math.h"
#include "cglm/cglm.h"
#include "cglm/vec3.h"
#include "log.h"
#include <math.h>
#include <string.h>

#define EPSILON 0.00001
#define DEG_TO_RAD 0.01745329252

int gm_ray_plane_intersect(Ray *ray, Plane *plane, RayHitInfo *out_hit_info) {

    float denominator = glm_vec3_dot(plane->normal, ray->direction);
    if (fabs(denominator) < EPSILON)
        return 0;

    vec3 difference;
    glm_vec3_sub(plane->center, ray->origin, difference);
    float t = glm_vec3_dot(difference, plane->normal) / denominator;
    if (t < EPSILON)
        return 0;

    vec3 hit_point;
    glm_vec3_copy(ray->direction, hit_point);
    glm_vec3_normalize(hit_point);
    glm_vec3_scale(hit_point, t, hit_point);
    glm_vec3_add(hit_point, ray->origin, hit_point);

    out_hit_info->distance = t;
    glm_vec3_copy(hit_point, out_hit_info->point);
    return 1;
}

void gm_view_projection(GapiCamera *camera,
                        float aspect_ratio,
                        mat4 *out_view,
                        mat4 *out_projection) {

    glm_mat4_identity(*out_view);
    glm_mat4_identity(*out_projection);
    glm_lookat(camera->pos, camera->target, camera->up, *out_view);
    glm_perspective(DEG_TO_RAD * camera->fov_degrees,
                    aspect_ratio,
                    camera->near_plane,
                    camera->far_plane,
                    *out_projection);
    (*out_projection)[1][1] *= -1;
}

Ray gm_screen_to_world_ray(GapiCamera *camera, float aspect_ratio, vec2 point) {

    mat4 view, projection;
    gm_view_projection(camera, aspect_ratio, &view, &projection);

    mat4 inverse;
    // Reverse?
    glm_mat4_mul(projection, view, inverse);
    glm_mat4_inv(inverse, inverse);

    vec4 far = {
        point[0] * 2.0 - 1,
        point[1] * 2.0 - 1,
        1.0,
        1.0,
    };
    vec4 near = {
        point[0] * 2.0 - 1,
        point[1] * 2.0 - 1,
        0.0,
        1.0,
    };

    vec4 near_pos;
    vec4 far_pos;
    glm_mat4_mulv(inverse, far, far_pos);
    glm_mat4_mulv(inverse, near, near_pos);

    near_pos[3] = 1.0 / near_pos[3];
    near_pos[0] *= near_pos[3];
    near_pos[1] *= near_pos[3];
    near_pos[2] *= near_pos[3];

    far_pos[3] = 1.0 / far_pos[3];
    far_pos[0] *= far_pos[3];
    far_pos[1] *= far_pos[3];
    far_pos[2] *= far_pos[3];

    vec3 direction;
    glm_vec3_sub(far_pos, near_pos, direction);
    glm_vec3_normalize(direction);

    Ray ray = {0};
    memcpy(ray.origin, camera->pos, sizeof(vec3));
    memcpy(ray.direction, direction, sizeof(vec3));

    return ray;
}
