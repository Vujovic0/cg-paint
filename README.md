# gl-paint

`gl-paint` is a Windows desktop drawing application written in C++ and
OpenGL. It provides an interactive canvas for drawing with different brush
shapes, changing colors, resizing the canvas, and transforming the drawing.

## Requirements

- Windows 10 or later
- Visual Studio with the **Desktop development with C++** workload
- C++20 support
- OpenGL 3.3-compatible graphics hardware and drivers
- [vcpkg](https://github.com/microsoft/vcpkg)
- The following libraries installed through vcpkg:
  - GLFW3
  - GLEW
  - FreeType

The application also expects these files to be available in its working
directory:

- `canvas.vert`
- `canvas.frag`
- `basic.vert`
- `basic.frag`
- `text.vert`
- `text.frag`
- `pencil.png`

The renderer loads Arial from `C:\Windows\Fonts\arial.ttf`.

## Installing dependencies

Install the required libraries with vcpkg:

```powershell
vcpkg install glfw3 glew freetype
vcpkg integrate install
```

If vcpkg is not installed, install it before running these commands.

## Building and running

1. Open `cg-paint.sln` or `cg-paint\cg-paint.vcxproj` in Visual Studio.
2. Select the `Debug` or `Release` configuration.
3. Select the `x64` platform.
4. Build the project with **Build > Build Solution**.
5. Run it with **Debug > Start Without Debugging**.

Run the application from the `cg-paint` directory so that the shader files
and `pencil.png` can be found. The application opens in a fullscreen window.

## Controls

### Mouse

- **Left click and drag**: Draw on the canvas
- **Middle click and drag**: Pan the canvas
- **Mouse wheel**: Change brush size
- **Ctrl + mouse wheel**: Zoom in or out
- **Drag a canvas edge**: Resize the canvas

### Keyboard

- **1**: Select a circular brush
- **2**: Select a triangular brush
- **3**: Select a square brush
- **H**: Flip the canvas horizontally
- **V**: Flip the canvas vertically
- **R**: Rotate the canvas clockwise
- **Shift + R**: Rotate the canvas counterclockwise
- **E**: Toggle between brush and eraser
- **Esc**: Exit the application

### Color toolbar

Enter values from `0` to `255` in the `R`, `G`, `B`, and `A` fields. Press
**Enter** or click **OK** to apply the selected color to the brush.
