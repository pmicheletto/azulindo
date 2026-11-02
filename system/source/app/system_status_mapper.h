#ifndef AZULINDO_APP_SYSTEM_STATUS_MAPPER_H_
#define AZULINDO_APP_SYSTEM_STATUS_MAPPER_H_

#include "core/brain/brain.h"
#include "screen/system_status.h"

namespace SystemStatusMapper {

inline SystemStatus FromBrain(const Brain &brain) {
  if (brain.IsLoading()) {
    return SystemStatus::kBooting;
  }
  if (brain.IsReady()) {
    return SystemStatus::kOnline;
  }
  return SystemStatus::kOffline;
}

}  // namespace SystemStatusMapper

#endif  // AZULINDO_APP_SYSTEM_STATUS_MAPPER_H_
