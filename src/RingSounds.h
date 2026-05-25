#pragma once

#include <cstdint>

namespace RingSounds {
enum class Event : std::uint8_t {
    kNone = 0,
    kEquip = 1,
    kUnequip = 2,
};

void Play(RE::Actor& a_actor, RE::TESObjectARMO& a_ring, Event a_event);
void Play(RE::Actor& a_actor, RE::FormID a_sourceFormID, Event a_event);
}
