engine.trace("Starting init.lua execution")

-- Create build directory structure first
local buildDir = "build/assets/scripts"
local scriptDir = "sandbox/assets/scripts"

engine.trace("Creating build directory: " .. buildDir)
local function sanitizePath(path)
    -- Remove any non-alphanumeric characters except for safe separators
    return path:gsub('[^%w%./%-]', '')
        :gsub('%.%.', '') -- Remove path traversal sequences
        :gsub('//+', '/') -- Normalize multiple slashes
end

local sanitizedPath = sanitizePath(buildDir)
local success, _, code = os.execute('mkdir -p "' .. sanitizedPath .. '"')
if not success then
    engine.error(string.format("Failed to create directory %s (exit code: %d)", sanitizedPath, code))
    return
end

local function copyScript(name)
    local function closeFiles(source, dest)
        if source then source:close() end
        if dest then dest:close() end
    end
    local function isPathSafe(path)
        return not path:match("%.%.")
    end

    if not isPathSafe(name) then
        engine.error(string.format("Invalid script name (potential path traversal): %s", name))
        return false
    end

    local sourcePath = scriptDir .. "/" .. name
    local destPath = buildDir .. "/" .. name

    local source, sourceErr = io.open(sourcePath, "rb")
    if not source then
        engine.error(string.format("Could not open source script ", sourcePath, ": ", sourceErr))
        return false
    end
    
    local MAX_FILE_SIZE = 10 * 1024 * 1024  -- 10MB limit
    
    local size = source:seek("end")
    source:seek("set")
    if size > MAX_FILE_SIZE then
        engine.error(string.format("Script %s is too large (%d bytes > %d bytes limit)", 
                                 sourcePath, size, MAX_FILE_SIZE))
        closeFiles(source)
        return false
    end

    local content = source:read("*all")
    if not content then
        engine.error(string.format("Failed to read from ", sourcePath))
        closeFiles(source)
        return false
    end
    
    local dest, destErr = io.open(destPath, "wb")
    if not dest then
        engine.error(string.format("Failed to create destination file", destPath, ": ", destErr))
        closeFiles(source)
        return false
    end
    
    local success = dest:write(content)
    if not success then
        engine.error(string.format("Failed to write to ", destPath))
        closeFiles(source, dest)
        return false
    end

    closeFiles(source, dest)
    engine.trace(string.format("Successfully copied: ", name))
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

