#pragma once

#include "device_capability_descriptor.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace synth_harness {

enum class ControlKind : std::uint8_t {
    AudioInput,
    AudioOutput,
    AnalogInput,
    SwitchInput,
    EncoderInput,
    LedOutput,
    MidiInput,
    MidiOutput,
};

enum class SemanticRole : std::uint8_t { None, Bypass, Alternate };

struct ControlRecord {
    ControlKind kind;
    std::uint8_t physical_index;
    SemanticRole role;
    Support support;
    Evidence evidence;

    bool operator==(const ControlRecord&) const = default;
};

enum class InventoryStatus { Ok, OutputTooSmall, InvalidDescriptor, TooManyControls };

struct InventoryResult {
    InventoryStatus status;
    std::size_t records_written;
    std::size_t records_required;
};

constexpr std::size_t kMaximumControlRecords = 32;

InventoryResult EnumerateControls(const Descriptor& descriptor,
                                  std::span<ControlRecord> output);

} // namespace synth_harness
