#include "RingTargets.h"

#include <format>

std::string MakeFingerBoneName(const RingTarget a_target, const std::uint8_t a_segment) {
    const auto side = TargetSideCode(a_target);
    const auto family = FingerFamily(a_target.finger);
    return std::format("NPC {} Finger{}{} [{}F{}{}]", side, family, a_segment, side, family, a_segment);
}
