# Validation

## Validation philosophy

Common is infrastructure for multiple products, so validation is contract-oriented. A passing compile is necessary but insufficient: shared behaviour must be exercised directly and through representative consumers.

## Automated gates

The repository currently uses:

- .github/workflows/ci.yml
- .github/workflows/release.yml

The tests/ tree covers arithmetic, encoding, escaping, formatting, graphics, localisation, timing, token parsing, POSIX I/O/path behaviour, installed CMake consumption and ABI-oriented contracts.

CI is expected to reject warnings, portability regressions, ABI/installation mistakes and behavioural changes that are not accompanied by an intentional contract update.

## Evidence levels

1. Unit/contract evidence — direct regression tests for a primitive.
2. Platform evidence — successful build and execution on the supported host/toolchain.
3. Consumer evidence — an external or in-tree consumer resolves and uses the installed interface correctly.
4. Release evidence — the exact tested commit is the one tagged and published.

A lower evidence level must not be described as proving a higher one.

## Manual validation

Manual testing remains appropriate where a result depends on a real graphical environment, platform integration or a downstream consumer that CI cannot reproduce. Such evidence supplements rather than replaces automated contract tests.

## Release criterion

A release is acceptable when the exact source revision passes the required automated gates, the published package/source identity matches that revision, and no known failing contract is represented as supported.
