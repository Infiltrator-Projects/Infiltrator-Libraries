// SPDX-License-Identifier: GPL-3.0-or-later
#include <infiltratr/graphics.h>

#include <assert.h>

int main(void) {
    InfiltratrSurface a = {0};
    InfiltratrSurface b = {0};
    InfiltratrColor red = {255, 0, 0, 255};
    InfiltratrColor blue_half = {0, 0, 255, 128};
    InfiltratrColor c;

    assert(infiltratr_surface_init(&a, 4, 4));
    assert(infiltratr_surface_init(&b, 8, 8));
    infiltratr_surface_clear(&a, (InfiltratrColor){0, 0, 0, 0});
    infiltratr_surface_clear(&b, (InfiltratrColor){10, 20, 30, 255});
    infiltratr_surface_fill_rect(&a, 1, 1, 2, 2, red);
    infiltratr_surface_blit_scaled_nearest(&b, &a, 0, 0, 8, 8);

    c = infiltratr_surface_get_pixel(&b, 3, 3);
    assert(c.r == 255 && c.g == 0 && c.b == 0 && c.a == 255);

    infiltratr_surface_blend_rect(&b, 3, 3, 1, 1, blue_half);
    c = infiltratr_surface_get_pixel(&b, 3, 3);
    assert(c.r >= 126 && c.r <= 128);
    assert(c.b >= 127 && c.b <= 129);

    assert(infiltratr_surface_copy(&a, &b));
    assert(a.width == 8 && a.height == 8 && a.pixel_count == 64);

    infiltratr_surface_release(&a);
    infiltratr_surface_release(&b);
    return 0;
}
