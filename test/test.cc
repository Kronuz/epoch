// Smoke test for the standalone epoch library.
//
// Exercises epoch.hh: the single templated `epoch::now<Period>()` that reads the
// system clock and returns the time since the Unix epoch as an integer count in
// the requested std::chrono duration (seconds by default). There is no fixed
// value to assert against, so the test checks the properties that must hold for
// any wall clock: the count is positive, it sits past a known recent date and
// short of a far-future one, it doesn't run backwards across calls, and the
// seconds/millis/micros/nanos variants line up in the right orders of magnitude.
//
// Build via CMake: cmake -B build && cmake --build build && ctest --test-dir build
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>

#include "epoch.hh"


// ---------------------------------------------------------------------------
// Sane bounds: now() in seconds must land past a known recent moment and short
// of a far-future one, so a clock reading garbage (or the wrong duration) shows
// up. 2020-01-01 and 2100-01-01 in Unix seconds bracket any plausible run.
// ---------------------------------------------------------------------------

static constexpr int64_t kSeconds2020 = 1577836800LL;   // 2020-01-01T00:00:00Z
static constexpr int64_t kSeconds2100 = 4102444800LL;   // 2100-01-01T00:00:00Z

static void test_bounds() {
	const auto secs = epoch::now<std::chrono::seconds>();
	assert(secs > 0);
	assert(secs > kSeconds2020);
	assert(secs < kSeconds2100);

	std::printf("epoch bounds OK: now() in seconds sits between 2020 and 2100\n");
}


// ---------------------------------------------------------------------------
// Default period is seconds: now() with no template argument must agree with
// now<seconds>(), taken close together so they can differ by at most a tick.
// ---------------------------------------------------------------------------

static void test_default_is_seconds() {
	const auto defaulted = epoch::now();
	const auto explicit_secs = epoch::now<std::chrono::seconds>();
	assert(defaulted >= kSeconds2020);
	// At most one second of drift between the two back-to-back reads.
	const auto diff = explicit_secs - defaulted;
	assert(diff >= 0 && diff <= 1);

	std::printf("epoch default OK: now() defaults to seconds\n");
}


// ---------------------------------------------------------------------------
// Doesn't run backwards: the system clock can jump, but two reads of the same
// period taken in order should not come back decreasing within a single run.
// ---------------------------------------------------------------------------

static void test_monotonic_ish() {
	const auto a = epoch::now<std::chrono::nanoseconds>();
	const auto b = epoch::now<std::chrono::nanoseconds>();
	assert(b >= a);

	std::printf("epoch ordering OK: back-to-back now() does not go backwards\n");
}


// ---------------------------------------------------------------------------
// Orders of magnitude: the same instant expressed in coarser vs. finer units
// must scale by the expected factor. Reads aren't simultaneous, so allow a
// generous slack window rather than an exact ratio.
// ---------------------------------------------------------------------------

static void test_magnitudes() {
	const auto secs   = epoch::now<std::chrono::seconds>();
	const auto millis = epoch::now<std::chrono::milliseconds>();
	const auto micros = epoch::now<std::chrono::microseconds>();
	const auto nanos  = epoch::now<std::chrono::nanoseconds>();

	// Finer units carry strictly larger counts for the same era.
	assert(millis > secs);
	assert(micros > millis);
	assert(nanos > micros);

	// millis ~= secs * 1000, micros ~= secs * 1e6, nanos ~= secs * 1e9. Allow a
	// full second of slack on each (the reads are sequential, not atomic).
	const int64_t slack_ms = 1000;
	const int64_t slack_us = 1000000;
	const int64_t slack_ns = 1000000000;
	assert(millis >= secs * 1000LL && millis <= secs * 1000LL + slack_ms);
	assert(micros >= secs * 1000000LL && micros <= secs * 1000000LL + slack_us);
	assert(nanos >= secs * 1000000000LL && nanos <= secs * 1000000000LL + slack_ns);

	std::printf("epoch magnitudes OK: seconds/millis/micros/nanos scale by 1e3/1e6/1e9\n");
}


int main() {
	test_bounds();
	test_default_is_seconds();
	test_monotonic_ish();
	test_magnitudes();
	std::printf("all epoch tests passed\n");
	return 0;
}
