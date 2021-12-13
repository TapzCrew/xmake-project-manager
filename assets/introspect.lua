-- imports
import("core.project.config")
import("core.project.project")

function source_files_to_json(source_files, source_file_count)
    SOURCE_FILE_JSON = [[
               "%s"%s
    ]]

    local output = ""

    for i, source_file in ipairs(source_files) do
        local separator = ","

        if i == source_file_count then
            separator = ""
        end

        output = output .. format(SOURCE_FILE_JSON, source_file, separator)
    end

    return output
end
function project_files_to_json(project_files, project_file_count)
    PROJECT_FILE_JSON = [[
         "%s"%s
    ]]

    local output = ""

    for i, project_file in ipairs(project_files) do
        local separator = ","

        if i == project_file_count then
            separator = ""
        end

        output = output .. format(PROJECT_FILE_JSON, project_file, separator)
    end

    return output
end

function targets_to_json(targets, targets_count)
    TARGET_JSON = [[
          {
            "name": "%s",
            "kind": "%s",
            "defined_in": "%s",
            "source_files": [
%s
            ]
          }%s
]]

    local output = ""

    for i, target in ipairs(targets) do
        local separator = ","

        local source_files_json = source_files_to_json(target.source_files, target.source_count)

        if i == targets_count then
            separator = ""
        end

        output = output .. format(TARGET_JSON, target.name, target.kind, target.defined_in, source_files_json, separator)
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
    local target_count = 0
    for name, target in pairs((project:targets())) do
        local source_files = {}
        local source_count = target:sourcecount()

        for _, file in ipairs(target:sourcefiles()) do
            table.insert(source_files, file)
        end

        table.sort(source_files)

        table.insert(targets, { name = name, kind = target:targetkind(), defined_in = format("%s/%s", target:scriptdir(), "xmake.lua"), source_files = source_files, source_count = source_count })
        target_count = target_count + 1
    end
    table.sort(targets, function(first, second) return first.name > second.name end)

    local project_files = {}
    local project_file_count = 0
    for i, file in ipairs((project:allfiles())) do
        table.insert(project_files, file)
        project_file_count = project_file_count + 1
    end

    table.sort(project_files)

    JSON_OUTPUT = [[
    {
      "version": "%s",
      "project_files": [
%s
       ],
      "targets": [
%s
      ]
    }
    ]]

    project_files_json = project_files_to_json(project_files, project_file_count)
    targets_json = targets_to_json(targets, target_count)

    print(format(JSON_OUTPUT, version, project_files_json, targets_json))
end
