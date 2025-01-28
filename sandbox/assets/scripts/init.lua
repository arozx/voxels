engine.trace("Starting init.lua execution")

-- Create build directory structure first
local buildDir = "build/assets/scripts"
local scriptDir = "sandbox/assets/scripts"

engine.trace("Creating build directory: " .. buildDir)
os.execute("mkdir -p " .. buildDir)

local function copyScript(name)
    -- Try to read source file directly
    local sourcePath = scriptDir .. "/" .. name
    local source = io.open(sourcePath, "rb")
    if not source then
        engine.error("Could not find source script: " .. sourcePath)
        return false
    end
    
    local content = source:read("*all")
    source:close()
    
    -- Write to build directory
    local destPath = buildDir .. "/" .. name
    local dest = io.open(destPath, "wb")
    if not dest then
        engine.error("Failed to create destination file: " .. destPath)
        return false
    end
    
    dest:write(content)
    dest:close()
    engine.trace("Successfully copied: " .. name)
    return true
end

-- Only copy scripts, don't execute them
local scripts = { "main.lua", "engine.lua" }
for _, script in ipairs(scripts) do
    if not copyScript(script) then
        engine.error("Failed to copy " .. script)
        return
    end
end

engine.trace("init.lua execution complete")

