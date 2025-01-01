# FILE: /hello-opengl-app/hello-opengl-app/README.md
# Hello OpenGL App

This is a simple C++ application that uses OpenGL to render a "Hello, World!" message on the screen.

## Requirements

To build and run this application on Arch Linux, you need to install the following packages:

- `cmake`
- `gcc`
- `glfw`
- `glm`
- `mesa`

You can install these dependencies using the following command:

```bash
sudo pacman -S cmake gcc glfw glm mesa
```

## Building the Application

1. Clone the repository or download the source code.
2. Navigate to the project directory:

   ```bash
   cd hello-opengl-app
   ```

3. Create a build directory:

   ```bash
   mkdir build
   cd build
   ```

4. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

5. Build the application:

   ```bash
   make
   ```

## Running the Application

After building the application, you can run it with the following command:

```bash
./hello-opengl-app
```

You should see a window displaying "Hello, World!" rendered using OpenGL.