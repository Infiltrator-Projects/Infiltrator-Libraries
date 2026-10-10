#!/usr/bin/env python3
from pathlib import Path


def replace_once(path: str, old: str, new: str) -> None:
    p = Path(path)
    text = p.read_text(encoding="utf-8")
    count = text.count(old)
    if count != 1:
        raise SystemExit(f"{path}: expected exactly one match, found {count}")
    p.write_text(text.replace(old, new, 1), encoding="utf-8")


replace_once(
    "src/temporal.c",
    '    { "chinese-time", "Traditional Chinese double-hours (Zǐ begins 23:00; 12 shíchén)", false, false, false },\n'
    '    { "chinese-ke", "Historical Chinese hundred-kè variant (100 equal kè per day; finer subdivisions varied by era)", false, false, false },\n'
    '    { "roman-temporal", "Roman seasonal time (12 unequal daylight hours; night in 4 military watches)", false, true, true },',
    '    { "chinese-time", "Traditional Chinese double-hours (Zǐ begins 23:00; 初/正 half-shí precision)", true, false, false },\n'
    '    { "chinese-ke", "Historical Chinese hundred-kè variant (100 equal kè per day; finer subdivisions varied by era)", false, false, false },\n'
    '    { "roman-temporal", "Roman seasonal time (12 unequal daylight hours; night in 4 military watches)", true, true, true },',
)

replace_once(
    "src/temporal_clock.c",
    '''typedef struct SeasonalPeriod {
    bool daylight;
    unsigned index;
} SeasonalPeriod;''',
    '''typedef struct SeasonalPeriod {
    bool daylight;
    unsigned index;
    long double fraction;
} SeasonalPeriod;''',
)

replace_once(
    "src/temporal_clock.c",
    '''    unit = span / parts;
    period->index = (unsigned)floorl(position / unit);
    if (period->index >= parts) {
        period->index = parts - 1U;
    }
    return true;''',
    '''    unit = span / parts;
    {
        long double scaled = position / unit;

        if (scaled < 0.0L) {
            scaled = 0.0L;
        }
        if (scaled >= (long double)parts) {
            scaled = nextafterl((long double)parts, 0.0L);
        }
        period->index = (unsigned)floorl(scaled);
        period->fraction = scaled - (long double)period->index;
    }
    return true;''',
)

replace_once(
    "src/temporal_clock.c",
    '''    if (strcmp(mode, "chinese-time") == 0) {
        static const char *const branches[] = {
            "子 Zǐ (Rat)", "丑 Chǒu (Ox)", "寅 Yín (Tiger)",
            "卯 Mǎo (Rabbit)", "辰 Chén (Dragon)", "巳 Sì (Snake)",
            "午 Wǔ (Horse)", "未 Wèi (Goat)", "申 Shēn (Monkey)",
            "酉 Yǒu (Rooster)", "戌 Xū (Dog)", "亥 Hài (Boar)"
        };
        const int64_t shifted = positive_modulo(
            local + SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND,
            MICROSECONDS_PER_DAY);
        const unsigned branch = (unsigned)(shifted /
            (INT64_C(2) * SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND));

        if (!vertical) {
            return write_text(buffer, capacity, length, branches[branch]);
        }
        {
            const char *first = strchr(branches[branch], ' ');
            const char *second_space =
                first != NULL ? strchr(first + 1, ' ') : NULL;

            if (first == NULL || second_space == NULL) {
                return write_text(
                    buffer, capacity, length, branches[branch]);
            }
            return write_printf(
                buffer, capacity, length, "%.*s\\n%.*s\\n%s",
                (int)(first - branches[branch]), branches[branch],
                (int)(second_space - first - 1), first + 1,
                second_space + 1);
        }
    }''',
    '''    if (strcmp(mode, "chinese-time") == 0) {
        static const char *const branches[] = {
            "子", "丑", "寅", "卯", "辰", "巳",
            "午", "未", "申", "酉", "戌", "亥"
        };
        const int64_t double_hour =
            INT64_C(2) * SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND;
        const int64_t shifted = positive_modulo(
            local + SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND,
            MICROSECONDS_PER_DAY);
        const unsigned branch = (unsigned)(shifted / double_hour);
        const int64_t within = shifted % double_hour;
        const char *suffix = show_seconds
            ? (within < SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND
                   ? "初" : "正")
            : "時";

        return write_printf(
            buffer, capacity, length,
            vertical ? "%s\\n%s" : "%s%s",
            branches[branch], suffix);
    }''',
)

