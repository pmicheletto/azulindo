#ifndef AZULINDO_APP_THINK_BLOCK_FILTER_H_
#define AZULINDO_APP_THINK_BLOCK_FILTER_H_

#include <algorithm>
#include <string>
#include <string_view>

#include "configs/prompt_config.h"

// Streams text and drops Qwen-style <think>...</think> blocks (and trailing
// newlines after the close tag). Holds back partial tag prefixes across chunks.
class ThinkBlockFilter {
 public:
  void Reset() {
    pending_.clear();
    in_think_ = false;
  }

  std::string Feed(std::string_view chunk) {
    pending_.append(chunk.data(), chunk.size());
    return Drain(/*flush=*/false);
  }

  // Emit any held-back non-think text (end of generation).
  std::string Flush() { return Drain(/*flush=*/true); }

 private:
  static std::size_t PartialSuffixLen(const std::string &s,
                                      std::string_view tag) {
    if (tag.empty()) {
      return 0;
    }
    const std::size_t max_n = std::min(s.size(), tag.size() - 1);
    for (std::size_t n = max_n; n > 0; --n) {
      if (s.compare(s.size() - n, n, tag.data(), n) == 0) {
        return n;
      }
    }
    return 0;
  }

  std::string Drain(bool flush) {
    std::string out;
    out.reserve(pending_.size());

    while (!pending_.empty()) {
      if (in_think_) {
        const std::size_t close =
            pending_.find(PromptConfig::kThinkClose);
        if (close == std::string::npos) {
          if (flush) {
            pending_.clear();
          } else {
            const std::size_t keep =
                PartialSuffixLen(pending_, PromptConfig::kThinkClose);
            if (pending_.size() > keep) {
              pending_.erase(0, pending_.size() - keep);
            }
          }
          break;
        }
        pending_.erase(0, close + std::char_traits<char>::length(
                                      PromptConfig::kThinkClose));
        while (!pending_.empty() &&
               (pending_.front() == '\n' || pending_.front() == '\r')) {
          pending_.erase(pending_.begin());
        }
        in_think_ = false;
        continue;
      }

      const std::size_t open = pending_.find(PromptConfig::kThinkOpen);
      if (open == std::string::npos) {
        if (flush) {
          out += pending_;
          pending_.clear();
        } else {
          const std::size_t keep =
              PartialSuffixLen(pending_, PromptConfig::kThinkOpen);
          out.append(pending_, 0, pending_.size() - keep);
          pending_.erase(0, pending_.size() - keep);
        }
        break;
      }
      out.append(pending_, 0, open);
      pending_.erase(0, open + std::char_traits<char>::length(
                                  PromptConfig::kThinkOpen));
      in_think_ = true;
    }
    return out;
  }

  std::string pending_;
  bool in_think_ = false;
};

#endif  // AZULINDO_APP_THINK_BLOCK_FILTER_H_
