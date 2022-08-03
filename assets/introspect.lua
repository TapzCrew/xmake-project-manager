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

function parse_cxflags(argument, arguments)
    if argument:startswith("/isystem") or
       argument:startswith("/ifcSearchDir") or
       argument:startswith("/stdIfcDir") then
        return  argument .. " ".. arguments[i + 1], true
    end

    return argument, false
end

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
        local header_files = target:headerfiles()
        table.sort(header_files)

        local cxx_source_batch = target:sourcebatches()["c++.build"]
        local cxx_module_batch = target:sourcebatches()["c++.build.modules"]
        local cxx_source_files
        local cxx_arguments
        if cxx_source_batch then
            for _, sourcefile in ipairs(cxx_source_batch.sourcefiles) do
                local arguments = compiler.compflags(sourcefile, {target = target})

                local ignore_next_arg = false
                for i, argument in ipairs(arguments) do
                        if ignore_next_arg then
                                ignore_next_arg = false
                                goto continue2
                        end

                        cxx_arguments = cxx_arguments or {}
                        cxx_argument, ignore_next_arg = parse_cxflags(argument, arguments)
                        table.append(cxx_arguments, cxx_argument)

                        ::continue2::
                 end
            end

            cxx_source_files = cxx_source_files or {}
            table.join2(cxx_source_files, cxx_source_batch.sourcefiles, header_files, cxx_module_batch and cxx_module_batch.sourcefiles or {})
        end

        local c_source_batch = target:sourcebatches()["c.build"]
        local c_source_files
        local c_arguments
        if c_source_batch then
            for _, sourcefile in ipairs(c_source_batch.sourcefiles) do
                local args = compiler.compflags(sourcefile, {target = target})

                local ignore_next_arg = false
                for i, argument in ipairs(arguments) do
                        if ignore_next_arg then
                                ignore_next_arg = false
                                goto continue2
                        end

                        c_arguments = c_arguments or {}
                        c_argument, ignore_next_arg = parse_cxflags(argument, arguments)
                        table.append(c_arguments, c_argument)

                        ::continue2::
                 end
            end

            c_source_files = c_source_files or {}
            table.join2(c_source_files, c_source_batch.sourcefiles, header_files, c_module_batch and c_module_batch.sourcefiles or {})
        end

        local source_batches = {}

        if cxx_source_files then
            table.append(source_batches, { kind = cxx_source_batch.sourcekind, source_files = cxx_source_files, arguments = cxx_arguments or {}})
        end
        if c_source_files then
            table.append(source_batches, { kind = c_source_batch.sourcekind, source_files = c_source_files, arguments = c_arguments or {}})
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

        if use_qt and target:get("runenv") and target:get("runenv")["QML2_IMPORT_PATH"] then
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
                                module_files = cxx_module_batch.sourcefiles or {},
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
