#pragma once

#include <services/base_service.h>
#include <services/serviceman.h>

namespace skyline::kernel::service::acc {
    class acc_SU : public BaseService {
      public:
        acc_SU(const DeviceState &state, ServiceManager &manager);

        void GetProfile(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);
    };
}
