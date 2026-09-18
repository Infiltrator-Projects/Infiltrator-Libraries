// SPDX-License-Identifier: GPL-3.0-or-later
#include <infiltratr/graphics.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

static int valid_dimensions(size_t width, size_t height, size_t *count) {
    if (!count || width == 0 || height == 0) return 0;
    if (width > SIZE_MAX / height) return 0;
    *count = width * height;
    if (*count > SIZE_MAX / sizeof(uint32_t)) return 0;
    return 1;
}

uint32_t infiltratr_color_pack(InfiltratrColor color) {
    return ((uint32_t)color.a << 24U) | ((uint32_t)color.r << 16U) |
           ((uint32_t)color.g << 8U) | (uint32_t)color.b;
}

InfiltratrColor infiltratr_color_unpack(uint32_t pixel) {
    InfiltratrColor color;
    color.r = (uint8_t)((pixel >> 16U) & 0xffU);
    color.g = (uint8_t)((pixel >> 8U) & 0xffU);
    color.b = (uint8_t)(pixel & 0xffU);
    color.a = (uint8_t)((pixel >> 24U) & 0xffU);
    return color;
}

int infiltratr_surface_init(InfiltratrSurface *surface, size_t width, size_t height) {
    if (!surface) return 0;
    surface->width = 0;
    surface->height = 0;
    surface->pixel_count = 0;
    surface->pixels = NULL;
    return infiltratr_surface_resize(surface, width, height);
}

int infiltratr_surface_resize(InfiltratrSurface *surface, size_t width, size_t height) {
    size_t count = 0;
    uint32_t *pixels = NULL;
    if (!surface || !valid_dimensions(width, height, &count)) return 0;
    if (surface->width == width && surface->height == height && surface->pixels) return 1;
    pixels = (uint32_t *)realloc(surface->pixels, count * sizeof(uint32_t));
    if (!pixels) return 0;
    surface->pixels = pixels;
    surface->width = width;
    surface->height = height;
    surface->pixel_count = count;
    return 1;
}

void infiltratr_surface_release(InfiltratrSurface *surface) {
    if (!surface) return;
    free(surface->pixels);
    surface->pixels = NULL;
    surface->width = 0;
    surface->height = 0;
    surface->pixel_count = 0;
}

int infiltratr_surface_copy(InfiltratrSurface *destination, const InfiltratrSurface *source) {
    if (!destination || !source || !source->pixels || source->pixel_count == 0) return 0;
    if (!infiltratr_surface_resize(destination, source->width, source->height)) return 0;
    memcpy(destination->pixels, source->pixels, source->pixel_count * sizeof(uint32_t));
    return 1;
}

int infiltratr_surface_copy_region(InfiltratrSurface *destination,
                                   const InfiltratrSurface *source,
                                   int source_x, int source_y,
                                   int width, int height) {
    int y;
    InfiltratrColor transparent = {0, 0, 0, 0};
    if (!destination || !source || !source->pixels || width <= 0 || height <= 0) return 0;
    if (!infiltratr_surface_resize(destination, (size_t)width, (size_t)height)) return 0;
    infiltratr_surface_clear(destination, transparent);
    for (y = 0; y < height; ++y) {
        const int sy = source_y + y;
        int x;
        if (sy < 0 || (size_t)sy >= source->height) continue;
        for (x = 0; x < width; ++x) {
            const int sx = source_x + x;
            if (sx < 0 || (size_t)sx >= source->width) continue;
            destination->pixels[(size_t)y * destination->width + (size_t)x] =
                source->pixels[(size_t)sy * source->width + (size_t)sx];
        }
    }
    return 1;
}

int infiltratr_surface_copy_luma_tinted(InfiltratrSurface *destination,
                                        const InfiltratrSurface *source,
                                        InfiltratrColor tint) {
    size_t i;
    if (!destination || !source || !source->pixels || source->pixel_count == 0) return 0;
    if (!infiltratr_surface_resize(destination, source->width, source->height)) return 0;
    for (i = 0; i < source->pixel_count; ++i) {
        InfiltratrColor c = infiltratr_color_unpack(source->pixels[i]);
        const unsigned luma = ((unsigned)c.r * 54U + (unsigned)c.g * 183U + (unsigned)c.b * 19U + 128U) >> 8U;
        c.r = (uint8_t)((luma * (unsigned)tint.r + 127U) / 255U);
        c.g = (uint8_t)((luma * (unsigned)tint.g + 127U) / 255U);
        c.b = (uint8_t)((luma * (unsigned)tint.b + 127U) / 255U);
        c.a = (uint8_t)(((unsigned)c.a * (unsigned)tint.a + 127U) / 255U);
        destination->pixels[i] = infiltratr_color_pack(c);
    }
    return 1;
}

