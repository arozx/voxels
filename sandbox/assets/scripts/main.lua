-- Initialize Lua environment
if not engine then
    error("Engine API not available")
end

local debuggingEnabled = true

-- Set default terrain parameters
engine.setTerrainHeight(10.0)

if debuggingEnabled then
    engine.log("debuggingEnabled")
else 
    engine.showFPSCounter(false)
    engine.showEventDebugger(false)
    engine.showRendererSettings(false)
    engine.showTerrainControls(false)
    engine.showProfiler(false)
end

