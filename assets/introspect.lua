-- imports
import("core.project.config")
import("core.project.project")
import("core.tool.compiler")
import("core.base.json")

function string_starts(String,Start)
   return string.sub(String,1,string.len(Start))==Start
end


-- main entry
function main ()
    local output = {}

    -- load config
    config.load()

    -- add version info
    output.version = format("%s", xmake.version())

    -- add targets data
    local targets = {}
    for name, target in pairs((project:targets())) do
        local source_batches = {}

        for name, batch in pairs(target:sourcebatches()) do
	    local source_files = {}
	    for _, file in ipairs(batch.sourcefiles) do
		file = path.absolute(file, project:directory()):gsub("%\\", "/")
		table.insert(source_files, file)
	    end

	    local arguments = {}
	    for _, file in ipairs(batch.sourcefiles) do
		for i, argument in ipairs(compiler.compflags(file, {target = target})) do
		    if string_starts(argument, "-I") then
			local p = argument:sub(3, argument:len() - 2)
			p = path.absolute(project:directory(), p):gsub("%\\", "/")

			table.insert(arguments, format("-I%s", p))
		    else
			table.insert(arguments, argument)
		    end
		end
                break
            end

	    table.insert(source_batches, { kind = batch.sourcekind, source_files = source_files, arguments = arguments } )
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

	table.insert(targets, { name = name, kind = target:targetkind(), defined_in = defined_in, source_batches = source_batches, header_files = header_files, target_file = target_file })
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
