# AGENTS.md

Working notes for agents modifying this repository. For the design read
`ARCHITECTURE.md`; for usage read `README.md`. This file covers the repo layout,
how to build and test, the invariants you must not break, and the traps that are
easy to fall into.

## Repo map

```
epoch.hh                     Templated epoch::now<Period>() over std::chrono::system_clock. Header. Verbatim from Xapiand.
test/test.cc                 Runnable smoke test: bounds, default unit, ordering, magnitudes.
CMakeLists.txt               INTERFACE library `epoch` (+ alias epoch::epoch); CTest test `epoch`.
LICENSE                      MIT, Copyright (c) 2015-2019 Dubalu LLC.
README.md                    What it is, install, usage.
ARCHITECTURE.md              Internal design: the one function, the template, return type, inline/noexcept.
```

Everything is header-only. There is no `.cc` to compile except the test. The
CMake target is a pure `INTERFACE` library that only adds the source dir to the
include path and requests `cxx_std_20`.

## Verification and benchmarks

The shared method is the catalog's
[PERFORMANCE.md](https://github.com/Kronuz/cpp-libraries/blob/main/docs/PERFORMANCE.md). From a
fresh clone:

```sh
cmake -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

- **Test.** CTest `epoch` (target `epoch_test`, `test/test.cc`) reads a live clock, so it checks
  properties instead of a value: `now()` in seconds lies between 2020-01-01 and 2100-01-01 (Unix
  constants written into the test), the default unit is seconds, back-to-back reads do not
  decrease, and milliseconds, microseconds and nanoseconds are seconds times 1e3, 1e6 and 1e9
  within a second of slack. A wall-clock step back during the run fails the ordering check; rerun
  it. The checks are `assert`s,
  so leave `NDEBUG` off (no build type, or Debug). Expected output ends with
  `all epoch tests passed`, exit 0.
- **CI.** `.github/workflows/ci.yml` calls the catalog's shared
  [`cmake.yml`](https://github.com/Kronuz/cpp-libraries/blob/main/.github/workflows/cmake.yml):
  Linux x86_64 (GCC, Clang) and arm64 (GCC), macOS arm64 and x86_64, and FreeBSD, at `-O2 -g` with
  asserts live, plus ASan+UBSan and TSan jobs over the same test. The ASan+UBSan job, on Linux:

  ```sh
  CC=clang CXX=clang++ cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug \
    "-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer -D_GLIBCXX_ASSERTIONS" \
    -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined
  cmake --build build-asan && ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build-asan --output-on-failure
  ```

- **Not here.** No differential test, mutation script, fuzz target or benchmark, and no
  performance claim: `now()` is one `system_clock::now()` and a `duration_cast`.

## Conventions

- **C++20.** The target requests `cxx_std_20` to stay uniform with the sibling
  libraries; the header itself is older-standard-friendly, but don't drop the
  target below it.
- **Zero external dependencies.** The only include is `<chrono>`. Don't add
  anything else.
- **Filename is stable.** The header keeps its original Xapiand name
  (`epoch.hh`) so a consumer that already `#include`s it just needs this repo on
  the include path. Don't rename it.
- Tabs for indentation, double quotes in code, no em dashes in prose.

## Load-bearing invariants

- **One templated function, one name.** The surface is exactly
  `epoch::now<Period>()` with `Period` defaulting to `std::chrono::seconds`.
  Don't split it into a family of `now_seconds()` / `now_millis()` helpers; the
  template parameter is the unit selector on purpose.
- **Default is seconds.** `epoch::now()` with no argument returns Unix seconds.
  Changing the default would silently rescale every existing call site. Keep it.
- **Returns a plain count, not a `duration` or `time_point`.** The function ends
  in `.count()`, handing back a bare integer in the requested unit. Callers rely
  on that; don't return the wrapped `std::chrono` type instead.
- **`noexcept` stays.** Reading the clock and casting a duration don't throw, and
  callers use this in `noexcept` contexts and on hot paths. Don't add anything
  that could throw, and keep the `noexcept`.

## Traps

- **`duration_cast` truncates toward zero.** A coarser `Period` drops the
  sub-unit remainder (seconds discards the fractional second). That is intended;
  it is not a rounding function. Don't "fix" it into rounding.
- **The epoch is the system clock's epoch.** On every mainstream implementation
  that is the Unix epoch (1970-01-01T00:00:00Z), but the standard only pins
  `system_clock`'s epoch as of C++20. Don't assume a different anchor.
- **The clock can jump.** `system_clock` is wall-clock time and can move backward
  on an NTP step or manual change. Two reads are not guaranteed monotonic across
  such an event; if you need a never-decreasing source use `steady_clock`
  instead, which is a different tool than this one provides.

## How to extend

- **New resolution? You already have it.** Any `std::chrono` duration works as
  `Period` with no code change, e.g. `epoch::now<std::chrono::minutes>()`. There
  is nothing to add for a new unit.
- **Always extend the smoke test.** `test/test.cc` is the only executable check.
  It asserts clock properties (positivity, sane date bounds, non-decreasing
  back-to-back reads, the magnitude ratios between units) rather than a fixed
  value, since the value is a live clock. Keep that property-based shape when you
  add cases.

## Standalone vs. Xapiand

This is a standalone extraction from
[Xapiand](https://github.com/Kronuz/Xapiand). `epoch.hh` had zero dependencies
already (only `<chrono>`), so it was copied verbatim, there is no decoupling
delta. Keep it that way; any change here should be reconcilable with upstream as
a plain edit.
