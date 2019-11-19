#include "acc.h"

namespace skyline::kernel::service::acc {
    IProfile::IProfile(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, false, Service::acc_IProfile, {
        {0x0, SFUNC(IProfile::Get)}
    }) {}

    void IProfile::Get(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {

    }
}