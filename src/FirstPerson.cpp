#include "FirstPerson.h"

namespace FirstPerson {
void ApplyRaceFlags() {
    auto* dataHandler = RE::TESDataHandler::GetSingleton();
    if (!dataHandler) {
        logger::warn("FirstPerson: race flag patch skipped | reason=noDataHandler");
        return;
    }

    std::uint32_t patched = 0;
    for (auto* race : dataHandler->GetFormArray<RE::TESRace>()) {
        if (!race) {
            continue;
        }

        auto& firstPersonFlags = race->bipedModelData.bipedObjectSlots;
        if (!firstPersonFlags.all(RE::BGSBipedObjectForm::FirstPersonFlag::kRing)) {
            firstPersonFlags.set(RE::BGSBipedObjectForm::FirstPersonFlag::kRing);
            ++patched;
        }
    }

    logger::info("FirstPerson: vanilla ring race flags patched | races={}", patched);
}
}
