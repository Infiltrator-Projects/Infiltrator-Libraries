// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file endian.h
 * @brief Dependency-free fixed-width endian conversion and byte-access primitives.
 */
#ifndef INFILTRATR_COMMON_ENDIAN_H
#define INFILTRATR_COMMON_ENDIAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint16_t infiltratr_bswap16(uint16_t value)
{
    return (uint16_t)((value >> 8) | (value << 8));
}

static inline uint32_t infiltratr_bswap32(uint32_t value)
{
    return ((value & UINT32_C(0x000000ff)) << 24) |
           ((value & UINT32_C(0x0000ff00)) << 8) |
           ((value & UINT32_C(0x00ff0000)) >> 8) |
           ((value & UINT32_C(0xff000000)) >> 24);
}

static inline uint64_t infiltratr_bswap64(uint64_t value)
{
    return ((uint64_t)infiltratr_bswap32((uint32_t)value) << 32) |
           infiltratr_bswap32((uint32_t)(value >> 32));
}

#if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || \
    (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define infiltratr_cpu_to_le16(value) ((uint16_t)(value))
#define infiltratr_cpu_to_le32(value) ((uint32_t)(value))
#define infiltratr_cpu_to_le64(value) ((uint64_t)(value))
#define infiltratr_le16_to_cpu(value) ((uint16_t)(value))
#define infiltratr_le32_to_cpu(value) ((uint32_t)(value))
#define infiltratr_le64_to_cpu(value) ((uint64_t)(value))
#define infiltratr_cpu_to_be16(value) infiltratr_bswap16((uint16_t)(value))
#define infiltratr_cpu_to_be32(value) infiltratr_bswap32((uint32_t)(value))
#define infiltratr_cpu_to_be64(value) infiltratr_bswap64((uint64_t)(value))
#define infiltratr_be16_to_cpu(value) infiltratr_bswap16((uint16_t)(value))
#define infiltratr_be32_to_cpu(value) infiltratr_bswap32((uint32_t)(value))
#define infiltratr_be64_to_cpu(value) infiltratr_bswap64((uint64_t)(value))
#else
#define infiltratr_cpu_to_le16(value) infiltratr_bswap16((uint16_t)(value))
#define infiltratr_cpu_to_le32(value) infiltratr_bswap32((uint32_t)(value))
#define infiltratr_cpu_to_le64(value) infiltratr_bswap64((uint64_t)(value))
#define infiltratr_le16_to_cpu(value) infiltratr_bswap16((uint16_t)(value))
#define infiltratr_le32_to_cpu(value) infiltratr_bswap32((uint32_t)(value))
#define infiltratr_le64_to_cpu(value) infiltratr_bswap64((uint64_t)(value))
#define infiltratr_cpu_to_be16(value) ((uint16_t)(value))
#define infiltratr_cpu_to_be32(value) ((uint32_t)(value))
#define infiltratr_cpu_to_be64(value) ((uint64_t)(value))
#define infiltratr_be16_to_cpu(value) ((uint16_t)(value))
#define infiltratr_be32_to_cpu(value) ((uint32_t)(value))
#define infiltratr_be64_to_cpu(value) ((uint64_t)(value))
#endif

static inline uint16_t infiltratr_load_le16(const void *bytes)
{
    const uint8_t *p = (const uint8_t *)bytes;
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static inline uint32_t infiltratr_load_le32(const void *bytes)
{
    const uint8_t *p = (const uint8_t *)bytes;
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline uint64_t infiltratr_load_le64(const void *bytes)
{
    const uint8_t *p = (const uint8_t *)bytes;
    return (uint64_t)infiltratr_load_le32(p) |
           ((uint64_t)infiltratr_load_le32(p + 4) << 32);
}

static inline uint16_t infiltratr_load_be16(const void *bytes)
{
    const uint8_t *p = (const uint8_t *)bytes;
    return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static inline uint32_t infiltratr_load_be32(const void *bytes)
{
    const uint8_t *p = (const uint8_t *)bytes;
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static inline uint64_t infiltratr_load_be64(const void *bytes)
{
    const uint8_t *p = (const uint8_t *)bytes;
    return ((uint64_t)infiltratr_load_be32(p) << 32) |
           (uint64_t)infiltratr_load_be32(p + 4);
}

static inline void infiltratr_store_le16(void *bytes, uint16_t value)
{
    uint8_t *p = (uint8_t *)bytes;
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static inline void infiltratr_store_le32(void *bytes, uint32_t value)
{
    uint8_t *p = (uint8_t *)bytes;
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16);
    p[3] = (uint8_t)(value >> 24);
}

static inline void infiltratr_store_le64(void *bytes, uint64_t value)
{
    uint8_t *p = (uint8_t *)bytes;
    infiltratr_store_le32(p, (uint32_t)value);
    infiltratr_store_le32(p + 4, (uint32_t)(value >> 32));
}

static inline void infiltratr_store_be16(void *bytes, uint16_t value)
{
    uint8_t *p = (uint8_t *)bytes;
    p[0] = (uint8_t)(value >> 8);
    p[1] = (uint8_t)value;
}

static inline void infiltratr_store_be32(void *bytes, uint32_t value)
{
    uint8_t *p = (uint8_t *)bytes;
    p[0] = (uint8_t)(value >> 24);
    p[1] = (uint8_t)(value >> 16);
    p[2] = (uint8_t)(value >> 8);
    p[3] = (uint8_t)value;
}

static inline void infiltratr_store_be64(void *bytes, uint64_t value)
{
    uint8_t *p = (uint8_t *)bytes;
    infiltratr_store_be32(p, (uint32_t)(value >> 32));
    infiltratr_store_be32(p + 4, (uint32_t)value);
}

#ifdef __cplusplus
}
#endif

#endif
