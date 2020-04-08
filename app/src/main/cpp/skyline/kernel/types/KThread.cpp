// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include <sys/resource.h>
#include <nce.h>
#include "KThread.h"
#include "KProcess.h"

namespace skyline::kernel::type {
    KThread::KThread(const DeviceState &state, KHandle handle, pid_t selfPid, u64 entryPoint, u64 entryArg, u64 stackTop, u64 tls, u8 priority, KProcess *parent, std::shared_ptr<type::KSharedMemory> &tlsMemory) : handle(handle), pid(selfPid), entryPoint(entryPoint), entryArg(entryArg), stackTop(stackTop), tls(tls), priority(priority), parent(parent), ctxMemory(tlsMemory), KSyncObject(state, KType::KThread) {
        UpdatePriority(priority);
    }

    KThread::~KThread() {
        Kill();
    }

    void KThread::Start() {
        if (status == Status::Created) {
            if (pid == parent->pid)
                parent->status = KProcess::Status::Started;
            status = Status::Running;

            state.nce->StartThread(entryArg, handle, parent->threads.at(pid));
        }
    }

    void KThread::Kill() {
        if (status != Status::Dead) {
            status = Status::Dead;
            Signal();
        }
    }

    void KThread::UpdatePriority(u8 priority) {
        this->priority = priority;
        auto linuxPriority = static_cast<int8_t>(constant::AndroidPriority.first + ((static_cast<float>(constant::AndroidPriority.second - constant::AndroidPriority.first) / static_cast<float>(constant::SwitchPriority.second - constant::SwitchPriority.first)) * (static_cast<float>(priority) - constant::SwitchPriority.first))); // Resize range SwitchPriority (Nintendo Priority) to AndroidPriority (Android Priority)

        if (setpriority(PRIO_PROCESS, static_cast<id_t>(pid), linuxPriority) == -1)
            throw exception("Couldn't set process priority to {} for PID: {}", linuxPriority, pid);
    }
}
