// SPDX-License-Identifier: GPL-3.0-or-later
/** @file completion_contract.c @brief Common 1.15 completion-family coverage. */
#include "infiltratr/arithmetic.h"
#include "infiltratr/endian.h"
#include "infiltratr/quantity.h"
#include "infiltratr/token.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_endian_load_store(void)
{
    uint8_t bytes[8] = {0};
    infiltratr_store_be64(bytes, UINT64_C(0x0123456789abcdef));
    assert(bytes[0] == 0x01U && bytes[7] == 0xefU);
    assert(infiltratr_load_be64(bytes) == UINT64_C(0x0123456789abcdef));
    infiltratr_store_le64(bytes, UINT64_C(0x0123456789abcdef));
    assert(bytes[0] == 0xefU && bytes[7] == 0x01U);
    assert(infiltratr_load_le64(bytes) == UINT64_C(0x0123456789abcdef));

    const uint16_t be16 = infiltratr_cpu_to_be16(UINT16_C(0x1234));
    const uint32_t be32 = infiltratr_cpu_to_be32(UINT32_C(0x12345678));
    const uint64_t be64 = infiltratr_cpu_to_be64(UINT64_C(0x0123456789abcdef));
    assert(infiltratr_be16_to_cpu(be16) == UINT16_C(0x1234));
    assert(infiltratr_be32_to_cpu(be32) == UINT32_C(0x12345678));
    assert(infiltratr_be64_to_cpu(be64) == UINT64_C(0x0123456789abcdef));
}

static void test_checked_arithmetic(void)
{
    uint64_t result = 77U;
    assert(infiltratr_u64_subtract_checked(10U, 3U, &result) && result == 7U);
    result = 77U;
    assert(!infiltratr_u64_subtract_checked(3U, 10U, &result) && result == 77U);
    assert(infiltratr_u64_multiply_checked(12U, 12U, &result) && result == 144U);
    result = 77U;
    assert(!infiltratr_u64_multiply_checked(UINT64_MAX, 2U, &result));
    assert(result == 77U);
}

static void test_signed_token(void)
{
    const char *text = "  -42,tail";
    const char *cursor = text;
    int64_t value = 17;
    assert(infiltratr_parse_i64_token(&cursor, 10U, &value));
    assert(value == -42 && strcmp(cursor, ",tail") == 0);

    text = "9223372036854775808";
    cursor = text;
    value = 17;
    assert(!infiltratr_parse_i64_token(&cursor, 10U, &value));
    assert(cursor == text && value == 17);
}

static void test_binary_quantity(void)
{
    uint64_t bytes = 0U;
    assert(infiltratr_parse_binary_quantity_u64("32K", &bytes));
    assert(bytes == UINT64_C(32768));
    assert(infiltratr_parse_binary_quantity_u64("1.5 MiB", &bytes));
    assert(bytes == UINT64_C(1572864));
    assert(infiltratr_parse_binary_quantity_u64("8GB", &bytes));
    assert(bytes == UINT64_C(8589934592));
    bytes = 99U;
    assert(!infiltratr_parse_binary_quantity_u64("1.1B", &bytes));
    assert(bytes == 99U);
    assert(!infiltratr_parse_binary_quantity_u64("-1K", &bytes));
    assert(!infiltratr_parse_binary_quantity_u64("9XB", &bytes));
}

int main(void)
{
    test_endian_load_store();
    test_checked_arithmetic();
    test_signed_token();
    test_binary_quantity();
    puts("Infiltratr Common completion contract tests passed.");
    return 0;
}
