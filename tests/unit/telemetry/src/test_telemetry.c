#include <zephyr/ztest.h>
#include "telemetry.h"

ZTEST(telemetry, test_should_log_every_10_samples)
{
    int counter = 0;

    for (int i = 1; i <= 9; i++) {
        zassert_false(telemetry_should_log(&counter), "should not log at %d", i);
    }

    zassert_true(telemetry_should_log(&counter), "should log at 10");

    for (int i = 11; i <= 19; i++) {
        zassert_false(telemetry_should_log(&counter), "should not log at %d", i);
    }

    zassert_true(telemetry_should_log(&counter), "should log at 20");
}

ZTEST_SUITE(telemetry, NULL, NULL, NULL, NULL, NULL);

