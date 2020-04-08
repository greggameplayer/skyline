// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright © 2020 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include "ILibraryAppletCreator.h"

namespace skyline::service::am {
    ILibraryAppletCreator::ILibraryAppletCreator(const DeviceState &state, ServiceManager &manager) : BaseService(state, manager, Service::am_ILibraryAppletCreator, "am:ILibraryAppletCreator", {
    }) {}
}
