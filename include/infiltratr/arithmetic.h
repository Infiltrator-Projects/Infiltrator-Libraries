// SPDX-License-Identifier: GPL-3.0-or-later
/** @file arithmetic.h @brief Dependency-free checked arithmetic primitives. */
#ifndef INFILTRATR_COMMON_ARITHMETIC_H
#define INFILTRATR_COMMON_ARITHMETIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool infiltratr_i64_floor_divmod(int64_t value, int64_t divisor,
                                 int64_t *quotient, int64_t *remainder);
int64_t infiltratr_i64_subtract_saturating(int64_t left, int64_t right);

bool infiltratr_u64_subtract_checked(uint64_t left, uint64_t right,
                                     uint64_t *result);
bool infiltratr_u64_multiply_checked(uint64_t left, uint64_t right,
                                     uint64_t *result);

bool infiltratr_size_add_checked(size_t left, size_t right, size_t *result);
bool infiltratr_size_multiply_checked(size_t left, size_t right,
                                      size_t *result);

bool infiltratr_array_reserve(void **array, size_t *capacity,
                              size_t element_size, size_t required,
                              size_t initial_capacity);

#ifdef __cplusplus
}
#endif
#endif
