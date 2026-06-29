# Architecture

The internal design of `epoch`: a single header with one templated function that
turns the system clock into a plain integer count since the Unix epoch. For usage
see `README.md`; for the repo map and invariants see `AGENTS.md`.

## Shape

One header, header-only, nothing behind it but `<chrono>`:

```
  epoch.hh   one templated epoch::now<Period>() over std::chrono::system_clock
```

## The one function

`epoch::now<Period>()` does three things in a single expression:

```cpp
return std::chrono::duration_cast<Period>(
           std::chrono::system_clock::now().time_since_epoch()
       ).count();
```

First it reads `system_clock::now()`, a `time_point` on the system clock. Then it
takes `.time_since_epoch()`, the `duration` from that clock's epoch (the Unix
epoch, 1970-01-01T00:00:00Z, on every mainstream implementation) to now. Then it
`duration_cast`s that to `Period` and returns `.count()`, so the caller gets a
bare integer in the requested unit rather than a `duration` or `time_point` they
have to unwrap.

`Period` defaults to `std::chrono::seconds`, which makes `epoch::now()` read as
"Unix time as a number." Pass `milliseconds`, `microseconds`, `nanoseconds`, or
any other `std::chrono` duration to change the resolution.

## Why a template over a duration, not a fixed unit

Folding the unit into a template parameter keeps the surface to one name while
letting the call site choose resolution. `duration_cast` does the unit math at
compile time, so `epoch::now<std::chrono::milliseconds>()` is the same single
clock read as the seconds form, just cast to a different ratio. The alternative,
a family of `now_seconds()` / `now_millis()` / ... functions, would be more names
for no gain; the template collapses them into one.

## Return type and truncation

The return type is whatever `duration_cast<Period>(...).count()` yields, a signed
integer wide enough for that unit. `duration_cast` truncates toward zero, so a
coarser `Period` drops the sub-unit remainder (seconds throws away the fractional
second). That is the intended "what time is it, in these units" behavior, not a
rounding helper.

## Why `inline` and `noexcept`

The function is `inline` because the header is meant to be included in many
translation units with no separate `.cc`; `inline` lets every TU carry its own
definition without a one-definition-rule clash. It is `noexcept` because reading
the system clock and casting a duration don't throw, so callers can use it on
hot paths and in `noexcept` contexts without a swallowed-exception worry.