void infiltratr_surface_clear(InfiltratrSurface *surface, InfiltratrColor color) {
    size_t i;
    uint32_t pixel;
    if (!surface || !surface->pixels) return;
    pixel = infiltratr_color_pack(color);
    for (i = 0; i < surface->pixel_count; ++i) surface->pixels[i] = pixel;
}

void infiltratr_surface_set_pixel(InfiltratrSurface *surface, int x, int y, InfiltratrColor color) {
    if (!surface || !surface->pixels || x < 0 || y < 0 ||
        (size_t)x >= surface->width || (size_t)y >= surface->height) return;
    surface->pixels[(size_t)y * surface->width + (size_t)x] = infiltratr_color_pack(color);
}

InfiltratrColor infiltratr_surface_get_pixel(const InfiltratrSurface *surface, int x, int y) {
    InfiltratrColor transparent = {0, 0, 0, 0};
    if (!surface || !surface->pixels || x < 0 || y < 0 ||
        (size_t)x >= surface->width || (size_t)y >= surface->height) return transparent;
    return infiltratr_color_unpack(surface->pixels[(size_t)y * surface->width + (size_t)x]);
}

void infiltratr_surface_fill_rect(InfiltratrSurface *surface, int x, int y, int width, int height,
                                  InfiltratrColor color) {
    int x0, y0, x1, y1, yy;
    uint32_t pixel;
    if (!surface || !surface->pixels || width <= 0 || height <= 0) return;
    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + width;
    y1 = y + height;
    if (x1 > (int)surface->width) x1 = (int)surface->width;
    if (y1 > (int)surface->height) y1 = (int)surface->height;
    if (x0 >= x1 || y0 >= y1) return;
    pixel = infiltratr_color_pack(color);
    for (yy = y0; yy < y1; ++yy) {
        int xx;
        uint32_t *row = surface->pixels + (size_t)yy * surface->width;
        for (xx = x0; xx < x1; ++xx) row[xx] = pixel;
    }
}

static InfiltratrColor composite_source_over(InfiltratrColor source,
                                              InfiltratrColor destination) {
    const unsigned source_alpha = source.a;
    const unsigned destination_alpha = destination.a;
    const unsigned inverse = 255U - source_alpha;
    const unsigned alpha_scaled =
        source_alpha * 255U + destination_alpha * inverse;
    InfiltratrColor output = {0, 0, 0, 0};

    if (alpha_scaled == 0U) return output;

    output.a = (uint8_t)((alpha_scaled + 127U) / 255U);
    output.r = (uint8_t)(
        ((unsigned)source.r * source_alpha * 255U +
         (unsigned)destination.r * destination_alpha * inverse +
         alpha_scaled / 2U) / alpha_scaled);
    output.g = (uint8_t)(
        ((unsigned)source.g * source_alpha * 255U +
         (unsigned)destination.g * destination_alpha * inverse +
         alpha_scaled / 2U) / alpha_scaled);
    output.b = (uint8_t)(
        ((unsigned)source.b * source_alpha * 255U +
         (unsigned)destination.b * destination_alpha * inverse +
         alpha_scaled / 2U) / alpha_scaled);
    return output;
}

void infiltratr_surface_blend_rect(InfiltratrSurface *surface, int x, int y, int width, int height,
                                   InfiltratrColor color) {
    int x0, y0, x1, y1, yy;
    if (!surface || !surface->pixels || width <= 0 || height <= 0 || color.a == 0) return;
    if (color.a == 255) {
        infiltratr_surface_fill_rect(surface, x, y, width, height, color);
        return;
    }
    x0 = x < 0 ? 0 : x;
    y0 = y < 0 ? 0 : y;
    x1 = x + width;
    y1 = y + height;
    if (x1 > (int)surface->width) x1 = (int)surface->width;
    if (y1 > (int)surface->height) y1 = (int)surface->height;
    if (x0 >= x1 || y0 >= y1) return;
    for (yy = y0; yy < y1; ++yy) {
        int xx;
        uint32_t *row = surface->pixels + (size_t)yy * surface->width;
        for (xx = x0; xx < x1; ++xx) {
            const InfiltratrColor destination = infiltratr_color_unpack(row[xx]);
            row[xx] = infiltratr_color_pack(composite_source_over(color, destination));
        }
    }
}

