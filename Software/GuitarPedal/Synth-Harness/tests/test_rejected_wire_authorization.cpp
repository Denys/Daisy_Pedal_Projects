#include "device_capability_descriptor.hpp"

#include <array>
#include <cstdint>
#include <iostream>

using namespace synth_harness;

int main() {
    const RuntimeCarrierFacts facts{{2, 2, 6, 2, 1, 2, 1024}, true, true, true, true, true};
    constexpr std::uint32_t kLayoutHash = 0x12345678u;
    const auto descriptor = MakeSourceBackedDescriptor(TargetId::Pedal125B, facts, kLayoutHash);

    std::array<std::uint8_t, kCanonicalDescriptorBytes> canonical{};
    const auto serialized = Serialize(descriptor, canonical);
    if (serialized.status != SerializeStatus::Ok || serialized.bytes_written != canonical.size()) {
        std::cerr << "FAIL: canonical serialization\n";
        return 1;
    }

    // A copied StatusField is a detached value snapshot. It must not retain the
    // ParseResult descriptor pointer or mutate the ParseResult when reassigned.
    auto valid_parse = Parse(canonical);
    if (valid_parse.status != ParseStatus::Ok || !IsValidDescriptor(valid_parse.descriptor)) {
        std::cerr << "FAIL: valid parse baseline\n";
        return 1;
    }
    auto saved_status = valid_parse.status;
    if (saved_status.descriptor != nullptr) {
        std::cerr << "FAIL: copied status retained ParseResult descriptor pointer\n";
        return 1;
    }
    saved_status = ParseStatus::BadEnum;
    if (valid_parse.status != ParseStatus::Ok || !IsValidDescriptor(valid_parse.descriptor)) {
        std::cerr << "FAIL: detached status copy mutated source ParseResult\n";
        return 1;
    }

    // Extracting status from a temporary ParseResult must likewise leave no
    // pointer that can dangle after the temporary is destroyed.
    auto temporary_status = Parse(canonical).status;
    if (temporary_status.descriptor != nullptr) {
        std::cerr << "FAIL: temporary status copy retained dangling descriptor pointer\n";
        return 1;
    }
    temporary_status = ParseStatus::BadEnum;
    if (temporary_status != ParseStatus::BadEnum) {
        std::cerr << "FAIL: detached temporary status is not independently assignable\n";
        return 1;
    }

    // Append an unknown critical TLV after all valid core TLVs. The parser must
    // reject the wire input and invalidate the partially reconstructed descriptor.
    std::array<std::uint8_t, kCanonicalDescriptorBytes + 7> rejected_wire{};
    for (std::size_t i = 0; i < canonical.size(); ++i) rejected_wire[i] = canonical[i];
    const auto total = static_cast<std::uint16_t>(rejected_wire.size());
    rejected_wire[8] = static_cast<std::uint8_t>(total);
    rejected_wire[9] = static_cast<std::uint8_t>(total >> 8);

    std::size_t cursor = canonical.size();
    rejected_wire[cursor++] = 0x00; // tag 0x1000
    rejected_wire[cursor++] = 0x10;
    rejected_wire[cursor++] = 0x01; // critical flag
    rejected_wire[cursor++] = 0x00;
    rejected_wire[cursor++] = 0x01; // one-byte payload
    rejected_wire[cursor++] = 0x00;
    rejected_wire[cursor] = 0xaa;

    const auto parsed = Parse(rejected_wire);
    if (parsed.status != ParseStatus::UnknownCriticalTag) {
        std::cerr << "FAIL: expected UnknownCriticalTag\n";
        return 1;
    }
    if (IsValidDescriptor(parsed.descriptor)) {
        std::cerr << "FAIL: rejected parse retained a valid descriptor\n";
        return 1;
    }
    if (AuthorizesPresetMutation(parsed.descriptor, kLayoutHash)) {
        std::cerr << "FAIL: rejected wire descriptor authorized preset mutation\n";
        return 1;
    }

    std::cout << "PASS rejected-wire authorization and status copies stay fail-closed\n";
    return 0;
}
