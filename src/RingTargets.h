#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class RingHand : std::uint8_t {
    kLeft = 0,
    kRight = 1,
};

enum class RingFinger : std::uint8_t {
    kThumb = 0,
    kIndex = 1,
    kMiddle = 2,
    kRing = 3,
    kPinky = 4,
};

struct RingTarget {
    RingHand hand {RingHand::kLeft};
    RingFinger finger {RingFinger::kIndex};

    [[nodiscard]] bool operator==(const RingTarget&) const = default;
};

inline constexpr std::array kRingTargets {
    RingTarget {RingHand::kLeft, RingFinger::kThumb},
    RingTarget {RingHand::kLeft, RingFinger::kIndex},
    RingTarget {RingHand::kLeft, RingFinger::kMiddle},
    RingTarget {RingHand::kLeft, RingFinger::kRing},
    RingTarget {RingHand::kLeft, RingFinger::kPinky},
    RingTarget {RingHand::kRight, RingFinger::kThumb},
    RingTarget {RingHand::kRight, RingFinger::kIndex},
    RingTarget {RingHand::kRight, RingFinger::kMiddle},
    RingTarget {RingHand::kRight, RingFinger::kRing},
    RingTarget {RingHand::kRight, RingFinger::kPinky},
};

inline constexpr auto kDefaultLeftEquipTarget = RingTarget {RingHand::kLeft, RingFinger::kIndex};
inline constexpr auto kVanillaRingTarget = RingTarget {RingHand::kRight, RingFinger::kIndex};

inline constexpr std::array kVirtualRingTargets {
    RingTarget {RingHand::kLeft, RingFinger::kThumb},
    RingTarget {RingHand::kLeft, RingFinger::kIndex},
    RingTarget {RingHand::kLeft, RingFinger::kMiddle},
    RingTarget {RingHand::kLeft, RingFinger::kRing},
    RingTarget {RingHand::kLeft, RingFinger::kPinky},
    RingTarget {RingHand::kRight, RingFinger::kThumb},
    RingTarget {RingHand::kRight, RingFinger::kMiddle},
    RingTarget {RingHand::kRight, RingFinger::kRing},
    RingTarget {RingHand::kRight, RingFinger::kPinky},
};

[[nodiscard]] constexpr std::uint32_t ToIndex(const RingTarget a_target) {
    const auto handOffset = a_target.hand == RingHand::kLeft ? 0u : 5u;
    return handOffset + static_cast<std::uint32_t>(a_target.finger);
}

[[nodiscard]] constexpr std::optional<RingTarget> FromIndex(const std::uint32_t a_index) {
    if (a_index >= kRingTargets.size()) {
        return std::nullopt;
    }

    return kRingTargets[a_index];
}

[[nodiscard]] constexpr bool IsVirtualRingTarget(const RingTarget a_target) {
    return ToIndex(a_target) != ToIndex(kVanillaRingTarget);
}

[[nodiscard]] constexpr std::string_view TargetLabel(const RingTarget a_target) {
    switch (ToIndex(a_target)) {
        case 0:  return "leftThumb";
        case 1:  return "leftIndex";
        case 2:  return "leftMiddle";
        case 3:  return "leftRing";
        case 4:  return "leftPinky";
        case 5:  return "rightThumb";
        case 6:  return "rightIndex";
        case 7:  return "rightMiddle";
        case 8:  return "rightRing";
        case 9:  return "rightPinky";
        default: return "unknown";
    }
}

[[nodiscard]] constexpr char TargetSideCode(const RingTarget a_target) {
    return a_target.hand == RingHand::kLeft ? 'L' : 'R';
}

[[nodiscard]] constexpr std::uint8_t FingerFamily(const RingFinger a_finger) {
    return static_cast<std::uint8_t>(a_finger);
}

[[nodiscard]] std::string MakeFingerBoneName(RingTarget a_target, std::uint8_t a_segment);
