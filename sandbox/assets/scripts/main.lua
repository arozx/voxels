-- Add safe engine access with error handling
local function getEngine()
    if not engine then
        error("Engine API not available")
        return nil
    end
    return engine
end

-- Move initialization check to a function to allow re-initialization on reload
local function initialize()
    local e = getEngine()
    if not e then return false end

    -- If scene type changed, force reinitialization
    if _G.lastUsed3D ~= nil and _G.lastUsed3D ~= USE_3D then
        _G.sceneInitialized = false
        e.log("Scene type changed, reinitializing...")
    end

    if _G.initialized and _G.sceneInitialized then
        e.log("Already initialized, skipping initialization")
        return true
    end

    -- Store current scene type preference
    _G.lastUsed3D = USE_3D

    local e = getEngine()
    if not e then return false end

    local USE_3D = false

    local textures = {
        checkerTexture = nil
    }

    local function initialize2DScene()
        e.log("Initializing 2D renderer...")
        e.setRenderType("2d")
        -- Basic scene setup without additional features
        if not e.createScene("MainScene") then
            e.error("Failed to create MainScene")
            return false
        end

        if not e.setActiveScene("MainScene") then
            e.error("Failed to set active scene")
            return false
        end

        e.log("abs")
        e.renderer2DInitialize()
        e.log("instalised 2d")
        e.setCameraType("orthographic")
        e.log("set cam")
        e.setCameraPosition(0, 0, 0)

        e.log("2D setup complete.")
        return true
    end

    local function initialize3DScene()
        e.log("Initializing 3D renderer...")
        e.setRenderType("3d")
        if not e.createScene("Test3DScene") then
            e.error("Failed to create Test3DScene")
            return false
        end

        if not e.setActiveScene("Test3DScene") then
            e.error("Failed to set active scene")
            return false
        end

        -- Configure camera only after scene is set up
        if not e.setCameraType("perspective") then
            e.error("Failed to set camera type")
            return false
        end

        if not e.setCameraPosition(0, 5, -10) then
            e.error("Failed to set camera position")
            return false
        end

        local cube = e.createCube("TestCube")
        if cube then
            cube.transform:SetPosition(0, 0, 0)
            cube.transform:SetScale(1, 1, 1)
            e.log("Created test cube")
        else
            e.error("Failed to create cube")
            return false
        end

        e.log("3D setup complete.")
        return true
    end

    if not _G.sceneInitialized then
        if USE_3D then
            if not initialize3DScene() then
                e.error("3D scene initialization failed")
                return
            end
        else
            if not initialize2DScene() then
                e.error("2D scene initialization failed")
                return
            end
        end
        _G.sceneInitialized = true
    end

    -- Debug setup
    e.log("Debug mode enabled.")
    e.showFPSCounter(true)

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
                e.drawQuad(
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
            e.drawQuad(
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
            e.drawQuad(
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
            textures.checkerTexture = e.createCheckerTexture()
        end

        -- Draw textured quads with different tiling factors
        local baseX = -0.5
        local tilingFactors = { 1.0, 2.0, 4.0 }

        for i, tiling in ipairs(tilingFactors) do
            e.drawTexturedQuad(
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
            if e.getActiveSceneName() == "Test3DScene" then
                local time = os.clock()
                local radius = 10.0
                e.setCameraPosition(
                    math.sin(time) * radius,
                    5,
                    math.cos(time) * radius
                )
                e.setCameraRotation(0, -time)

                local cube = e.getObject("TestCube")
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
            if e.getActiveSceneName() == "MainScene" then
                if not e.renderer2DBeginScene then
                    e.error("renderer2DBeginScene not available")
                    return
                end
                e.renderer2DBeginScene()

                -- Get current time for animation
                local time = os.clock()
                drawCheckerboard(8, 8, 0.2, time)

                drawAnimatedQuads(time)

                drawTexturedQuads()

                if not e.renderer2DEndScene then
                    e.error("renderer2DEndScene not available")
                    return
                end
                e.renderer2DEndScene()
            end
        end
    end

    local function createCheckerTexture()
        if not textures.checkerTexture then
            textures.checkerTexture = e.createCheckerTexture()
        end
        return textures.checkerTexture
    end

    createCheckerTexture()

    _G.initialized = true
    e.log("Initialization complete")
    return true
end

-- Keep track of global state that should persist across reloads
if _G.USE_3D == nil then
    _G.USE_3D = false -- Default to 2D
end
USE_3D = _G.USE_3D    -- Local reference to global state

-- Function to toggle between 2D and 3D
function toggleDimension()
    local e = getEngine()
    if not e then return end

    _G.USE_3D = not _G.USE_3D
    USE_3D = _G.USE_3D
    _G.sceneInitialized = false -- Force scene reinitialization
    e.log("Switching to " .. (USE_3D and "3D" or "2D") .. " mode")
    initialize()                -- Reinitialize with new mode
end

-- Protect the initialization call
local success, err = pcall(initialize)
if not success then
    if engine then -- Check if engine exists before using it
        engine.error("Initialization failed - " .. tostring(err))
    end
    return
end

-- ...rest of existing code...

function update()
    local e = getEngine()
    if not e then return end

    -- Update script watcher to check for changes
    e.updateScriptWatcher()

    -- Call existing update function if it exists
    if UpdateScene then
        UpdateScene()
    end
end
