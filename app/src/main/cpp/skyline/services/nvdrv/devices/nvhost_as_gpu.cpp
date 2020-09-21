// SPDX-License-Identifier: MPL-2.0
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include <gpu.h>
#include <os.h>
#include <kernel/types/KProcess.h>
#include <services/nvdrv/driver.h>
#include "nvmap.h"
#include "nvhost_as_gpu.h"

namespace skyline::service::nvdrv::device {
    struct MappingFlags {
        bool fixed : 1;
        u8 _pad0_ : 7;
        bool remap : 1;
        u32 _pad1_ : 23;
    };
    static_assert(sizeof(MappingFlags) == sizeof(u32));

    NvHostAsGpu::NvHostAsGpu(const DeviceState &state) : NvDevice(state) {}

    NvStatus NvHostAsGpu::BindChannel(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        return NvStatus::Success;
    }

    NvStatus NvHostAsGpu::AllocSpace(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        struct Data {
            u32 pages;          // In
            u32 pageSize;       // In
            MappingFlags flags; // In
            u32 _pad_;
            union {
                u64 offset;     // InOut
                u64 align;      // In
            };
        } region = util::As<Data>(buffer);

        u64 size = static_cast<u64>(region.pages) * static_cast<u64>(region.pageSize);

        if (region.flags.fixed)
            region.offset = state.gpu->memoryManager.ReserveFixed(region.offset, size);
        else
            region.offset = state.gpu->memoryManager.ReserveSpace(size, region.align);

        if (region.offset == 0) {
            state.logger->Warn("Failed to allocate GPU address space region!");
            return NvStatus::BadParameter;
        }

        return NvStatus::Success;
    }

    NvStatus NvHostAsGpu::UnmapBuffer(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        u64 offset{util::As<u64>(buffer)};

        try {
            auto region = regionMap.at(offset);

            // Non-fixed regions are unmapped so that they can be used by future non-fixed mappings
            if (!region.fixed)
                if (!state.gpu->memoryManager.Unmap(offset, region.size))
                    state.logger->Warn("Failed to unmap region at 0x{:X}", offset);

            regionMap.erase(offset);
        } catch (const std::out_of_range &e) {
            state.logger->Warn("Couldn't find region to unmap at 0x{:X}", offset);
        }

        return NvStatus::Success;
    }

    NvStatus NvHostAsGpu::Modify(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        struct Data {
            MappingFlags flags; // In
            u32 kind;           // In
            u32 nvmapHandle;    // In
            u32 pageSize;       // InOut
            u64 bufferOffset;   // In
            u64 mappingSize;    // In
            u64 offset;         // InOut
        }  &data = util::As<Data>(buffer);

        try {
            auto driver = nvdrv::driver.lock();
            auto nvmap = driver->nvMap.lock();
            auto mapping = nvmap->handleTable.at(data.nvmapHandle);

            if (data.flags.remap) {
                auto region = regionMap.upper_bound(data.offset);
                if ((region == regionMap.begin()) || (region == regionMap.end())) {
                    state.logger->Warn("Cannot remap an unmapped GPU address space region: 0x{:X}", data.offset);
                    return NvStatus::BadParameter;
                }

                // Upper bound gives us the region after the one we want
                region--;

                if (region->second.size < data.mappingSize) {
                    state.logger->Warn("Cannot remap an partially mapped GPU address space region: 0x{:X}", data.offset);
                    return NvStatus::BadParameter;
                }

                u64 gpuAddress = data.offset + data.bufferOffset;
                u64 cpuAddress = region->second.cpuAddress + data.bufferOffset;

                if (state.gpu->memoryManager.MapFixed(gpuAddress, cpuAddress, data.mappingSize)) {
                    state.logger->Warn("Failed to remap GPU address space region: 0x{:X}", gpuAddress);
                    return NvStatus::BadParameter;
                }

                return NvStatus::Success;
            }

            u64 mapPhysicalAddress = data.bufferOffset + mapping->address;
            u64 mapSize = data.mappingSize ? data.mappingSize : mapping->size;

            if (data.flags.fixed)
                data.offset = state.gpu->memoryManager.MapFixed(data.offset, mapPhysicalAddress, mapSize);
            else
                data.offset = state.gpu->memoryManager.MapAllocate(mapPhysicalAddress, mapSize);

            if (data.offset == 0) {
                state.logger->Warn("Failed to map GPU address space region!");
                return NvStatus::BadParameter;
            }

            regionMap[data.offset] = {mapPhysicalAddress, mapSize, data.flags.fixed};

            return NvStatus::Success;
        } catch (const std::out_of_range &) {
            state.logger->Warn("Invalid NvMap handle: 0x{:X}", data.nvmapHandle);
            return NvStatus::BadParameter;
        }
    }

    NvStatus NvHostAsGpu::GetVaRegions(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        /*
        struct Data {
            u64 _pad0_;
            u32 bufferSize; // InOut
            u32 _pad1_;

            struct {
                u64 offset;
                u32 page_size;
                u32 pad;
                u64 pages;
            } regions[2];   // Out
        } &regionInfo = util::As<Data>(buffer);
        */
        return NvStatus::Success;
    }

    NvStatus NvHostAsGpu::AllocAsEx(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        /*
        struct Data {
            u32 bigPageSize;  // In
            i32 asFd;         // In
            u32 flags;        // In
            u32 reserved;     // In
            u64 vaRangeStart; // In
            u64 vaRangeEnd;   // In
            u64 vaRangeSplit; // In
        } addressSpace = util::As<Data>(buffer);
        */
        return NvStatus::Success;
    }

    NvStatus NvHostAsGpu::Remap(IoctlType type, std::span<u8> buffer, std::span<u8> inlineBuffer) {
        struct Entry {
            u16 flags;       // In
            u16 kind;        // In
            u32 nvmapHandle; // In
            u32 mapOffset;   // In
            u32 gpuOffset;   // In
            u32 pages;       // In
        };

        constexpr u32 MinAlignmentShift{0x10}; // This shift is applied to all addresses passed to Remap

        auto entries{util::AsSpan<Entry>(buffer)};
        for (auto entry : entries) {
            try {
                auto driver = nvdrv::driver.lock();
                auto nvmap = driver->nvMap.lock();
                auto mapping = nvmap->handleTable.at(entry.nvmapHandle);

                u64 mapAddress = static_cast<u64>(entry.gpuOffset) << MinAlignmentShift;
                u64 mapPhysicalAddress = mapping->address + (static_cast<u64>(entry.mapOffset) << MinAlignmentShift);
                u64 mapSize = static_cast<u64>(entry.pages) << MinAlignmentShift;

                state.gpu->memoryManager.MapFixed(mapAddress, mapPhysicalAddress, mapSize);
            } catch (const std::out_of_range &) {
                state.logger->Warn("Invalid NvMap handle: 0x{:X}", entry.nvmapHandle);
                return NvStatus::BadParameter;
            }
        }

        return NvStatus::Success;
    }
}
