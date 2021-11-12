package("QtCreator")
    on_fetch(function (package, opt)
         opt.components = {}
         table.insert(opt.components, "Core")
         table.insert(opt.components, "ExtensionSystem")
         table.insert(opt.components, "Utils")
         return package:find_package("cmake::QtCreator", opt)
    end)
package_end()


add_requires("QtCreator")

target("xmakeprojectmanager")
    set_kind("shared")
    add_rules("qt.shared")
    add_rules("mode.debug", "mode.release")
    add_files("src/*.cpp")
    add_headerfiles("src/*.hpp")
    add_languages("cxx20")
    if "${os}" == "windows" then
        add_cxxflags("/Zc:__cplusplus")
    end
    add_frameworks("QtCore", "QtWidgets")
    add_packages("QtCreator")