static void blend_pixel(InfiltratrSurface *destination, int x, int y, InfiltratrColor source) {
    InfiltratrColor existing;
    if (source.a == 0 || !destination || !destination->pixels || x < 0 || y < 0 ||
        (size_t)x >= destination->width || (size_t)y >= destination->height) return;
    if (source.a == 255) {
        destination->pixels[(size_t)y * destination->width + (size_t)x] = infiltratr_color_pack(source);
        return;
    }
    existing = infiltratr_surface_get_pixel(destination, x, y);
    destination->pixels[(size_t)y * destination->width + (size_t)x] =
        infiltratr_color_pack(composite_source_over(source, existing));
}

void infiltratr_surface_blit(InfiltratrSurface *destination, const InfiltratrSurface *source,
                             int destination_x, int destination_y) {
    infiltratr_surface_blit_region(destination, source, 0, 0,
                                   source ? (int)source->width : 0,
                                   source ? (int)source->height : 0,
                                   destination_x, destination_y);
}

void infiltratr_surface_blit_region(InfiltratrSurface *destination,
                                    const InfiltratrSurface *source,
                                    int source_x, int source_y,
                                    int source_width, int source_height,
                                    int destination_x, int destination_y) {
    int y;
    if (!destination || !destination->pixels || !source || !source->pixels ||
        source_width <= 0 || source_height <= 0) return;
    for (y = 0; y < source_height; ++y) {
        const int sy = source_y + y;
        int x;
        if (sy < 0 || (size_t)sy >= source->height) continue;
        for (x = 0; x < source_width; ++x) {
            const int sx = source_x + x;
            if (sx < 0 || (size_t)sx >= source->width) continue;
            blend_pixel(destination, destination_x + x, destination_y + y,
                        infiltratr_color_unpack(source->pixels[(size_t)sy * source->width + (size_t)sx]));
        }
    }
}

void infiltratr_surface_blit_scaled_nearest(InfiltratrSurface *destination,
                                            const InfiltratrSurface *source,
                                            int destination_x, int destination_y,
                                            int destination_width, int destination_height) {
    infiltratr_surface_blit_region_scaled_nearest(destination, source, 0, 0,
                                                  source ? (int)source->width : 0,
                                                  source ? (int)source->height : 0,
                                                  destination_x, destination_y,
                                                  destination_width, destination_height);
}

void infiltratr_surface_blit_region_scaled_nearest(InfiltratrSurface *destination,
                                                   const InfiltratrSurface *source,
                                                   int source_x, int source_y,
                                                   int source_width, int source_height,
                                                   int destination_x, int destination_y,
                                                   int destination_width, int destination_height) {
    int y;
    if (!destination || !destination->pixels || !source || !source->pixels ||
        source_width <= 0 || source_height <= 0 || destination_width <= 0 || destination_height <= 0) return;
    for (y = 0; y < destination_height; ++y) {
        const int sy = source_y + (int)(((size_t)y * (size_t)source_height) / (size_t)destination_height);
        int x;
        if (sy < 0 || (size_t)sy >= source->height) continue;
        for (x = 0; x < destination_width; ++x) {
            const int sx = source_x + (int)(((size_t)x * (size_t)source_width) / (size_t)destination_width);
            if (sx < 0 || (size_t)sx >= source->width) continue;
            blend_pixel(destination, destination_x + x, destination_y + y,
                        infiltratr_color_unpack(source->pixels[(size_t)sy * source->width + (size_t)sx]));
        }
    }
}

static InfiltratrColor sample_surface_i64(const InfiltratrSurface *source,
                                          int64_t x, int64_t y) {
    InfiltratrColor transparent = {0, 0, 0, 0};
    if (!source || !source->pixels || x < 0 || y < 0) return transparent;
    if ((uint64_t)x >= (uint64_t)source->width ||
        (uint64_t)y >= (uint64_t)source->height) return transparent;
    return infiltratr_color_unpack(
        source->pixels[(size_t)y * source->width + (size_t)x]);
}

static uint8_t rounded_u8(double value) {
    if (!(value > 0.0)) return 0U;
    if (value >= 255.0) return 255U;
    return (uint8_t)floor(value + 0.5);
}

