#include <kithelper/KitHelper.hpp>

namespace XMakeProjectManager::Internal::KitHelper {
    namespace details {
        ////////////////////////////////////////////////////
        ////////////////////////////////////////////////////
        auto expand(const ProjectExplorer::Kit *kit, const QString &macro) -> QString {
            return kit->macroExpander()->expand(macro);
        }
    } // namespace details

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto cCompilerPath(const ProjectExplorer::Kit *kit) -> QString {
        QTC_ASSERT(kit, return {});
        return details::expand(kit, "%{Compiler:Executable:C}");
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto cxxCompilerPath(const ProjectExplorer::Kit *kit) -> QString {
        QTC_ASSERT(kit, return {});
        return details::expand(kit, "%{Compiler:Executable:Cxx}");
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto kitData(const ProjectExplorer::Kit *kit) -> KitData {
        QTC_ASSERT(kit, return {});

        auto data = KitData { cCompilerPath(kit), cxxCompilerPath(kit) };

        return data;
    }
} // namespace XMakeProjectManager::Internal::KitHelper
