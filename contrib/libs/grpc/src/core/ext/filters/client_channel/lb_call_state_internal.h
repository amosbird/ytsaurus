//
// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef GRPC_SRC_CORE_EXT_FILTERS_CLIENT_CHANNEL_LB_CALL_STATE_INTERNAL_H
#define GRPC_SRC_CORE_EXT_FILTERS_CLIENT_CHANNEL_LB_CALL_STATE_INTERNAL_H

#include <grpc/support/port_platform.h>

#include "y_absl/strings/string_view.h"

#include "src/core/lib/gprpp/unique_type_name.h"
#include "src/core/lib/load_balancing/lb_policy.h"

namespace grpc_core {

//
// LbCallStateInternal
//
class LbCallStateInternal : public LoadBalancingPolicy::CallState {
 public:
  virtual y_absl::string_view GetCallAttribute(UniqueTypeName type) = 0;
};

}  // namespace grpc_core

#endif  // GRPC_SRC_CORE_EXT_FILTERS_CLIENT_CHANNEL_LB_CALL_STATE_INTERNAL_H
