-- Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
-- This file is subject to the license terms in the LICENSE file
-- found in the top-level of this distribution


-- imports
import("core.project.config")
import("core.project.project")
import("core.tool.compiler")
import("core.base.json")

function string_starts(string,start)
   return string.sub(string, 1, string.len(start)) == start
end

function string_ends(string, ends)
   return string.sub(string, -string.len(ends)) == ends
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
        local header_files = {}

        for _, file in ipairs(target:headerfiles()) do
            file = path.absolute(file, project:directory()):gsub("%\\", "/")
            table.insert(header_files, file)
        end

        table.sort(header_files)

        local source_batches = {}

        local target_sourcebatches = {}
        for _, batch in pairs(target:sourcebatches()) do
            table.insert(target_sourcebatches, batch)
        end

        table.sort(target_sourcebatches, function (first, second) return first.rulename < second.rulename end)

        local last_cxx_arguments = {}
        for name, batch in ipairs(target_sourcebatches) do
            if batch.rulename == "qt.moc" or batch.rulename == "qt.qmltyperegistrar" then
                goto continue4
            end

            local source_files = {}

            for _, file in ipairs(batch.sourcefiles) do
                file = path.absolute(file, project:directory()):gsub("%\\", "/")
                table.insert(source_files, file)
            end

            local arguments = {}
            for _, file in ipairs(batch.sourcefiles) do
                if string_starts(batch.rulename, "c++.build") or string_starts(batch.rulename, "c.build") then
                    local args = compiler.compflags(file, {target = target})

                    local ignore_next_arg = false

                    for i, argument in ipairs(args) do
                            if ignore_next_arg then
                                    ignore_next_arg = false
                                    goto continue2
                            end

                            if string_starts(argument, "-I") then
                                    local p = argument:sub(3, argument:len())
                                    p = path.absolute(p, project:directory()):gsub("%\\", "/")

                                    table.insert(arguments, format("-I%s", p))
                            elseif string_starts(argument, "-external:I") then
                                    local p = argument:sub(12, argument:len())
                                    p = path.absolute(p, project:directory()):gsub("%\\", "/")

                                    table.insert(arguments, format("-external:I%s", p))
                            elseif(string_starts(argument, "-isystem")) then
                                    table.insert(arguments, argument .. " ".. args[i + 1])
                                    ignore_next_arg = true
                            elseif string_starts(argument, "/I") then
                                    local p = argument:sub(3, argument:len())
                                    p = path.absolute(p, project:directory()):gsub("%\\", "/")

                                    table.insert(arguments, format("/I%s", p))
                            elseif string_starts(argument, "/external:I") then
                                    local p = argument:sub(12, argument:len())
                                    p = path.absolute(p, project:directory()):gsub("%\\", "/")

                                    table.insert(arguments, format("/external:I%s", p))
                            elseif(string_starts(argument, "/isystem")) then
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

                     source_files = table.join(source_files, header_files)
                end

            end


            local kind = ""
            if batch.sourcekind then
                kind = batch.sourcekind
            else
                kind = "unknown"
            end

            table.insert(source_batches, { kind = kind, source_files = source_files, arguments = arguments})

            ::continue4::
        end



        local target_file = target:targetfile()

        if target_file then
            target_file = path.absolute(target_file, project:directory()):gsub("%\\", "/")
        else
            target_file = ""
        end

        local defined_in = path.absolute("xmake.lua", target:scriptdir()):gsub("%\\", "/")

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
                p = path.absolute(p, project:directory()):gsub("%\\", "/")
                table.append(qml_import_path, p)
            end
        end

        table.insert(targets, { name = name,
                                kind = target:targetkind(),
                                defined_in = defined_in,
                                source_batches = source_batches,
                                header_files = header_files,
                                target_file = target_file,
                                packages = target:get("packages"),
                                frameworks = target:get("frameworks"),
                                use_qt = use_qt,
                                group = target:get("group") } )
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

    local project_files = {}
    for _, project_file in ipairs((project:allfiles())) do
        project_file = project_file:gsub("%\\", "/")
        table.insert(project_files, project_file)
    end
    output.qml_import_path = qml_import_path
    output.project_files = project_files


    print(json.encode(output))
end
