#include "apm.h"

namespace skyline::kernel::service::apm {
    BaseApm::BaseApm(const DeviceState &state, ServiceManager &manager, Service serviceType, const std::unordered_map<u32, std::function<void(type::KSession &, ipc::IpcRequest &, ipc::IpcResponse &)>> &vTable) : BaseService(state, manager, false, serviceType, vTable) {}

    void BaseApm::OpenSession(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        manager.RegisterService(std::make_shared<ISession>(state, manager), session, response);
    }

    apm::apm(const DeviceState &state, ServiceManager &manager) : BaseApm(state, manager, Service::apm, {
        {0x0, SFUNC(BaseApm::OpenSession)}
    }) {}


    apmP::apmP(const DeviceState &state, ServiceManager &manager) : BaseApm(state, manager, Service::apm_p, {
        {0x0, SFUNC(BaseApm::OpenSession)}
    }) {}


    ISession::ISession(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, false, Service::apm_ISession, {
        {0x0, SFUNC(ISession::SetPerformanceConfiguration)},
        {0x1, SFUNC(ISession::GetPerformanceConfiguration)}
    }) {}

    void ISession::SetPerformanceConfiguration(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        struct InputStruct {
            u32 mode;
            u32 config;
        } *performance = reinterpret_cast<InputStruct *>(request.cmdArg);
        performanceConfig[performance->mode] = performance->config;
        state.logger->Info("SetPerformanceConfiguration called with 0x{:X} ({})", performance->config, performance->mode ? "Docked" : "Handheld");
    }

    void ISession::GetPerformanceConfiguration(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        u32 performanceMode = *reinterpret_cast<u32 *>(request.cmdArg);
        response.WriteValue<u32>(performanceConfig[performanceMode]);
    }
}
