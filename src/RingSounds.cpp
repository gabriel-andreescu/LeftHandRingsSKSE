#include "RingSounds.h"

#include <memory>

namespace RingSounds {
void Play(RE::Actor& a_actor, RE::TESObjectARMO& a_ring, const Event a_event) {
    if (a_event == Event::kNone) {
        return;
    }

    const auto playPickupSound = a_event == Event::kEquip;
    a_actor.PlayPickUpSound(std::addressof(a_ring), playPickupSound, false);
}

void Play(RE::Actor& a_actor, const RE::FormID a_sourceFormID, const Event a_event) {
    if (a_event == Event::kNone || a_sourceFormID == 0) {
        return;
    }

    auto* sourceRing = RE::TESForm::LookupByID<RE::TESObjectARMO>(a_sourceFormID);
    if (!sourceRing) {
        return;
    }

    Play(a_actor, *sourceRing, a_event);
}
}
