#include "acc_su.h"
#include "acc.h"

namespace skyline::kernel::service::acc {
    acc_SU::acc_SU(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, false, Service::acc_su, {
        {0x5, SFUNC(acc_SU::GetProfile)}
    }) {}

    void acc_SU::GetProfile(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        manager.RegisterService(SRVREG(IProfile), session, response);
    }
}