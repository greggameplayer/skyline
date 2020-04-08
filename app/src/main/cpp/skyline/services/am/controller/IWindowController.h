// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <services/base_service.h>
#include <services/serviceman.h>

namespace skyline::service::am {
    /**
     * @brief This has functions used to retrieve the status of the application's window (https://switchbrew.org/wiki/Applet_Manager_services#IWindowController)
     */
    class IWindowController : public BaseService {
      public:
        IWindowController(const DeviceState &state, ServiceManager &manager);

        /**
         * @brief This returns the PID of the current application (https://switchbrew.org/wiki/Applet_Manager_services#GetAppletResourceUserId)
         */
        void GetAppletResourceUserId(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief This function has mo inputs or outputs (Stubbed) (https://switchbrew.org/wiki/Applet_Manager_services#AcquireForegroundRights)
         */
        void AcquireForegroundRights(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);
    };
}
