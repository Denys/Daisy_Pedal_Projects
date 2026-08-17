#pragma once

#include "device_capability_descriptor.hpp"
#include "target_selection_contract.hpp"

#include <cstdint>
#include <limits>

namespace synth_harness {

constexpr TargetId CompiledTargetId() {
#if defined(VARIANT_TERRARIUM)
    return TargetId::Terrarium;
#elif defined(VARIANT_1590B)
    return TargetId::Pedal1590B;
#elif defined(VARIANT_1590B_SMD)
    return TargetId::Pedal1590BSmd;
#elif defined(VARIANT_FUNBOX)
    return TargetId::Funbox;
#else
    return TargetId::Pedal125B;
#endif
}

template <typename Hardware>
RuntimeCarrierFacts ObserveCarrierFacts(Hardware& hardware, std::uint16_t storage_region_words) {
    const auto narrow_count = [](int value) -> std::uint8_t {
        if (value < 0 || value > static_cast<int>(std::numeric_limits<std::uint8_t>::max())) return 0;
        return static_cast<std::uint8_t>(value);
    };
    return {{2, 2,
             narrow_count(hardware.GetKnobCount()),
             narrow_count(hardware.GetSwitchCount()),
             narrow_count(hardware.GetEncoderCount()),
             narrow_count(hardware.GetLedCount()),
             storage_region_words},
            hardware.SupportsStereo(),
            hardware.SupportsMidi(),
            hardware.SupportsDisplay(),
            hardware.SupportsEncoder(),
            hardware.SupportsTrueBypass()};
}

template <typename Hardware>
Descriptor BuildSourceDescriptor(Hardware& hardware, std::uint16_t storage_region_words,
                                 std::uint32_t layout_hash) {
    return MakeSourceBackedDescriptor(CompiledTargetId(),
                                      ObserveCarrierFacts(hardware, storage_region_words),
                                      layout_hash);
}

} // namespace synth_harness
