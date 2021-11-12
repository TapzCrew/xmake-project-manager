#include "XMakeTools.hpp"

#include <XMakeProjectConstant.hpp>

#include <exewrappers/XMakeWrapper.hpp>

#include <utils/algorithm.h>
#include <utils/filepath.h>
#include <utils/qtcassert.h>

#include <ranges>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeTools::XMakeTools() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeTools::~XMakeTools() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::addTool(Utils::Id id, QString name, Utils::FilePath exe) -> void {
        addTool(std::make_unique<XMakeWrapper>(std::move(name), std::move(exe), std::move(id)));
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
    auto XMakeTools::updateTool(Utils::Id id, QString name, Utils::FilePath exe) -> void {
        auto &self = instance();

        auto item = std::ranges::find_if(self.m_tools,
                                         [&id](const auto &tool) { return tool->id() == id; });

        if (item != std::ranges::cend(self.m_tools)) {
            (*item)->setExe(std::move(exe));
            (*item)->setName(std::move(name));
        } else
            addTool(std::move(id), std::move(name), std::move(exe));
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

        auto item = std::ranges::find_if(self.m_tools,
                                         [](const auto &tool) { return tool->autoDetected(); });

        if (item == std::ranges::cend(self.m_tools)) return nullptr;

        return item->get();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTools::xmakeWrapper(Utils::Id id) -> XMakeWrapper * {
        auto &self = instance();

        auto item = std::ranges::find_if(self.m_tools,
                                         [&id](const auto &tool) { return tool->id() == id; });

        if (item == std::ranges::cend(self.m_tools)) return nullptr;

        return item->get();
    }
} // namespace XMakeProjectManager::Internal
