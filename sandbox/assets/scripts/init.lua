--- Loads the new lua files into the build directory

local function copyScript(name)
    -- Define relative paths
    local sourcePath = "sandbox/assets/scripts/" .. name
    local destPath = "build/assets/scripts/" .. name
    
    -- Read source script
    local source = io.open(sourcePath, "rb")
    if not source then
        engine.error("Failed to open source script: " .. sourcePath)
        return false
    end
    
    local content = source:read("*all")
    source:close()
    
    -- Write to build directory
    local dest = io.open(destPath, "wb")
    if not dest then
        engine.error("Failed to create build script: " .. destPath)
        return false
    end
    
    local success = dest:write(content)
    dest:close()
    
    if success then
        engine.trace("Successfully copied: " .. name)
        return true
    else
        engine.error("Failed to write: " .. destPath)
        return false
    end
end

-- Copy main script
copyScript("main.lua")

