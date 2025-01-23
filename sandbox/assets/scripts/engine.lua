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
---@field loadScript fun(filepath: string): boolean # Loads and executes a Lua script file
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

---Logs an info message to the console
---@param message string The message to log
function engine.log(message) end

---Logs an error message to the console
---@param message string The error message to log
function engine.error(message) end

---Loads and executes a Lua script file
---@param filepath string Path to the script file
---@return boolean success Whether the script was loaded successfully
function engine.loadScript(filepath) end