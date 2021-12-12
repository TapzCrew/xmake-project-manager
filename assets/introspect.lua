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
    for name, target in pairs((project.targets())) do
	table.insert(targets, { name = name, kind = target:targetkind() })
        target_count = target_count + 1
    end
    table.sort(targets, function(first, second) return first.name > second.name end)

    -- print data
    print("{\n    \"version\": \"%s\",\n    \"project_name\": \"%s\",\n    \"targets\": [", version, project:name())

    for i, target in ipairs(targets) do
        if i == target_count then
	    print("        { \"name\": \"%s\", \"kind\": \"%s\" }", target.name, target.kind)
        else
	    print("        { \"name\": \"%s\", \"kind\": \"%s\" },", target.name, target.kind)
        end
    end

    print("    ]\n}")
end
