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
