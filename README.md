# epoch

Part of [Kronuz C++ Libraries](https://github.com/Kronuz/cpp-libraries), a collection of small, reusable C++ libraries.

[![CI](https://github.com/Kronuz/epoch/actions/workflows/ci.yml/badge.svg)](https://github.com/Kronuz/epoch/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)

Header-only Unix time for C++20: epoch::now() reads the system clock as an integer count in any std::chrono unit.
It is one function template over `std::chrono::system_clock`, seconds by
default, that returns a bare number instead of a `time_point`. Reach for it when
a record, a log line, or a cache entry needs a timestamp field.

## ✨ Highlights

- **Seconds by default.** `epoch::now()` returns whole seconds since
  1970-01-01 00:00:00 UTC.
- **Any unit.** `epoch::now<std::chrono::milliseconds>()`, or any other
  `std::chrono::duration`, picks the resolution; `duration_cast` truncates
  toward zero.
- **A plain integer.** The result is the unit's `rep`, a signed integer, ready
  for a timestamp field or a log line.
- **One clock read.** The function is `inline` and `noexcept`, and depends only
  on `<chrono>`.

## 🚀 Quick start

```cpp
#include "epoch.hh"

#include <chrono>
#include <cstdio>

int main() {
	long long s = epoch::now();                                  // e.g. 1793533378
	long long ms = epoch::now<std::chrono::milliseconds>();      // e.g. 1793533378412
	std::printf("%lld s, %lld ms\n", s, ms);
}
```

## 📦 Installation

Requires a C++20 compiler and CMake 3.14 or newer; no dependencies. CI builds it
on Linux (x86_64, arm64), macOS (arm64, x86_64), and FreeBSD, plus ASan, UBSan,
and TSan builds.

```cmake
include(FetchContent)
FetchContent_Declare(epoch
  GIT_REPOSITORY https://github.com/Kronuz/epoch.git
  GIT_TAG        4f9e76afd406e3fc296c01c557df121281e849fb)  # or main for the tip
FetchContent_MakeAvailable(epoch)

target_link_libraries(my_app PRIVATE epoch::epoch)
```

```cpp
#include "epoch.hh"
```

## 🎯 When to use it

Use it for wall-clock timestamps: record fields, log lines, expiry times.

Not for:

- **Measuring durations.** The system clock can jump when the host adjusts its
  time; use `std::chrono::steady_clock` for elapsed time.
- **Typed time points.** The result is a bare count with the unit in the call;
  keep a `std::chrono::time_point` when the type should carry the unit.
- **Rounding.** A coarser unit drops the remainder (seconds discards the
  fraction); it does not round to nearest.

## 📚 Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md): the one function, its return type, and
  truncation.
- [AGENTS.md](AGENTS.md): repository map and invariants.
- [Catalog entry](https://github.com/Kronuz/cpp-libraries/blob/main/AGENTS.md#epoch)
  in Kronuz C++ Libraries.

## 🛠️ Building and testing

```sh
cmake -B build
cmake --build build
ctest --test-dir build
```

## 🤝 Contributing

Issues and pull requests are welcome. Read [AGENTS.md](AGENTS.md) and the
catalog's [rules of engagement](https://github.com/Kronuz/cpp-libraries/blob/main/AGENTS.md#rules-of-engagement)
first; CI must pass on every platform.

## 📄 License

MIT, Copyright (c) 2026 Germán Méndez Bravo (Kronuz) and Copyright (c) 2015-2020
Dubalu LLC. See [LICENSE](LICENSE). Extracted from
[Xapiand](https://github.com/Kronuz/Xapiand).
