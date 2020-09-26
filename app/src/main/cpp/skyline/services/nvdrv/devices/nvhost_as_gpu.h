// SPDX-License-Identifier: MPL-2.0
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include "nvdevice.h"

namespace skyline::service::nvdrv::device {
    /**
     * @brief NvHostAsGpu (/dev/nvhost-as-gpu) is used to access GPU virtual address spaces (https://switchbrew.org/wiki/NV_services#.2Fdev.2Fnvhost-as-gpu)
     */
    class NvHostAsGpu : public NvDevice {
      private:
        struct AddressSpaceRegion {
            u64 cpuAddress;
            u64 size;
            bool fixed;
        };

        std::map<u64, AddressSpaceRegion> regionMap; //!< This maps the base addresses of mapped buffers to their total sizes and mapping type, this is needed as what was originally a single buffer may have been split into multiple GPU side buffers with the remap flag.

      public:
        NvHostAsGpu(const DeviceState &state);

        /**
         * @brief This binds a channel to the address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_BIND_CHANNEL)
         */
        NvStatus BindChannel(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        /**
         * @brief This reserves a region in the GPU address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_ALLOC_SPACE)
         */
        NvStatus AllocSpace(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        /**
         * @brief This unmaps a region in the GPU address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_UNMAP_BUFFER)
         */
        NvStatus UnmapBuffer(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        /**
         * @brief This maps a region in the GPU address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_MODIFY)
         */
        NvStatus Modify(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        /**
         * @brief This returns the application's GPU address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_GET_VA_REGIONS)
         */
        NvStatus GetVaRegions(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        /**
         * @brief This initializes the application's GPU address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_ALLOC_AS_EX)
         */
        NvStatus AllocAsEx(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        /**
         * @brief Remaps a region of the GPU address space (https://switchbrew.org/wiki/NV_services#NVGPU_AS_IOCTL_REMAP)
         */
        NvStatus Remap(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer);

        NVDEVICE_DECL(
            NVFUNC(0x4101, NvHostAsGpu, BindChannel),
            NVFUNC(0x4102, NvHostAsGpu, AllocSpace),
            NVFUNC(0x4105, NvHostAsGpu, UnmapBuffer),
            NVFUNC(0x4106, NvHostAsGpu, Modify),
            NVFUNC(0x4108, NvHostAsGpu, GetVaRegions),
            NVFUNC(0x4109, NvHostAsGpu, AllocAsEx),
            NVFUNC(0x4114, NvHostAsGpu, Remap)
        )
    };
}
