// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INFILTRATR_GRAPHICS_H
#define INFILTRATR_GRAPHICS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InfiltratrColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} InfiltratrColor;

typedef struct InfiltratrSurface {
    size_t width;
    size_t height;
    size_t pixel_count;
    uint32_t *pixels; /* 0xAARRGGBB */
} InfiltratrSurface;

uint32_t infiltratr_color_pack(InfiltratrColor color);
InfiltratrColor infiltratr_color_unpack(uint32_t pixel);

int infiltratr_surface_init(InfiltratrSurface *surface, size_t width, size_t height);
int infiltratr_surface_resize(InfiltratrSurface *surface, size_t width, size_t height);
void infiltratr_surface_release(InfiltratrSurface *surface);
int infiltratr_surface_copy(InfiltratrSurface *destination, const InfiltratrSurface *source);
int infiltratr_surface_copy_region(InfiltratrSurface *destination,
                                   const InfiltratrSurface *source,
                                   int source_x, int source_y,
                                   int width, int height);
int infiltratr_surface_copy_luma_tinted(InfiltratrSurface *destination,
                                        const InfiltratrSurface *source,
                                        InfiltratrColor tint);

void infiltratr_surface_clear(InfiltratrSurface *surface, InfiltratrColor color);
void infiltratr_surface_set_pixel(InfiltratrSurface *surface, int x, int y, InfiltratrColor color);
InfiltratrColor infiltratr_surface_get_pixel(const InfiltratrSurface *surface, int x, int y);
void infiltratr_surface_fill_rect(InfiltratrSurface *surface, int x, int y, int width, int height,
                                  InfiltratrColor color);
void infiltratr_surface_blend_rect(InfiltratrSurface *surface, int x, int y, int width, int height,
                                   InfiltratrColor color);
void infiltratr_surface_blit(InfiltratrSurface *destination, const InfiltratrSurface *source,
                             int destination_x, int destination_y);
void infiltratr_surface_blit_region(InfiltratrSurface *destination,
                                    const InfiltratrSurface *source,
                                    int source_x, int source_y,
                                    int source_width, int source_height,
                                    int destination_x, int destination_y);
void infiltratr_surface_blit_scaled_nearest(InfiltratrSurface *destination,
                                            const InfiltratrSurface *source,
                                            int destination_x, int destination_y,
                                            int destination_width, int destination_height);
void infiltratr_surface_blit_region_scaled_nearest(InfiltratrSurface *destination,
                                                   const InfiltratrSurface *source,
                                                   int source_x, int source_y,
                                                   int source_width, int source_height,
                                                   int destination_x, int destination_y,
                                                   int destination_width, int destination_height);
void infiltratr_surface_blit_rotated(InfiltratrSurface *destination,
                                     const InfiltratrSurface *source,
                                     int center_x, int center_y,
                                     double angle_radians);

#ifdef __cplusplus
}
#endif

#endif
