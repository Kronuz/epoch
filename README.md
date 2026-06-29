# epoch

A small, header-only, dependency-free **time-since-epoch** helper for C++20,
extracted from [Xapiand](https://github.com/Kronuz/Xapiand).

## What it is

One header, `epoch.hh`: a single templated function `epoch::now<Period>()` that
reads the system clock and hands back the time since the Unix epoch as an integer
count in whatever `std::chrono` duration you ask for. It defaults to seconds, so
`epoch::now()` is "Unix time, right now, as a number." Nothing behind it but
`<chrono>`.

```cpp
namespace epoch {
    template<typename Period = std::chrono::seconds>
    inline auto now() noexcept;
}
```

`Period` is any `std::chrono` duration type. The function casts
`system_clock::now().time_since_epoch()` to that duration and returns `.count()`,
so you get a plain integer in the unit you named, not a `time_point` or a
`duration` you then have to unwrap.

## Install

CMake with `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
  epoch
  GIT_REPOSITORY https://github.com/Kronuz/epoch.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(epoch)

target_link_libraries(your_target PRIVATE epoch::epoch)
```

The `epoch` target is a pure `INTERFACE` library: it compiles nothing, requests
`cxx_std_20`, and puts the header directory on your include path. Then:

```cpp
#include "epoch.hh"
```

Requires C++20. On macOS it builds with AppleClang/libc++, the same toolchain
Xapiand uses. The header keeps its original filename, so a codebase that already
`#include "epoch.hh"` just needs this repo on its include path.

## Usage

```cpp
#include "epoch.hh"

// Unix time in seconds, the default:
auto secs = epoch::now();                            // e.g. 1751200000

// Pick the resolution you need by naming the duration:
auto millis = epoch::now<std::chrono::milliseconds>();
auto micros = epoch::now<std::chrono::microseconds>();
auto nanos  = epoch::now<std::chrono::nanoseconds>();

// Each is a plain integer count since 1970-01-01T00:00:00Z, so it drops straight
// into a timestamp field, a log line, or arithmetic with no further unwrapping.
```

The return type follows from `Period` (whatever `duration_cast<Period>(...)
.count()` yields), so it's a wide signed integer for the finer units.

## Build & test

```sh
cmake -B build && cmake --build build && ctest --test-dir build
```

There is no fixed value to assert against a live clock, so the test checks the
properties that must hold for any wall clock: the count is positive, it sits past
2020 and short of 2100, `now()` defaults to seconds, back-to-back reads don't run
backwards, and the seconds/millis/micros/nanos variants scale by the expected
`1e3` / `1e6` / `1e9`. It prints `all epoch tests passed` and exits 0.

## Provenance

Extracted from [Xapiand](https://github.com/Kronuz/Xapiand). `epoch.hh` had zero
dependencies already (only `<chrono>`), so it was copied verbatim, no decoupling
delta. See [ARCHITECTURE.md](ARCHITECTURE.md) for the design and
[AGENTS.md](AGENTS.md) for the repo map and invariants.

## License

MIT, Copyright (c) 2015-2019 Dubalu LLC. See [LICENSE](LICENSE).
