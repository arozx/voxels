---@meta

---@class engine
---@field setTerrainHeight fun(height: number) # Sets the base height for terrain generation
---@field generateTerrain fun(seed: number) # Regenerates terrain with given seed
---@field setClearColor fun(r: number, g: number, b: number, a: number) # Sets the renderer clear color
---@field isKeyPressed fun(keycode: number): boolean # Checks if a key is pressed
---@field getMousePosition fun(): number, number # Gets current mouse position
---@field trace fun(message: string) # Logs a trace message
---@field log fun(message: string) # Logs an info message
---@field warn fun(message: string) # Logs an warn message
---@field error fun(message: string) # Logs an error message
---@field profileFunction fun(name: string) # Profiles the current function
---@field profileScope fun(name: string) # Profiles the current function & assigns a name
---@field loadScript fun(filepath: string): boolean # Loads and executes a Lua script file
---@field setCameraPosition fun(x: number, y: number, z: number) # Sets camera position
---@field setCameraRotation fun(pitch: number, yaw: number) # Sets camera rotation
---@field getCameraPosition fun(): number, number, number # Gets camera position
---@field moveCameraForward fun(deltaTime: number) # Move camera forward
---@field moveCameraBackward fun(deltaTime: number) # Move camera backward
---@field moveCameraLeft fun(deltaTime: number) # Move camera left
---@field moveCameraRight fun(deltaTime: number) # Move camera right
---@field moveCameraUp fun(deltaTime: number) # Move camera up
---@field moveCameraDown fun(deltaTime: number) # Move camera down
---@field rotateCameraWithMouse fun(xOffset: number, yOffset: number, sensitivity: number) # Rotate camera with mouse
---@field setViewport fun(x: number, y: number, width: number, height: number) # Sets the viewport dimensions
---@field setCameraType fun(type: string) # Sets the camera type ("orthographic" or "perspective")
---@field getCameraType fun(): string # Gets the current camera type
---@field isMouseButtonPressed fun(button: number): boolean # Checks if a mouse button is pressed
---@field setMouseSensitivity fun(sensitivity: number) # Sets mouse sensitivity
---@field getMouseSensitivity fun(): number # Gets current mouse sensitivity
---@field setMovementSpeed fun(speed: number) # Sets movement speed
---@field getMovementSpeed fun(): number # Gets current movement speed
---@field toggleCameraControls fun() # Toggles camera controls on/off
---@field toggleMovementLock fun() # Toggles movement lock
---@field toggleSmoothCamera fun() # Toggles smooth camera movement
---@field showTransformControls fun(show: boolean) # Shows/hides transform controls window
---@field showProfiler fun(show: boolean) # Shows/hides profiler window
---@field showRendererSettings fun(show: boolean) # Shows/hides renderer settings window
---@field showEventDebugger fun(show: boolean) # Shows/hides event debugger window
---@field showTerrainControls fun(show: boolean) # Shows/hides terrain controls window
---@field showFPSCounter fun(show: boolean) # Shows/hides FPS counter
engine = {}

-- Key code constants
---@class KeyCode
---@field ESCAPE number
---@field SPACE number
---@field W number
---@field A number
---@field S number
---@field D number
KeyCode = {}

-- Function definitions with documentation
---Sets the terrain base height
---@param height number The height value to set
function engine.setTerrainHeight(height) end

---Generates new terrain with the given seed
---@param seed number The seed for terrain generation
function engine.generateTerrain(seed) end

---Sets the renderer clear color
---@param r number Red component (0-1)
---@param g number Green component (0-1)
---@param b number Blue component (0-1)
---@param a number Alpha component (0-1)
function engine.setClearColor(r, g, b, a) end

---Checks if a key is currently pressed
---@param keycode number The key code to check
---@return boolean
function engine.isKeyPressed(keycode) end

---Gets the current mouse position
---@return number x, number y
function engine.getMousePosition() end

