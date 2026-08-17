#include "source_device_capability_adapter.hpp"

#ifndef EXPECTED_TARGET
#error "EXPECTED_TARGET is required by the probe"
#endif

int main() {
    return static_cast<int>(synth_harness::CompiledTargetId()) == EXPECTED_TARGET ? 0 : 1;
}
