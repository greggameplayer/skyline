// SPDX-License-Identifier: MPL-2.0
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include <nce.h>
#include <os.h>
#include <kernel/memory.h>
#include "loader.h"

namespace skyline::loader {
    Loader::ExecutableLoadInfo Loader::LoadExecutable(const std::shared_ptr<kernel::type::KProcess> process, const DeviceState &state, Executable &executable, size_t offset) {
        u64 patchRegionSize = PAGE_SIZE * 0x10;
        u64 base = constant::BaseAddress + offset - patchRegionSize;

        u64 executableBase = base + patchRegionSize;

        u64 textSize = executable.text.contents.size();
        u64 roSize = executable.ro.contents.size();
        u64 dataSize = executable.data.contents.size() + executable.bssSize;

        if (!util::PageAligned(textSize) || !util::PageAligned(roSize) || !util::PageAligned(dataSize))
            throw exception("LoadProcessData: Sections are not aligned with page size: 0x{:X}, 0x{:X}, 0x{:X}", textSize, roSize, dataSize);

        if (!util::PageAligned(executable.text.offset) || !util::PageAligned(executable.ro.offset) || !util::PageAligned(executable.data.offset))
            throw exception("LoadProcessData: Section offsets are not aligned with page size: 0x{:X}, 0x{:X}, 0x{:X}", executable.text.offset, executable.ro.offset, executable.data.offset);

        // The data section will always be the last section in memory, so put the patch section after it
        std::vector<u32> patch = state.nce->PatchCode(executable.text.contents, executableBase, -patchRegionSize);
        u64 patchSize = patch.size() * sizeof(u32);
        
        process->NewHandle<kernel::type::KPrivateMemory>(base, patchRegionSize, memory::Permission{true, true, true}, memory::states::CodeMutable); // RWX
        state.logger->Debug("Successfully mapped section .patch @ 0x{0:X}, Size = 0x{1:X}", base, patchRegionSize);

        process->NewHandle<kernel::type::KPrivateMemory>(executableBase + executable.text.offset, textSize, memory::Permission{true, false, true}, memory::states::CodeStatic); // R-X
        state.logger->Debug("Successfully mapped section .text @ 0x{0:X}, Size = 0x{1:X}", executableBase + executable.text.offset, textSize);

        process->NewHandle<kernel::type::KPrivateMemory>(executableBase + executable.ro.offset, roSize, memory::Permission{true, false, false}, memory::states::CodeReadOnly); // R--
        state.logger->Debug("Successfully mapped section .rodata @ 0x{0:X}, Size = 0x{1:X}", executableBase + executable.ro.offset, roSize);

        process->NewHandle<kernel::type::KPrivateMemory>(executableBase + executable.data.offset, dataSize, memory::Permission{true, true, false}, memory::states::CodeMutable); // RW-
        state.logger->Debug("Successfully mapped section .data @ 0x{0:X}, Size = 0x{1:X}", executableBase + executable.data.offset, dataSize);

        process->WriteMemory(patch.data(), base, patchSize);
        process->WriteMemory(executable.text.contents.data(), executableBase + executable.text.offset, textSize);
        process->WriteMemory(executable.ro.contents.data(), executableBase + executable.ro.offset, roSize);
        process->WriteMemory(executable.data.contents.data(), executableBase + executable.data.offset, dataSize - executable.bssSize);

        return {base ,  patchRegionSize + executable.data.offset + dataSize};
    }
}