---Logs an trace message to the console
---@param message string The message to log
function engine.trace(message) end

---Logs an info message to the console
---@param message string The message to log
function engine.log(message) end

---Logs an warn message to the console
---@param message string The message to log
function engine.warn(message) end

---Logs an error message to the console
---@param message string The error message to log
function engine.error(message) end

---Loads and executes a Lua script file
---@param filepath string Path to the script file
---@return boolean success Whether the script was loaded successfully
function engine.loadScript(filepath) end

---Profiles current function
---@param name string Name of the profiling session
function engine.profileFunction(name) end

---Profiles current function: assigns a name to the session
---@param name string Name of the profiling session
function engine.profileScope(name) end

---Sets the camera position
---@param x number X position
---@param y number Y position
---@param z number Z position
function engine.setCameraPosition(x, y, z) end

---Sets the camera rotation
---@param pitch number Pitch angle in degrees
---@param yaw number Yaw angle in degrees
function engine.setCameraRotation(pitch, yaw) end

---Gets the camera position
---@return number x, number y, number z
function engine.getCameraPosition() end

---Move camera forward
---@param deltaTime number Time since last frame
function engine.moveCameraForward(deltaTime) end

---Move camera backward
---@param deltaTime number Time since last frame
function engine.moveCameraBackward(deltaTime) end

---Move camera left
---@param deltaTime number Time since last frame
function engine.moveCameraLeft(deltaTime) end

---Move camera right
---@param deltaTime number Time since last frame
function engine.moveCameraRight(deltaTime) end

---Move camera up
---@param deltaTime number Time since last frame
function engine.moveCameraUp(deltaTime) end

---Move camera down
---@param deltaTime number Time since last frame
function engine.moveCameraDown(deltaTime) end

---Rotate camera with mouse movement
---@param xOffset number Mouse X movement
---@param yOffset number Mouse Y movement
---@param sensitivity number Mouse sensitivity
function engine.rotateCameraWithMouse(xOffset, yOffset, sensitivity) end

---Sets the viewport dimensions
---@param x number Viewport X position
---@param y number Viewport Y position
---@param width number Viewport width
---@param height number Viewport height
function engine.setViewport(x, y, width, height) end

---Sets the camera type
---@param type string Either "orthographic" or "perspective"
function engine.setCameraType(type) end

---Gets the current camera type
---@return string # Either "orthographic" or "perspective"
function engine.getCameraType() end

---Checks if a mouse button is pressed
---@param button number The mouse button to check
---@return boolean isPressed Whether the button is pressed
function engine.isMouseButtonPressed(button) end

---Sets the mouse sensitivity
---@param sensitivity number The new sensitivity value
function engine.setMouseSensitivity(sensitivity) end

---Gets the current mouse sensitivity
---@return number sensitivity The current sensitivity value
function engine.getMouseSensitivity() end

---Sets the movement speed
---@param speed number The new movement speed
function engine.setMovementSpeed(speed) end

---Gets the current movement speed
---@return number speed The current movement speed
function engine.getMovementSpeed() end

---Toggles camera controls on/off
function engine.toggleCameraControls() end

---Toggles movement lock
function engine.toggleMovementLock() end

---Toggles smooth camera movement
function engine.toggleSmoothCamera() end

---Shows/hides the transform controls window
---@param show boolean Whether to show the window
function engine.showTransformControls(show) end

---Shows/hides the profiler window
---@param show boolean Whether to show the window
function engine.showProfiler(show) end

---Shows/hides the renderer settings window
---@param show boolean Whether to show the window
function engine.showRendererSettings(show) end

---Shows/hides the event debugger window
---@param show boolean Whether to show the window
function engine.showEventDebugger(show) end

---Shows/hides the terrain controls window
---@param show boolean Whether to show the window
function engine.showTerrainControls(show) end

---Shows/hides the FPS counter
---@param show boolean Whether to show the counter
function engine.showFPSCounter(show) end
