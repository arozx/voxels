if not engine then
    engine.fatal("Engine API not available")
end

local debuggingEnabled = true

-- Set up UI and debug options
if debuggingEnabled then
    engine.log("Debug mode enabled")
    engine.showFPSCounter(true)
    engine.showRendererSettings(true)
    engine.showTerrainControls(true)
end

-- Initialize scene system
local function initializeScenes()
    -- Create and set up main scene
    if not engine.createScene("MainScene") then
        engine.error("Failed to create main scene")
        return false
    end

    if not engine.setActiveScene("MainScene") then
        engine.error("Failed to set main scene as active")
        return false
    end

    -- Set up camera and terrain
    engine.setCameraType("perspective")
    engine.setCameraPosition(0, 10, 10)
    engine.setCameraRotation(-45, 0)
    engine.setClearColor(0.2, 0.3, 0.3, 1.0)

    return true
end

-- Initialize game
if not initializeScenes() then
    engine.error("Scene initialization failed")
end
