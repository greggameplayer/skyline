#pragma once

#include <services/base_service.h>
#include <services/serviceman.h>

namespace skyline::kernel::service::acc {
    class acc_U0 : public BaseService {
      public:
        acc_U0(const DeviceState &state, ServiceManager &manager);
    };
}
