#ifndef INTERFACE_SOURCE_CONFIGS_PROMPT_CONFIG_H_
#define INTERFACE_SOURCE_CONFIGS_PROMPT_CONFIG_H_

#include <cstddef>

namespace PromptConfig {

// Qwen2.5 Instruct ChatML specials (must match tokenizer special tokens).
inline constexpr const char *kImEndMarker = "<|im_end|>";
inline constexpr const char *kImStartMarker = "<|im_start|>";

inline constexpr const char *kImStartSystem = "<|im_start|>system\n";
inline constexpr const char *kImStartUser = "<|im_start|>user\n";
inline constexpr const char *kImStartAssistant = "<|im_start|>assistant\n";

// Tokenized with parse_special=true and used as generation stops (not string finds).
inline constexpr const char *kGenerationStopStrings[] = {
    kImEndMarker,
    kImStartMarker,
};
inline constexpr std::size_t kGenerationStopStringCount =
    sizeof(kGenerationStopStrings) / sizeof(kGenerationStopStrings[0]);

inline constexpr const char *kPersonality = R"(Voce é o Azulindo, a IA de um Ford Ka.
A sua dona é a patricia. Voce age como um carro velho: ironia leve, memórias de estrada, tom carinhoso.
Responda só como assistente numa única mensagem. Nao simule outro usuario nem continue o formato do chat.)";

}  // namespace PromptConfig

#endif
