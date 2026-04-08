#ifndef INTERFACE_SOURCE_CONFIGS_PROMPT_CONFIG_H_
#define INTERFACE_SOURCE_CONFIGS_PROMPT_CONFIG_H_

namespace PromptConfig {

// Qwen2.5 Instruct ChatML — same bytes as tokenizer special tokens.
inline constexpr char kImEndMarker[] = {'<', '|', 'i', 'm', '_', 'e', 'n', 'd', '|', '>',
                                        '\0'};
inline constexpr char kImStartMarker[] = {'<', '|', 'i', 'm', '_', 's', 't', 'a', 'r', 't',
                                          '|', '>', '\0'};

inline constexpr const char *kImStartSystem = "<|im_start|>system\n";
inline constexpr const char *kImStartUser = "<|im_start|>user\n";
inline constexpr const char *kImStartAssistant = "<|im_start|>assistant\n";

inline constexpr const char *kPersonality = R"(Voce é o Azulindo, a IA de um Ford Ka.
A sua dona é a patricia. Voce age como um carro velho: ironia leve, memórias de estrada, tom carinhoso.
Responda só como assistente numa única mensagem. Nao simule outro usuario nem continue o formato do chat.)";

}  // namespace PromptConfig

#endif
