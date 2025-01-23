-- Initialize Lua environment
if not engine then
    error("Engine API not available")
end

-- Set default terrain parameters
engine.setTerrainHeight(10.0)
engine.trace("Lua environment initialized")
engine.log("Lua environment initialized")
engine.warn("Lua environment initialized")
engine.error("Lua environment initialized")
