#include "control_inventory.hpp"

namespace synth_harness {
namespace {

constexpr std::uint8_t kBypassRoleId = 1;
constexpr std::uint8_t kAlternateRoleId = 2;

bool HasConflictingRoles(const Descriptor& descriptor) {
    const auto& bypass = descriptor.roles[0];
    const auto& alternate = descriptor.roles[1];
    return bypass.id != kBypassRoleId || alternate.id != kAlternateRoleId ||
           (bypass.support == Support::Supported &&
            alternate.support == Support::Supported &&
            bypass.physical_index == alternate.physical_index);
}

SemanticRole RoleForSwitch(const Descriptor& descriptor, std::uint8_t index) {
    if (descriptor.roles[0].support == Support::Supported &&
        descriptor.roles[0].physical_index == index)
        return SemanticRole::Bypass;
    if (descriptor.roles[1].support == Support::Supported &&
        descriptor.roles[1].physical_index == index)
        return SemanticRole::Alternate;
    return SemanticRole::None;
}

} // namespace

InventoryResult EnumerateControls(const Descriptor& descriptor,
                                  std::span<ControlRecord> output) {
    if (!IsValidDescriptor(descriptor) || HasConflictingRoles(descriptor))
        return {InventoryStatus::InvalidDescriptor, 0, 0};

    const bool has_midi = descriptor.capabilities[1].support == Support::Supported;
    const std::size_t required =
        static_cast<std::size_t>(descriptor.resources.callback_inputs) +
        static_cast<std::size_t>(descriptor.resources.callback_outputs) +
        static_cast<std::size_t>(descriptor.resources.analog_controls) +
        static_cast<std::size_t>(descriptor.resources.switches) +
        static_cast<std::size_t>(descriptor.resources.encoders) +
        static_cast<std::size_t>(descriptor.resources.leds) + (has_midi ? 2u : 0u);

    if (required > kMaximumControlRecords)
        return {InventoryStatus::TooManyControls, 0, required};
    if (output.size() < required)
        return {InventoryStatus::OutputTooSmall, 0, required};

    std::size_t cursor = 0;
    const auto append = [&](ControlKind kind, std::uint8_t index,
                            SemanticRole role = SemanticRole::None) {
        output[cursor++] = {kind, index, role, Support::Supported,
                            Evidence::ImplementedSource};
    };

    for (std::uint8_t i = 0; i < descriptor.resources.callback_inputs; ++i)
        append(ControlKind::AudioInput, i);
    for (std::uint8_t i = 0; i < descriptor.resources.callback_outputs; ++i)
        append(ControlKind::AudioOutput, i);
    for (std::uint8_t i = 0; i < descriptor.resources.analog_controls; ++i)
        append(ControlKind::AnalogInput, i);
    for (std::uint8_t i = 0; i < descriptor.resources.switches; ++i)
        append(ControlKind::SwitchInput, i, RoleForSwitch(descriptor, i));
    for (std::uint8_t i = 0; i < descriptor.resources.encoders; ++i)
        append(ControlKind::EncoderInput, i);
    for (std::uint8_t i = 0; i < descriptor.resources.leds; ++i)
        append(ControlKind::LedOutput, i);
    if (has_midi) {
        append(ControlKind::MidiInput, 0);
        append(ControlKind::MidiOutput, 0);
    }

    return {InventoryStatus::Ok, cursor, required};
}

} // namespace synth_harness
