// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeTools.hpp"

#include <XMakeProjectConstant.hpp>

#include <exewrappers/XMakeWrapper.hpp>

#include <utils/algorithm.h>
#include <utils/filepath.h>
#include <utils/qtcassert.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeTools::XMakeTools() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeTools::~XMakeTools() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::addTool(Utils::Id id,
                             QString name,
                             Utils::FilePath exe,
                             bool autorun,
                             bool auto_accept_requests) -> void {
        addTool(std::make_unique<XMakeWrapper>(std::move(name),
                                               std::move(exe),
                                               std::move(id),
                                               autorun,
                                               auto_accept_requests));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::addTool(XMakeWrapperPtr &&xmake) -> void {
        auto &self = instance();
        self.m_tools.emplace_back(std::move(xmake));

        Q_EMIT self.toolAdded(*self.m_tools.back());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::setTools(std::vector<XMakeWrapperPtr> &&tools) -> void {
        auto &self = instance();

        self.m_tools = std::move(tools);

        auto wrapper = xmakeWrapper();
        if (wrapper) return;

        auto path = XMakeWrapper::findTool();
        if (path)
            self.m_tools.emplace_back(std::make_unique<XMakeWrapper>(QStringLiteral("System XMake"),
                                                                     std::move(path.value()),
                                                                     true));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::updateTool(Utils::Id id,
                                QString name,
                                Utils::FilePath exe,
                                bool autorun,
                                bool auto_accept_requests) -> void {
        auto &self = instance();

        auto item = std::find_if(std::begin(self.m_tools),
                                 std::end(self.m_tools),
                                 [&id](const auto &tool) { return tool->id() == id; });

        if (item != std::cend(self.m_tools)) {
            (*item)->setExe(std::move(exe));
            (*item)->setName(std::move(name));
            (*item)->setAutorun(autorun);
            (*item)->setAutoAcceptRequests(auto_accept_requests);
        } else
            addTool(std::move(id), std::move(name), std::move(exe), autorun, auto_accept_requests);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::removeTool(Utils::Id id) -> void {
        auto &self = instance();

        auto item = Utils::take(self.m_tools, [&id](const auto &tool) { return tool->id() == id; });

        QTC_ASSERT(item, return );

        Q_EMIT self.toolRemoved(*item->get());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::xmakeWrapper() -> XMakeWrapper * {
        auto &self = instance();

        auto item = std::find_if(std::begin(self.m_tools),
                                 std::end(self.m_tools),
                                 [](const auto &tool) { return tool->autoDetected(); });

        if (item == std::cend(self.m_tools)) return nullptr;

        return item->get();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::xmakeWrapper(Utils::Id id) -> XMakeWrapper * {
        auto &self = instance();

        auto item = std::find_if(std::begin(self.m_tools),
                                 std::end(self.m_tools),
                                 [&id](const auto &tool) { return tool->id() == id; });

        if (item == std::cend(self.m_tools)) return nullptr;

        return item->get();
    }
} // namespace XMakeProjectManager::Internal
