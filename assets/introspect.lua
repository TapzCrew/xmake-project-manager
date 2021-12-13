-- imports
import("core.project.config")
import("core.project.project")

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
        table.insert(targets, { name = name, kind = target:targetkind(), defined_in = format("%s/%s", target:scriptdir(), "xmake.lua") })
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

    -- print data
    print("{\n    \"version\": \"%s\",\n    \"targets\": [", version)

    for i, target in ipairs(targets) do
        if i == target_count then
            print("        { \"name\": \"%s\", \"kind\": \"%s\", \"defined_in\": \"%s\" }", target.name, target.kind, target.defined_in)
        else
            print("        { \"name\": \"%s\", \"kind\": \"%s\", \"defined_in\": \"%s\" },", target.name, target.kind, target.defined_in)
        end
    end

    print("    ],")
    print("\"project_files\": [")

    for i, project_file in ipairs(project_files) do
        if i == project_file_count then
            print("\"%s\"", project_file)
        else
            print("\"%s\",", project_file)
        end
    end

    print("    ]\n}")
end
