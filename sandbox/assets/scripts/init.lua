-- Initialize Lua environment
if not engine then
    error("Engine API not available")
end

-- Set default terrain parameters
engine.setTerrainHeight(10.0)
engine.log("Lua environment initialized")