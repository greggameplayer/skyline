#include "acc_u0.h"

namespace skyline::kernel::service::acc {
    acc_U0::acc_U0(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, false, Service::acc_u0, {

    }) {}
}