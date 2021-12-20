#include "XMakeTargetParser.hpp"

#include <utils/filepath.h>
#include <utils/fileutils.h>

namespace XMakeProjectManager::Internal {
    auto Target::fullName(const Utils::FilePath &src_dir, const Target &target) -> QString {
        if (Utils::FileUtils::isAbsolutePath(target.target_file)) {
            const auto fname = target.target_file.split('/').last();

            auto definedIn =
                Utils::FilePath::fromString(target.defined_in).absolutePath().toString();

            return definedIn.remove(src_dir.toString()) + '/' + fname;
        } else {
            return target.target_file;
        }
    }
} // namespace XMakeProjectManager::Internal
