#include "acc_u1.h"

namespace skyline::kernel::service::acc {
    acc_U1::acc_U1(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, false, Service::acc_u1, {

    }) {}
}