// SPDX-License-Identifier: MPL-2.0
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include <kernel/types/KProcess.h>
#include <vfs/filesystem.h>
#include "results.h"
#include "IFile.h"
#include "IFileSystem.h"

namespace skyline::service::fssrv {
    IFileSystem::IFileSystem(std::shared_ptr<vfs::FileSystem> backing, const DeviceState &state, ServiceManager &manager) : backing(backing), BaseService(state, manager, {
        {0x0, SFUNC(IFileSystem::CreateFile)},
        {0x7, SFUNC(IFileSystem::GetEntryType)},
        {0x8, SFUNC(IFileSystem::OpenFile)},
        {0xA, SFUNC(IFileSystem::Commit)}
    }) {}

    Result IFileSystem::CreateFile(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        std::string path = std::string(state.process->GetPointer<char>(request.inputBuf.at(0).address));
        auto mode = request.Pop<u64>();
        auto size = request.Pop<u32>();

        return backing->CreateFile(path, size) ? Result{} : result::PathDoesNotExist;
    }

    Result IFileSystem::GetEntryType(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        std::string path = std::string(state.process->GetPointer<char>(request.inputBuf.at(0).address));

        auto type = backing->GetEntryType(path);

        if (type) {
            response.Push(*type);
            return {};
        } else {
            response.Push<u32>(0);
            return result::PathDoesNotExist;
        }
    }

    Result IFileSystem::OpenFile(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        std::string path(state.process->GetPointer<char>(request.inputBuf.at(0).address));
        auto mode = request.Pop<vfs::Backing::Mode>();

        if (!backing->FileExists(path))
            return result::PathDoesNotExist;

        auto file = backing->OpenFile(path, mode);
        if (file == nullptr)
            return result::UnexpectedFailure;
        else
            manager.RegisterService(std::make_shared<IFile>(file, state, manager), session, response);

        return {};
    }

    Result IFileSystem::Commit(type::KSession &session, ipc::IpcRequest &request, ipc::IpcResponse &response) {
        return {};
    }
}
