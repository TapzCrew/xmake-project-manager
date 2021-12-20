-- imports
import("core.project.config")
import("core.project.project")
import("core.tool.compiler")

function header_files_to_json(header_files)
    HEADER_FILE_JSON = [[
               "%s"%s
]]

    table.sort(header_files)

    local output = ""

    local separator = ","
    for i, file in ipairs(header_files) do
        if i == #header_files then
            separator = ""
        end

        output = output .. format(SOURCE_FILE_JSON, file, separator)
    end

    return output
end

function source_files_to_json(source_files)
    SOURCE_FILE_JSON = [[
                   "%s"%s
]]

    table.sort(source_files)

    local output = ""

    local separator = ","
    for i, file in ipairs(source_files) do
        if i == #source_files then
            separator = ""
        end

        output = output .. format(SOURCE_FILE_JSON, file, separator)
    end

    return output
end

function arguments_to_json(arguments)
    ARGUMENTS_JSON = [[
                   "%s"%s
]]

    table.sort(arguments)

    local output = ""

    local separator = ","
    for i, arg in ipairs(arguments) do
       if i == #arguments then
           separator = ""
       end

       output = output .. format(ARGUMENTS_JSON, arg, separator)
    end

    return output
end

function source_batches_to_json(source_batches)
    SOURCE_BATCH_JSON = [[
              {
                "kind": "%s",
                "files": [
%s
                ],
                "arguments": [
%s
                ]
              }%s]]

    local output = ""

    local separator = ","
    for i, batch in ipairs(source_batches) do
        if i == #source_batches then
            separator = ""
        end

        local source_files_json = source_files_to_json(batch.source_files)
        local arguments_json = arguments_to_json(batch.arguments)

        output = output .. format(SOURCE_BATCH_JSON, batch.kind, source_files_json, arguments_json, separator)
    end

    return output
end

function project_files_to_json(project_files)
    PROJECT_FILE_JSON = [[
          "%s"%s]]

    local output = ""

    local separator = ","
    for i, project_file in ipairs(project_files) do
        if i == #project_files then
            separator = ""
        end

        output = output .. format(PROJECT_FILE_JSON, project_file, separator)
    end

    return output
end

function targets_to_json(targets)
    TARGET_JSON = [[
          {
            "name": "%s",
            "kind": "%s",
            "defined_in": "%s",
            "source_batches": [
%s
            ],
            "headers": [
%s
            ],
            "target_file": "%s"
          }%s]]

    local output = ""

    local separator = ","
    for i, target in ipairs(targets) do
        local source_batches_json = source_batches_to_json(target.source_batches)
        local header_files_json = header_files_to_json(target.header_files)

        if i == #targets then
            separator = ""
        end

        output = output .. format(TARGET_JSON, target.name, target.kind, target.defined_in, source_batches_json, header_files_json, target.target_file, separator)
    end

    return output
end

-- main entry
function main ()
    -- load config
    config.load()

    -- add version info
    local version = xmake.version()

    -- add targets data
    local targets = {}
    for name, target in pairs((project:targets())) do
        local source_batches = {}

        for name, batch in pairs(target:sourcebatches()) do
            local arguments = {}

            for _, file in ipairs(batch.sourcefiles) do
                arguments = compiler.compflags(file, {target = target})
                break
            end

            table.insert(source_batches, { kind = batch.sourcekind, source_files = batch.sourcefiles, arguments = arguments } )
        end

        local header_files = {}

        for _, file in ipairs(target:headerfiles()) do
            table.insert(header_files, file)
        end

        table.sort(header_files)

        table.insert(targets, { name = name, kind = target:targetkind(), defined_in = format("%s/%s", target:scriptdir(), "xmake.lua"), source_batches = source_batches, header_files = header_files, target_file = target:targetfile() })
    end
    table.sort(targets, function(first, second) return first.name > second.name end)

    local project_files = {}
    for _, project_file in ipairs((project:allfiles())) do
        table.insert(project_files, project_file)
    end

    JSON_OUTPUT = [[
    {
      "version": "%s",
      "project_files": [
%s
       ],
      "targets": [
%s
      ]
    }]]

    project_files_json = project_files_to_json(project_files)
    targets_json = targets_to_json(targets)

    print(format(JSON_OUTPUT, version, project_files_json, targets_json))
end
