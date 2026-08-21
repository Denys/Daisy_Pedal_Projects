#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace synth_harness {

enum class TargetId : std::uint16_t {
    Pedal125B = 1,
    Pedal1590B = 2,
    Pedal1590BSmd = 3,
    Terrarium = 4,
    Funbox = 5,
};

enum class Support : std::uint8_t { Unknown = 0, Unsupported = 1, Supported = 2 };
enum class Evidence : std::uint8_t { Unavailable = 0, ImplementedSource = 1 };

struct CapabilityRecord {
    std::uint8_t id;
    std::uint8_t scope;
    Support support;
    Evidence evidence;
};

struct RoleRecord {
    std::uint8_t id;
    std::uint8_t physical_index;
    Support support;
    Evidence evidence;
};

struct ResourceCounts {
    std::uint8_t callback_inputs;
    std::uint8_t callback_outputs;
    std::uint8_t analog_controls;
    std::uint8_t switches;
    std::uint8_t encoders;
    std::uint8_t leds;
    std::uint16_t storage_region_words;
};

struct LayoutIdentity {
    std::uint8_t state;
    std::uint16_t algorithm;
    std::uint32_t value;
};

struct RuntimeCarrierFacts {
    ResourceCounts resources;
    bool stereo;
    bool midi;
    bool display;
    bool encoder;
    bool true_bypass;
};

struct Descriptor {
    TargetId target;
    TargetId compiled_for_carrier;
    ResourceCounts resources;
    LayoutIdentity layout_identity;
    std::array<CapabilityRecord, 23> capabilities;
    std::array<RoleRecord, 2> roles;
};

enum class SerializeStatus { Ok, OutputTooSmall, InvalidDescriptor };

struct SerializeResult {
    SerializeStatus status;
    std::size_t bytes_written;
};

struct EffectLayoutView {
    std::string_view name;
    std::span<const std::uint32_t> parameter_types;
};

enum class ParseStatus {
    Ok,
    BadMagic,
    UnsupportedVersion,
    BadHeader,
    SizeMismatch,
    Oversize,
    BadTagOrder,
    DuplicateTag,
    MissingCoreTag,
    UnknownCriticalTag,
    BadFlags,
    BadLength,
    BadEnum,
    BadIdentityState,
    BadCapability,
    BadRole,
    TargetCarrierMismatch,
};

struct ParseResult {
    struct StatusField {
        ParseStatus value;
        Descriptor* descriptor;

        constexpr StatusField(ParseStatus initial, Descriptor* owner) noexcept
            : value(initial), descriptor(owner) {}

        StatusField(const StatusField& other) noexcept
            : value(other.value), descriptor(nullptr) {}

        StatusField& operator=(const StatusField& other) noexcept {
            return *this = other.value;
        }

        StatusField& operator=(ParseStatus next) noexcept {
            value = next;
            if (next != ParseStatus::Ok && descriptor != nullptr)
                descriptor->target = static_cast<TargetId>(0);
            return *this;
        }

        constexpr operator ParseStatus() const noexcept { return value; }

        friend constexpr bool operator==(const StatusField& lhs, ParseStatus rhs) noexcept {
            return lhs.value == rhs;
        }
        friend constexpr bool operator!=(const StatusField& lhs, ParseStatus rhs) noexcept {
            return lhs.value != rhs;
        }
    };

    Descriptor descriptor{};
    StatusField status{ParseStatus::BadHeader, &descriptor};

    ParseResult(ParseStatus initial, Descriptor parsed = {}) noexcept
        : descriptor(parsed), status{initial, &descriptor} {
        if (initial != ParseStatus::Ok)
            descriptor.target = static_cast<TargetId>(0);
    }

    ParseResult(const ParseResult& other) noexcept
        : descriptor(other.descriptor), status{other.status.value, &descriptor} {}

    ParseResult& operator=(const ParseResult& other) noexcept {
        if (this == &other) return *this;
        descriptor = other.descriptor;
        status.value = other.status.value;
        status.descriptor = &descriptor;
        return *this;
    }

    ParseResult(ParseResult&& other) noexcept : ParseResult(other) {}
    ParseResult& operator=(ParseResult&& other) noexcept { return *this = other; }
};

constexpr std::size_t kCanonicalDescriptorBytes = 238;
constexpr std::size_t kMaximumDescriptorBytes = 1024;

Descriptor MakeSourceBackedDescriptor(TargetId target, const RuntimeCarrierFacts& facts,
                                      std::uint32_t layout_hash = 0);
bool IsValidDescriptor(const Descriptor& descriptor);
SerializeResult Serialize(const Descriptor& descriptor, std::span<std::uint8_t> output);
ParseResult Parse(std::span<const std::uint8_t> bytes);
bool AuthorizesPhysicalOperation(const Descriptor& descriptor);
bool AuthorizesPresetMutation(const Descriptor& descriptor, std::uint32_t expected_layout_hash);
std::uint32_t ComputeEffectLayoutHash(std::span<const EffectLayoutView> effects);

} // namespace synth_harness