replace_once(
    "src/temporal_clock.c",
    '''    if (strcmp(mode, "roman-temporal") == 0) {
        static const char *const roman[] = {
            "I", "II", "III", "IV", "V", "VI",
            "VII", "VIII", "IX", "X", "XI", "XII"
        };
        SeasonalPeriod period;

        if (!seasonal_period_at(
                unix_microseconds, latitude, longitude,
                0.833, 12U, 4U, &period)) {
            return write_text(
                buffer, capacity, length,
                vertical ? "N/A\\nROM" : "N/A ROM");
        }
        return write_printf(
            buffer, capacity, length,
            period.daylight
                ? (vertical ? "Hora\\n%s" : "Hora %s")
                : (vertical ? "Vigilia\\n%s" : "Vigilia %s"),
            roman[period.index]);
    }''',
    '''    if (strcmp(mode, "roman-temporal") == 0) {
        static const char *const roman[] = {
            "I", "II", "III", "IV", "V", "VI",
            "VII", "VIII", "IX", "X", "XI", "XII"
        };
        SeasonalPeriod period;

        if (!seasonal_period_at(
                unix_microseconds, latitude, longitude,
                0.833, 12U, 4U, &period)) {
            return write_text(
                buffer, capacity, length,
                vertical ? "N/A\\nROM" : "N/A ROM");
        }

        /*
         * Roman daylight horae were seasonal, so finer presentation must
         * subdivide the current unequal hora rather than append modern
         * minutes. An uncia is one twelfth of an hour. Night remains in the
         * selected four-watch civil convention because a fractional military
         * watch would manufacture a precision convention this profile does
         * not claim historically.
         */
        if (show_seconds && period.daylight) {
            unsigned uncia =
                (unsigned)floorl(period.fraction * 12.0L);

            if (uncia > 11U) {
                uncia = 11U;
            }
            if (uncia != 0U) {
                return write_printf(
                    buffer, capacity, length,
                    vertical ? "Hora\\n%s\\n%s %s"
                             : "Hora %s · %s %s",
                    roman[period.index], roman[uncia - 1U],
                    uncia == 1U ? "uncia" : "unciae");
            }
        }

        return write_printf(
            buffer, capacity, length,
            period.daylight
                ? (vertical ? "Hora\\n%s" : "Hora %s")
                : (vertical ? "Vigilia\\n%s" : "Vigilia %s"),
            roman[period.index]);
    }''',
)

replace_once(
    "tests/temporal_smoke.c",
    '''    CHECK(strcmp(
        infiltratr_temporal_clock_mode_find("roman-temporal")->name,
        "Roman seasonal time (12 unequal daylight hours; night in 4 military watches)") == 0);''',
    '''    CHECK(strcmp(
        infiltratr_temporal_clock_mode_find("roman-temporal")->name,
        "Roman seasonal time (12 unequal daylight hours; night in 4 military watches)") == 0);
    CHECK(infiltratr_temporal_clock_mode_find("roman-temporal")->supports_seconds);
    CHECK(infiltratr_temporal_clock_mode_find("chinese-time")->supports_seconds);''',
)

replace_once(
    "tests/temporal_smoke.c",
    '''    CHECK(infiltratr_temporal_format_clock_mode(
        "roman-temporal",
        INT64_C(1789990000) * INT64_C(1000000),
        36000, false, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strncmp(text, "Hora ", 5U) == 0 ||
          strncmp(text, "Vigilia ", 8U) == 0);

    CHECK(!infiltratr_temporal_format_clock_mode(''',
    '''    CHECK(infiltratr_temporal_format_clock_mode(
        "chinese-time",
        INT64_C(30) * INT64_C(60) * INT64_C(1000000),
        0, false, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "子時") == 0);
    CHECK(infiltratr_temporal_format_clock_mode(
        "chinese-time",
        INT64_C(30) * INT64_C(60) * INT64_C(1000000),
        0, true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "子正") == 0);
    CHECK(infiltratr_temporal_format_clock_mode(
        "chinese-time",
        (INT64_C(23) * INT64_C(3600) + INT64_C(30) * INT64_C(60)) *
            INT64_C(1000000),
        0, true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "子初") == 0);

    CHECK(infiltratr_temporal_format_clock_mode(
        "roman-temporal",
        INT64_C(1789990000) * INT64_C(1000000),
        36000, false, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strncmp(text, "Hora ", 5U) == 0 ||
          strncmp(text, "Vigilia ", 8U) == 0);

    CHECK(infiltratr_temporal_format_clock_mode(
        "roman-temporal",
        (INT64_C(12) * INT64_C(3600) + INT64_C(30) * INT64_C(60)) *
            INT64_C(1000000),
        0, true, false, true, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strncmp(text, "Hora ", 5U) == 0);
    CHECK(strstr(text, "uncia") != NULL);
    CHECK(strchr(text, ':') == NULL);

    CHECK(!infiltratr_temporal_format_clock_mode(''',
)

replace_once(
    "include/infiltratr/temporal.h",
    ''' * @vertical selects newline-separated clock fields for narrow presentations.
 * Applications that render ordinary rows or timestamps should pass false.
 */''',
    ''' * @show_seconds requests the clock mode's closest meaningful finer
 * presentation. Historical modes may therefore use native subdivisions rather
 * than literal modern SI seconds.
 * @vertical selects newline-separated clock fields for narrow presentations.
 * Applications that render ordinary rows or timestamps should pass false.
 */''',
)

replace_once(
    "CHANGELOG.md",
    '''## Unreleased

No unreleased changes.''',
    '''## Unreleased

- Render Traditional Chinese double-hours as document-style Earthly-Branch time (`子時`) and use the attested `初`/`正` half-shí distinction when finer precision is requested, instead of embedding pinyin and English zodiac teaching labels in the timestamp.
- Give Roman seasonal daylight time a native finer presentation using twelfths of the current unequal hora (`unciae`) while retaining the profile's four-watch night convention.
- Advertise native finer-precision support for the Chinese double-hour and Roman seasonal profiles so system-wide presentation controls match formatter behaviour.''',
)
