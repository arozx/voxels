-- Add initialization guard to prevent multiple executions
if _G.initialized then return end
_G.initialized = true
if not engine then
    engine.fatal("Engine API not available")
end

local USE_3D = false

local textures = {
    checkerTexture = nil
}

local function initialize2DScene()
    engine.log("[Lua]: Initializing 2D renderer...")
    engine.setRenderType("2d")
    -- Basic scene setup without additional features
    if not engine.createScene("MainScene") then
        engine.error("[Lua]: Failed to create MainScene")
        return false
    end

    if not engine.setActiveScene("MainScene") then
        engine.error("[Lua]: Failed to set active scene")
        return false
    end

    engine.log("abs")
    engine.renderer2DInitialize()
    engine.log("instalised 2d")
    engine.setCameraType("orthographic")
    engine.log("set cam")
    engine.setCameraPosition(0, 0, 0)

    engine.log("[Lua]: 2D setup complete.")
    return true
end

local function initialize3DScene()
    engine.log("[Lua]: Initializing 3D renderer...")
    engine.setRenderType("3d")
    if not engine.createScene("Test3DScene") then
        engine.error("[Lua]: Failed to create Test3DScene")
        return false
    end

    if not engine.setActiveScene("Test3DScene") then
        engine.error("[Lua]: Failed to set active scene")
        return false
    end

    -- Configure camera only after scene is set up
    if not engine.setCameraType("perspective") then
        engine.error("[Lua]: Failed to set camera type")
        return false
    end

    if not engine.setCameraPosition(0, 5, -10) then
        engine.error("[Lua]: Failed to set camera position")
        return false
    end

    local cube = engine.createCube("TestCube")
    if cube then
        cube.transform:SetPosition(0, 0, 0)
        cube.transform:SetScale(1, 1, 1)
        engine.log("[Lua]: Created test cube")
    else
        engine.error("[Lua]: Failed to create cube")
        return false
    end

    engine.log("[Lua]: 3D setup complete.")
    return true
end

if not _G.sceneInitialized then
    if USE_3D then
        if not initialize3DScene() then
            engine.error("[Lua]: 3D scene initialization failed")
            return
        end
    else
        if not initialize2DScene() then
            engine.error("[Lua]: 2D scene initialization failed")
            return
        end
    end
    _G.sceneInitialized = true
end

-- Debug setup
engine.log("[Lua]: Debug mode enabled.")
engine.showFPSCounter(true)

local colors = {
    { 1.0, 0.0, 0.0, 1.0 }, -- Red
    { 0.0, 1.0, 0.0, 1.0 }, -- Green
    { 0.0, 0.0, 1.0, 1.0 }, -- Blue
    { 1.0, 1.0, 0.0, 1.0 }, -- Yellow
    { 1.0, 0.0, 1.0, 1.0 }, -- Magenta
    { 0.0, 1.0, 1.0, 1.0 }  -- Cyan
}

local function drawCheckerboard(rows, cols, size, time)
    local startX = -(cols * size) / 2
    local startY = -(rows * size) / 2

    for row = 0, rows - 1 do
        for col = 0, cols - 1 do
            local x = startX + (col * size)
            local y = startY + (row * size)

            -- Create animated color selection based on position and time
            local colorIndex = ((row + col + math.floor(time)) % #colors) + 1
            local color = colors[colorIndex]

            -- Draw the checker square
            engine.drawQuad(
                x + (size / 2), -- Center position X
                y + (size / 2), -- Center position Y
                size * 0.95,    -- Slightly smaller than cell size to create grid effect
                size * 0.95,    -- Same for height
                color[1], color[2], color[3], color[4]
            )
        end
    end
end

-- Add animation utilities
local animations = {
    time = 0,
    rotatingQuads = {
        { x = -0.8, y = 0.8,  size = 0.15, rotation = 0 },
        { x = 0.8,  y = 0.8,  size = 0.15, rotation = math.pi / 4 },
        { x = 0.8,  y = -0.8, size = 0.15, rotation = math.pi / 2 },
        { x = -0.8, y = -0.8, size = 0.15, rotation = math.pi * 3 / 4 }
    },
    bouncingQuads = {
        { x = 0,    y = 0.8, size = 0.1, baseY = 0.8 },
        { x = 0.3,  y = 0.8, size = 0.1, baseY = 0.8 },
        { x = -0.3, y = 0.8, size = 0.1, baseY = 0.8 }
    }
}

local function drawAnimatedQuads(time)
    -- Draw rotating quads
    for _, quad in ipairs(animations.rotatingQuads) do
        quad.rotation = quad.rotation + time * 2
        engine.drawQuad(
            quad.x,
            quad.y,
            quad.size,
            quad.size,
            1.0, 1.0, 1.0, 1.0 -- White color
        )
    end

    -- Draw bouncing quads with different phases
    for i, quad in ipairs(animations.bouncingQuads) do
        local phase = time * 4 + (i * math.pi / 4)
        local yOffset = math.sin(phase) * 0.2
        engine.drawQuad(
            quad.x,
            quad.baseY + yOffset,
            quad.size,
            quad.size,
            1.0, 0.5, 0.0, 1.0 -- Orange color
        )
    end
end

local function drawTexturedQuads()
    -- Only create texture once
    if not textures.checkerTexture then
        textures.checkerTexture = engine.createCheckerTexture()
    end
    
    -- Draw textured quads with different tiling factors
    local baseX = -0.5
    local tilingFactors = { 1.0, 2.0, 4.0 }

    for i, tiling in ipairs(tilingFactors) do
        engine.drawTexturedQuad(
            baseX + (i * 0.5),
            0,
            0.3,
            0.3,
            textures.checkerTexture,
            tiling
        )
    end
end

-- Combined update function for both modes
function UpdateScene()
    if USE_3D then
        if engine.getActiveSceneName() == "Test3DScene" then
            local time = os.clock()
            local radius = 10.0
            engine.setCameraPosition(
                math.sin(time) * radius,
                5,
                math.cos(time) * radius
            )
            engine.setCameraRotation(0, -time)

            local cube = engine.getObject("TestCube")
            if cube then
                local rotation = cube.transform:GetRotation()
                cube.transform:SetRotation(
                    rotation.x + 0.01,
                    rotation.y + 0.02,
                    rotation.z + 0.03
                )
            end
        end
    else
        if engine.getActiveSceneName() == "MainScene" then
            if not engine.renderer2DBeginScene then
                engine.error("renderer2DBeginScene not available")
                return
            end
            engine.renderer2DBeginScene()

            -- Get current time for animation
            local time = os.clock()
            drawCheckerboard(8, 8, 0.2, time)

            drawAnimatedQuads(time)

            drawTexturedQuads()

            if not engine.renderer2DEndScene then
                engine.error("renderer2DEndScene not available")
                return
            end
            engine.renderer2DEndScene()
        end
    end
end

local function createCheckerTexture()
    if not textures.checkerTexture then
        textures.checkerTexture = engine.createCheckerTexture()
    end
    return textures.checkerTexture
end

createCheckerTexture()
