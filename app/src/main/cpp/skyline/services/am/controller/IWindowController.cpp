// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include <kernel/types/KProcess.h>
#include "IWindowController.h"

namespace skyline::service::am {
    IWindowController::IWindowController(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, Service::am_IWindowController, "am:IWindowController", {
        {0x1, SFUNC(IWindowController::GetAppletResourceUserId)},
        {0xA, SFUNC(IWindowController::AcquireForegroundRights)}
    }) {}

    void IWindowController::GetAppletResourceUserId(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        response.Push(static_cast<u64>(state.process->pid));
    }

    void IWindowController::AcquireForegroundRights(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {}
}
