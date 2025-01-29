if not engine then
    engine.fatal("Engine API not available")
end

-- Set render type and initialize appropriate renderer
local USE_2D = true -- Change this to false for 3D rendering

if USE_2D then
    engine.setRenderType("2d")
    engine.log("Initializing 2D renderer")
    renderer2d_initialize()
else
    engine.setRenderType("3d")
    engine.log("Initializing 3D renderer")
end

local debuggingEnabled = true

-- Set up UI and debug options
if debuggingEnabled then
    engine.log("Debug mode enabled")
    engine.showFPSCounter(true)
end

-- Initialize scene system
local function initializeScenes()
    -- Create and set up main scene
    if not engine.createScene("MainScene") then
        engine.error("Failed to create main scene")
        return false
    end

    -- Set camera and rendering settings before activating scene
    engine.setCameraType("orthographic")
    engine.setCameraPosition(0, 0, 0)
    engine.setClearColor(0.1, 0.1, 0.1, 1.0)

    -- Activate scene after configuration
    if not engine.setActiveScene("MainScene") then
        engine.error("Failed to set main scene as active")
        return false
    end

    return true
end

-- Initialize game
if not initializeScenes() then
    engine.error("Scene initialization failed")
end

-- Texture utilities
local textures = {
    checkerTexture = nil
}

local function createCheckerTexture()
    if not textures.checkerTexture then
        textures.checkerTexture = create_checker_texture()
    end
    return textures.checkerTexture
end

-- Cache the checker texture on startup
createCheckerTexture()

local quadRotation = 0.0

function UpdateScene()
    renderer2d_begin_scene()

    -- Draw background grid
    for y = -2.0, 2.0, 0.5 do
        for x = -3.0, 3.0, 0.5 do
            local color_r = (x + 3.0) / 6.0
            local color_g = (y + 2.0) / 4.0
            draw_quad(x, y, 0.45, 0.45, color_r, color_g, 0.7, 1.0)
        end
    end

    -- Draw animated quads
    draw_quad(-1.0, 0.0, 0.8, 0.8, 0.8, 0.2, 0.3, 1.0)
    draw_quad(0.5, -0.5, 0.5, 0.75, 0.2, 0.3, 0.8, 1.0)

    -- Draw textured quad with cached texture
    quadRotation = (quadRotation or 0) + 0.01
    draw_textured_quad(0.0, 0.5, 1.0, 1.0, textures.checkerTexture, 1.0)

    renderer2d_end_scene()
end
