// SPDX-License-Identifier: MPL-2.0
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <services/base_service.h>
#include <services/serviceman.h>

namespace skyline::service::nvdrv {
    class Driver;

    /**
     * @brief nvdrv or INvDrvServices is used to access the Nvidia GPU inside the Switch (https://switchbrew.org/wiki/NV_services#nvdrv.2C_nvdrv:a.2C_nvdrv:s.2C_nvdrv:t)
     */
    class INvDrvServices : public BaseService {
      private:
        std::shared_ptr<Driver> driver;

      public:
        INvDrvServices(const DeviceState &state, ServiceManager &manager);

        /**
         * @brief Open a specific device and return a FD (https://switchbrew.org/wiki/NV_services#Open)
         */
        Result Open(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief Perform an IOCTL on the specified FD (https://switchbrew.org/wiki/NV_services#Ioctl)
         */
        Result Ioctl(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief Close the specified FD (https://switchbrew.org/wiki/NV_services#Close)
         */
        Result Close(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief This initializes the driver (https://switchbrew.org/wiki/NV_services#Initialize)
         */
        Result Initialize(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief This returns a specific event from a device (https://switchbrew.org/wiki/NV_services#QueryEvent)
         */
        Result QueryEvent(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief This sets the AppletResourceUserId which matches the PID (https://switchbrew.org/wiki/NV_services#SetAruid)
         */
        Result SetAruid(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief Perform an IOCTL on the specified FD with an extra input buffer (https://switchbrew.org/wiki/NV_services#Ioctl2)
         */
        Result Ioctl2(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief Perform an IOCTL on the specified FD with an extra output buffer (https://switchbrew.org/wiki/NV_services#Ioctl3)
         */
        Result Ioctl3(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        /**
         * @brief This enables the graphics firmware memory margin (https://switchbrew.org/wiki/NV_services#SetGraphicsFirmwareMemoryMarginEnabled)
         */
        Result SetGraphicsFirmwareMemoryMarginEnabled(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response);

        SERVICE_DECL(
            SFUNC(0x0, INvDrvServices, Open),
            SFUNC(0x1, INvDrvServices, Ioctl),
            SFUNC(0x2, INvDrvServices, Close),
            SFUNC(0x3, INvDrvServices, Initialize),
            SFUNC(0x4, INvDrvServices, QueryEvent),
            SFUNC(0x8, INvDrvServices, SetAruid),
            SFUNC(0xB, INvDrvServices, Ioctl2),
            SFUNC(0xC, INvDrvServices, Ioctl3),
            SFUNC(0xD, INvDrvServices, SetGraphicsFirmwareMemoryMarginEnabled)
        )
    };
}