static InfiltratrColor bilinear_sample(const InfiltratrSurface *source,
                                       double source_x, double source_y) {
    const int64_t x0 = (int64_t)floor(source_x);
    const int64_t y0 = (int64_t)floor(source_y);
    const int64_t x1 = x0 + 1;
    const int64_t y1 = y0 + 1;
    const double fx = source_x - floor(source_x);
    const double fy = source_y - floor(source_y);
    const double weights[4] = {
        (1.0 - fx) * (1.0 - fy),
        fx * (1.0 - fy),
        (1.0 - fx) * fy,
        fx * fy
    };
    const InfiltratrColor samples[4] = {
        sample_surface_i64(source, x0, y0),
        sample_surface_i64(source, x1, y0),
        sample_surface_i64(source, x0, y1),
        sample_surface_i64(source, x1, y1)
    };
    double alpha = 0.0;
    double red_alpha = 0.0;
    double green_alpha = 0.0;
    double blue_alpha = 0.0;
    int i;
    InfiltratrColor result = {0, 0, 0, 0};

    for (i = 0; i < 4; ++i) {
        const double weighted_alpha = weights[i] * (double)samples[i].a;
        alpha += weighted_alpha;
        red_alpha += weighted_alpha * (double)samples[i].r;
        green_alpha += weighted_alpha * (double)samples[i].g;
        blue_alpha += weighted_alpha * (double)samples[i].b;
    }
    if (!(alpha > 0.0)) return result;

    result.a = rounded_u8(alpha);
    result.r = rounded_u8(red_alpha / alpha);
    result.g = rounded_u8(green_alpha / alpha);
    result.b = rounded_u8(blue_alpha / alpha);
    return result;
}

void infiltratr_surface_blit_scaled_bilinear(InfiltratrSurface *destination,
                                             const InfiltratrSurface *source,
                                             int destination_x, int destination_y,
                                             int destination_width, int destination_height) {
    infiltratr_surface_blit_region_scaled_bilinear(
        destination, source, 0, 0,
        source ? (int)source->width : 0,
        source ? (int)source->height : 0,
        destination_x, destination_y, destination_width, destination_height);
}

void infiltratr_surface_blit_region_scaled_bilinear(InfiltratrSurface *destination,
                                                    const InfiltratrSurface *source,
                                                    int source_x, int source_y,
                                                    int source_width, int source_height,
                                                    int destination_x, int destination_y,
                                                    int destination_width, int destination_height) {
    int y;
    if (!destination || !destination->pixels || !source || !source->pixels ||
        source_width <= 0 || source_height <= 0 ||
        destination_width <= 0 || destination_height <= 0) return;

    for (y = 0; y < destination_height; ++y) {
        const double fy = destination_height == 1 ? 0.0 :
            (double)y * (double)(source_height - 1) /
            (double)(destination_height - 1);
        int x;
        for (x = 0; x < destination_width; ++x) {
            const double fx = destination_width == 1 ? 0.0 :
                (double)x * (double)(source_width - 1) /
                (double)(destination_width - 1);
            const InfiltratrColor sample =
                bilinear_sample(source, (double)source_x + fx,
                                (double)source_y + fy);
            blend_pixel(destination, destination_x + x, destination_y + y, sample);
        }
    }
}

void infiltratr_surface_blit_rotated(InfiltratrSurface *destination,
                                     const InfiltratrSurface *source,
                                     int center_x, int center_y,
                                     double angle_radians) {
    const double cosine = cos(angle_radians);
    const double sine = sin(angle_radians);
    const double source_cx = source ? ((double)source->width - 1.0) * 0.5 : 0.0;
    const double source_cy = source ? ((double)source->height - 1.0) * 0.5 : 0.0;
    const int radius_x = source ? (int)((source->width + 1U) / 2U) : 0;
    const int radius_y = source ? (int)((source->height + 1U) / 2U) : 0;
    int y;
    if (!destination || !destination->pixels || !source || !source->pixels) return;
    for (y = center_y - radius_y; y <= center_y + radius_y; ++y) {
        int x;
        for (x = center_x - radius_x; x <= center_x + radius_x; ++x) {
            const double dx = (double)(x - center_x);
            const double dy = (double)(y - center_y);
            const double sx = source_cx + cosine * dx + sine * dy;
            const double sy = source_cy - sine * dx + cosine * dy;
            int isx, isy;
            if (sx < 0.0 || sy < 0.0 || sx >= (double)source->width || sy >= (double)source->height) continue;
            isx = (int)floor(sx + 0.5);
            isy = (int)floor(sy + 0.5);
            if ((size_t)isx >= source->width || (size_t)isy >= source->height) continue;
            blend_pixel(destination, x, y,
                        infiltratr_color_unpack(source->pixels[(size_t)isy * source->width + (size_t)isx]));
        }
    }
}
