-- Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
-- This file is subject to the license terms in the LICENSE file
-- found in the top-level of this distribution


-- imports
import("core.project.config")
import("core.project.project")
import("core.tool.compiler")
import("core.tool.linker")
import("core.base.json")
import("private.action.run.make_runenvs")

-- main entry
function main ()
    local output = {}

    -- load config
    config.load()
    -- add version info
    output.version = format("%s", xmake.version())
    -- add package infos

    -- add targets data
    local targets = {}
    local qml_import_path = {}
    for name, target in pairs((project:targets())) do
        local target_with_modules = (target:modulefiles() and #target:modulefiles() > 0) and true or false

        for _, dep in ipairs(target:orderdeps()) do
            local modulefiles = dep:get("modulefiles")
            if modulefiles and #modulefiles > 0 then
                target_with_modules = true
                break
            end
        end

        local header_files = target:headerfiles()
        table.sort(header_files)

        local module_files = target.modulefiles and target:modulefiles() or {}
        table.sort(module_files)

        local target_sourcebatches = {}
        for _, batch in pairs(target:sourcebatches()) do
            table.append(target_sourcebatches, batch)
        end

        table.sort(target_sourcebatches, function (first, second) return first.rulename < second.rulename end)

        local source_batches = {}
        local last_cxx_arguments = {}
        for name, batch in ipairs(target_sourcebatches) do
            if batch.rulename == "c++.build.modules.builder.headerunits" or batch.rulename == "qt.moc" or batch.rulename == "qt.qmltyperegistrar" then
                goto continue4
            end

            local source_files = batch.sourcefiles

            local arguments = {}
            for _, file in ipairs(source_files) do
                if batch.rulename == "c++.build" or batch.rulename == "c.build" then
                    local args = compiler.compflags(file, {target = target})

                    local ignore_next_arg = false

                    for i, argument in ipairs(args) do
                            if ignore_next_arg then
                                    ignore_next_arg = false
                                    goto continue2
                            end

                            if argument:startswith("/isystem") then
                                    table.insert(arguments, argument .. " ".. args[i + 1])
                                    ignore_next_arg = true
                            else
                                    table.insert(arguments, argument)
                            end

                            ::continue2::
                     end


                     if batch.sourcekind and (batch.sourcekind == "cxx" or batch.sourcekind == "cc") then
                            last_cxx_arguments = arguments
                     end
                end
            end


            table.join2(source_files, header_files, module_files)

            if batch.rulename == "c.build" then
                table.join2(arguments, target:get("cflags"))
            end

            if batch.rulename == "c++.build" then
                table.join2(arguments, target:get("cxxflags"))
            end

            arguments = table.unique(arguments)

            local kind = ""
            if batch.sourcekind then
                kind = batch.sourcekind
            else
                kind = "unknown"
            end

            table.append(source_batches, { kind = kind, source_files = source_files, arguments = arguments})

            ::continue4::
        end

        local target_file = target:targetfile() or ""

        local defined_in = path.absolute("xmake.lua", target:scriptdir())

        local use_qt = false
        local frameworks = target:get("frameworks")
        for _, framework in ipairs(frameworks) do
            if framework:startswith("Qt") then
                use_qt = true
            end


        end
        ::continue3::

        if use_qt then
            for _, p in ipairs(target:get("runenv")["QML2_IMPORT_PATH"]) do
                p = project:directory()
                table.append(qml_import_path, p)
            end
        end

        function _add_target_pkgenvs(target, pkgenvs, targets_added)
            if targets_added[target:name()] then
                return
            end

            targets_added[target:name()] = true

            table.join2(pkgenvs, target:pkgenvs())

            for _, dep in ipairs(target:orderdeps()) do
                _add_target_pkgenvs(dep, pkgenvs, targets_added)
            end
        end


        local pkgenvs = {}
        _add_target_pkgenvs(target, pkgenvs, {})

        local addrunenvs, setrunenvs = make_runenvs(target)
        local runenvs = { add = addrunenvs, set = setrunenvs }

        for name, env in pairs(pkgenvs) do
            runenvs.add[name] = runenvs.add[name] or {}
            table.append(runenvs.add[name], env)
        end

        table.insert(targets, { name = name,
                                kind = target:targetkind(),
                                run_envs = runenvs,
                                defined_in = defined_in,
                                source_batches = source_batches,
                                header_files = header_files,
                                module_files = module_files,
                                target_file = target_file,
                                packages = target:get("packages"),
                                frameworks = target:get("frameworks"),
                                use_qt = use_qt,
                                group = target:get("group"),
                                autogendir = target:autogendir() } )
    end

    table.sort(targets, function(first, second) return first.name > second.name end)

    output.targets = targets

    local options = {}
    for optionname, option in pairs(project.options()) do
        local value = option:value()
        local values = option:get("values")
        if value == true or value == false then
            values = {"true", "false"}
        end

        table.insert(options, {
          name = option:name(),
          value = format("%s", value),
          description = option:get("description"),
          values = values
        })
    end

    output.options = options

    output.project_dir = project:directory()
    output.qml_import_path = qml_import_path
    output.project_files = project:allfiles()


    print(json.encode(output))
end
