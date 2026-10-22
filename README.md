# epoch

Part of [Kronuz C++ Libraries](https://github.com/Kronuz/cpp-libraries), a collection of small, reusable C++ libraries.

[![CI](https://github.com/Kronuz/epoch/actions/workflows/ci.yml/badge.svg)](https://github.com/Kronuz/epoch/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)

`epoch::now()`: Unix time as a plain integer, in the unit you ask for, in C++20.

## ✨ Highlights

- **Seconds by default**; `epoch::now<std::chrono::milliseconds>()` for any other
  unit.
- **A plain integer**, ready for a timestamp field or a log line.
- **One line of code**, on `std::chrono::system_clock`.

## 📦 Installation

Requires a C++20 compiler; CMake 3.14 or newer for the CMake route. CI builds it
on Linux (x86_64, arm64), macOS (arm64, x86_64), and FreeBSD, plus ASan, UBSan,
and TSan builds.

```cmake
include(FetchContent)
FetchContent_Declare(epoch
  GIT_REPOSITORY https://github.com/Kronuz/epoch.git
  GIT_TAG        main)
FetchContent_MakeAvailable(epoch)

target_link_libraries(my_app PRIVATE epoch::epoch)
```

## 🚀 Quick start

```cpp
#include "epoch.hh"

#include <chrono>
#include <cstdio>

int main() {
	std::printf("%lld s\n", static_cast<long long>(epoch::now()));
	std::printf("%lld ms\n", static_cast<long long>(epoch::now<std::chrono::milliseconds>()));
}
```

## 🎯 When to use it

- ✅ Timestamps for records and logs.
- ❌ Measuring durations: the system clock can jump; use
  `std::chrono::steady_clock`.

## 📚 Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md): why one template instead of a family of
  functions.
- [AGENTS.md](AGENTS.md): repository map.

## 🛠️ Building and testing

```sh
cmake -B build && cmake --build build && ctest --test-dir build
```

## 🤝 Contributing

Issues and pull requests are welcome. Read [AGENTS.md](AGENTS.md) first. CI must
pass on every platform.

## 📄 License

MIT, Copyright (c) 2015-2018 Dubalu LLC. See [LICENSE](LICENSE). Extracted from
[Xapiand](https://github.com/Kronuz/Xapiand).
