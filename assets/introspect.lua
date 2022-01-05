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
    for name, target in pairs((project:targets())) do
        local source_batches = {}

        local target_sourcebatches = {}
        for _, batch in pairs(target:sourcebatches()) do
            table.insert(target_sourcebatches, batch)
        end
        table.sort(target_sourcebatches, function (first, second) return first.rulename < second.rulename end)

        local last_cxx_arguments = {}
        for name, batch in ipairs(target_sourcebatches) do
            local source_files = {}
            for _, file in ipairs(batch.sourcefiles) do
                file = path.absolute(file, project:directory()):gsub("%\\", "/")
                table.insert(source_files, file)
            end

            local arguments = {}
            for _, file in ipairs(batch.sourcefiles) do
                if string_ends(file, ".mpp") or string_ends(file, ".ixx") or string_ends(file, ".cppm") or string_ends(file, ".mxx") then -- can't do better for now with C++20 modules
                   arguments = last_cxx_arguments
                   if compiler.has_flags("cxx", "-fmodules-ts") then
                       table.insert(arguments, "-fmodules-ts")
                   elseif compiler.has_flags("cxx", "-fmodules") then
                       table.insert(arguments, "-fmodules-ts")
                   elseif compiler.has_flags("cxx", "/experimental:module") then
                       table.insert(arguments, "/experimental:module")
                   end
                   break
                end

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
                    elseif(string_starts(argument, "-isystem")) then
                        table.insert(arguments, argument .. " ".. args[i + 1])
                        ignore_next_arg = true
                    else
                        table.insert(arguments, argument)
                    end

                    ::continue2::
                 end

                 if batch.sourcekind and batch.sourcekind == "cxx" then
                    last_cxx_arguments = arguments
                 end
                 break
            end

            kind = ""
            if batch.rulename == "c++.build.modules" then
                kind = "cxxmodule"
            elseif batch.sourcekind then
                kind = batch.sourcekind
            else
                kind = "unknown"
            end

            table.insert(source_batches, { kind = kind, source_files = source_files, arguments = arguments, languages = target:get("languages") })
        end

        local header_files = {}

        for _, file in ipairs(target:headerfiles()) do
            file = path.absolute(file, project:directory()):gsub("%\\", "/")
            table.insert(header_files, file)
        end

        table.sort(header_files)

        local target_file = target:targetfile()
        target_file = path.absolute(target_file, project:directory()):gsub("%\\", "/")

        local defined_in = path.absolute("xmake.lua", target:scriptdir()):gsub("%\\", "/")

        table.insert(targets, { name = name, kind = target:targetkind(), defined_in = defined_in, source_batches = source_batches, header_files = header_files, target_file = target_file, packages = target:get("packages"), group = target:get("group") } )
    end

    table.sort(targets, function(first, second) return first.name > second.name end)

    output.targets = targets

    local project_files = {}
    for _, project_file in ipairs((project:allfiles())) do
        project_file = project_file:gsub("%\\", "/")
        table.insert(project_files, project_file)
    end
    output.project_files = project_files


    print(json.encode(output))
end
