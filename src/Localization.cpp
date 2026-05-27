#include "Localization.h"

#include <RE/B/BSResourceNiBinaryStream.h>
#include <RE/I/INISettingCollection.h>

#include <iterator>
#include <unordered_map>

namespace {
std::unordered_map<std::string, std::string> translationMap;

[[nodiscard]] std::string GetLanguage() {
    auto* ini = RE::INISettingCollection::GetSingleton();
    const auto* setting = ini ? ini->GetSetting("sLanguage:General") : nullptr;
    if (setting && setting->GetType() == RE::Setting::Type::kString && setting->data.s) {
        std::string language {setting->data.s};
        std::ranges::transform(language, language.begin(), [](const unsigned char a_ch) {
            return static_cast<char>(std::toupper(a_ch));
        });
        return language;
    }

    return "ENGLISH";
}

[[nodiscard]] bool IsEnglish(const std::string& a_language) {
    return a_language == "ENGLISH";
}

[[nodiscard]] std::string ToUtf8(const std::wstring_view a_text) {
    return stl::utf16_to_utf8(a_text).value_or(""s);
}

template <std::size_t Size>
[[nodiscard]] bool ReadLine(const RE::BSResource::Stream& a_stream, wchar_t (&a_dst)[Size], std::uint32_t& a_len) {
    auto* iter = a_dst;
    auto readAny = false;
    for (std::size_t i = 0; i + 1 < std::size(a_dst); ++i) {
        wchar_t data = 0;
        std::uint64_t read = 0;
        a_stream.DoRead(std::addressof(data), sizeof(data), read);
        if (read != sizeof(data)) {
            break;
        }

        readAny = true;
        if (data == L'\n') {
            break;
        }

        *iter++ = data;
    }

    *iter = 0;
    a_len = static_cast<std::uint32_t>(iter - a_dst);
    return readAny;
}

void LoadFile(const std::string_view a_name, const std::string_view a_language) {
    const auto path = std::format("Interface\\Translations\\{}_{}.txt", a_name, a_language);
    RE::BSResourceNiBinaryStream fileStream {path};
    if (!fileStream.good()) {
        return;
    }

    std::uint16_t bom = 0;
    std::uint64_t read = 0;
    fileStream.stream->DoRead(std::addressof(bom), sizeof(bom), read);
    if (read != sizeof(bom) || bom != 0xFEFF) {
        logger::warn("Failed to load translations. File must be UTF-16 LE: {}", path);
        return;
    }

    std::size_t count = 0;
    while (true) {
        wchar_t line[512];
        std::uint32_t len = 0;
        if (!ReadLine(*fileStream.stream, line, len)) {
            logger::info("Loaded {} translations from {}", count, path);
            return;
        }

        if (len == 0) {
            continue;
        }

        if (line[len - 1] == L'\r') {
            --len;
        }

        if (len == 0) {
            continue;
        }

        if (len < 4 || line[0] != L'$') {
            continue;
        }

        std::uint32_t delimiter = 0;
        for (std::uint32_t i = 0; i < len; ++i) {
            if (line[i] == L'\t') {
                delimiter = i;
            }
        }

        if (delimiter < 2 || delimiter + 1 >= len) {
            continue;
        }

        const auto key = ToUtf8(std::wstring_view {line, delimiter});
        const auto value = ToUtf8(std::wstring_view {std::addressof(line[delimiter + 1]), len - delimiter - 1});
        translationMap.insert_or_assign(key, value);
        ++count;
    }
}
}

namespace Localization {
void Load(const std::string_view a_name) {
    translationMap.clear();

    const auto language = GetLanguage();
    LoadFile(a_name, "ENGLISH");
    if (!IsEnglish(language)) {
        LoadFile(a_name, language);
    }
}

std::string Translate(const std::string_view a_key, const std::string_view a_fallback) {
    if (const auto it = translationMap.find(std::string {a_key}); it != translationMap.end()) {
        return it->second;
    }

    return std::string {a_fallback};
}
}
