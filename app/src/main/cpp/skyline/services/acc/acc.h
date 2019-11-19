#pragma once

#include <services/base_service.h>
#include <services/serviceman.h>

namespace skyline::kernel::service::acc {
    class IProfile : public BaseService {
      public:
        IProfile(const DeviceState &state, ServiceManager &manager);

        void Get(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);
    };
}
