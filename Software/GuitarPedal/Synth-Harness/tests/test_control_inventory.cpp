#include "control_inventory.hpp"

#include <array>
#include <cstdlib>
#include <iostream>

using namespace synth_harness;

namespace {
int checks = 0;

void Check(bool condition, const char* message) {
    ++checks;
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

std::size_t CountKind(std::span<const ControlRecord> records, ControlKind kind) {
    std::size_t result = 0;
    for (const auto& record : records)
        if (record.kind == kind) ++result;
    return result;
}

const ControlRecord* Find(std::span<const ControlRecord> records, ControlKind kind,
                          std::uint8_t physical_index) {
    for (const auto& record : records)
        if (record.kind == kind && record.physical_index == physical_index) return &record;
    return nullptr;
}
} // namespace

int main() {
    const std::array<TargetId, 5> targets{{TargetId::Pedal125B, TargetId::Pedal1590B,
        TargetId::Pedal1590BSmd, TargetId::Terrarium, TargetId::Funbox}};
    const std::array<RuntimeCarrierFacts, 5> facts{{
        {{2,2,6,2,1,2,1024},true,true,true,true,true},
        {{2,2,4,2,0,2,1024},true,true,false,false,false},
        {{2,2,4,2,0,2,1024},true,true,false,false,true},
        {{2,2,6,6,0,2,1024},false,false,false,false,false},
        {{2,2,7,12,0,2,1024},true,true,false,false,false},
    }};
    const std::array<std::size_t, 5> expected_totals{{17,14,14,18,27}};

    for (std::size_t carrier = 0; carrier < targets.size(); ++carrier) {
        const auto descriptor = MakeSourceBackedDescriptor(targets[carrier], facts[carrier], 0x12345678u);
        std::array<ControlRecord, kMaximumControlRecords> first{};
        std::array<ControlRecord, kMaximumControlRecords> second{};
        const auto a = EnumerateControls(descriptor, first);
        const auto b = EnumerateControls(descriptor, second);
        Check(a.status == InventoryStatus::Ok, "source carrier enumerates");
        Check(a.records_written == expected_totals[carrier], "source carrier total");
        Check(a.records_required == a.records_written, "required equals written");
        Check(b.status == InventoryStatus::Ok && b.records_written == a.records_written,
              "repeat enumeration succeeds");
        for (std::size_t i = 0; i < a.records_written; ++i)
            Check(first[i] == second[i], "enumeration is deterministic");

        const auto records = std::span<const ControlRecord>(first.data(), a.records_written);
        Check(CountKind(records, ControlKind::AudioInput) == facts[carrier].resources.callback_inputs,
              "audio input count");
        Check(CountKind(records, ControlKind::AudioOutput) == facts[carrier].resources.callback_outputs,
              "audio output count");
        Check(CountKind(records, ControlKind::AnalogInput) == facts[carrier].resources.analog_controls,
              "analog count");
        Check(CountKind(records, ControlKind::SwitchInput) == facts[carrier].resources.switches,
              "switch count");
        Check(CountKind(records, ControlKind::EncoderInput) == facts[carrier].resources.encoders,
              "encoder count");
        Check(CountKind(records, ControlKind::LedOutput) == facts[carrier].resources.leds,
              "LED count");
        Check(CountKind(records, ControlKind::MidiInput) == (facts[carrier].midi ? 1u : 0u),
              "MIDI input presence");
        Check(CountKind(records, ControlKind::MidiOutput) == (facts[carrier].midi ? 1u : 0u),
              "MIDI output presence");

        const auto* bypass = Find(records, ControlKind::SwitchInput, 0);
        const auto* alternate = Find(records, ControlKind::SwitchInput, 1);
        Check(bypass != nullptr && bypass->role == SemanticRole::Bypass,
              "bypass semantic role");
        Check(alternate != nullptr && alternate->role == SemanticRole::Alternate,
              "alternate semantic role");
        for (std::uint8_t i = 2; i < facts[carrier].resources.switches; ++i) {
            const auto* generic = Find(records, ControlKind::SwitchInput, i);
            Check(generic != nullptr && generic->role == SemanticRole::None,
                  "additional switch stays generic");
        }
        for (const auto& record : records) {
            Check(record.support == Support::Supported, "enumerated record is supported");
            Check(record.evidence == Evidence::ImplementedSource, "enumerated record keeps source evidence");
        }
    }

    const auto descriptor = MakeSourceBackedDescriptor(TargetId::Pedal125B, facts[0], 0x12345678u);
    std::array<ControlRecord, 16> short_output{};
    short_output.fill({ControlKind::MidiOutput, 0xaa, SemanticRole::Alternate,
                       Support::Unknown, Evidence::Unavailable});
    const auto short_result = EnumerateControls(descriptor, short_output);
    Check(short_result.status == InventoryStatus::OutputTooSmall,
          "short inventory output rejected");
    Check(short_result.records_written == 0 && short_result.records_required == 17,
          "short inventory reports required count");
    Check(short_output.front().physical_index == 0xaa && short_output.back().physical_index == 0xaa,
          "short inventory publishes nothing");

    auto invalid = descriptor;
    invalid.compiled_for_carrier = TargetId::Funbox;
    std::array<ControlRecord, kMaximumControlRecords> invalid_output{};
    invalid_output.fill({ControlKind::MidiOutput, 0x55, SemanticRole::Alternate,
                         Support::Unknown, Evidence::Unavailable});
    const auto invalid_result = EnumerateControls(invalid, invalid_output);
    Check(invalid_result.status == InventoryStatus::InvalidDescriptor &&
          invalid_result.records_written == 0, "invalid descriptor rejected");
    Check(invalid_output.front().physical_index == 0x55,
          "invalid descriptor publishes nothing");

    auto conflicting_roles = descriptor;
    conflicting_roles.roles[1].physical_index = 0;
    const auto conflict_result = EnumerateControls(conflicting_roles, invalid_output);
    Check(conflict_result.status == InventoryStatus::InvalidDescriptor,
          "conflicting semantic roles rejected");

    auto excessive = descriptor;
    excessive.resources.analog_controls = 255;
    const auto excessive_result = EnumerateControls(excessive, invalid_output);
    Check(excessive_result.status == InventoryStatus::TooManyControls &&
          excessive_result.records_required > kMaximumControlRecords,
          "excessive inventory rejected by bounded policy");

    const RuntimeCarrierFacts no_controls{{2,2,0,0,0,0,1024},false,false,false,false,false};
    const auto minimal = MakeSourceBackedDescriptor(TargetId::Terrarium, no_controls);
    std::array<ControlRecord, 4> minimal_output{};
    const auto minimal_result = EnumerateControls(minimal, minimal_output);
    Check(minimal_result.status == InventoryStatus::Ok && minimal_result.records_written == 4,
          "audio-only descriptor enumerates");
    Check(CountKind(minimal_output, ControlKind::MidiInput) == 0,
          "audio-only descriptor has no MIDI record");

    std::cout << "PASS " << checks << " control-inventory checks\n";
}
