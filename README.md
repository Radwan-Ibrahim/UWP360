# UWP360 — 360° Panorama Image Control for UWP

A lightweight **Windows Runtime component** that renders 360° equirectangular (panorama) images inside any **UWP XAML** app using **Direct3D 11**. Drop it into a page, give it an image URL, and let users drag to look around.

```xml
<dx:Image360 TextureSource="https://example.com/my-panorama.jpg" />
```

---

## Features

- **One-line XAML usage**: a plain `SwapChainPanel`-based control, no extra setup.
- **Remote or local images**: `http(s)://` URLs are downloaded, other paths are read from the app package.
- **Touch and mouse navigation**: drag horizontally to yaw, vertically to pitch.
- **Swap images at runtime**: setting `TextureSource` again replaces the texture without reloading the mesh.
- **Battery friendly**: the render loop runs on a background thread synced to the display's vertical blank (VBlank).
- **Robust**: handles resizing, DPI and zoom scale, app suspend, and GPU device loss.
- **Usable from any language**: C#, VB, C++/CX, or JavaScript (WinRT component).

---

## Repository layout

```
UWP360/
├── UWP360.sln                  # Visual Studio solution
├── DirectX360Image/            # C++/CX WinRT component (the control)
│   ├── Image360.h/.cpp         # Public XAML control: properties, input, render loop
│   ├── Camera.h/.cpp           # First-person camera (view + projection)
│   ├── Transform.h/.cpp        # World transform of the sphere
│   ├── VSD3DStarter.h          # Graphics, Mesh and .cmo loader (VS 3D starter kit, customized)
│   ├── Common/                 # DirectXPanelBase, StepTimer, DirectXHelper
│   ├── DirectXTex/             # DDS / WIC texture loaders (Microsoft DirectXTK)
│   ├── Animation/              # Optional skinned mesh support
│   ├── Sphere.cmo              # Sphere mesh rendered from the inside
│   └── *Phong*.cso             # Compiled pixel shader
├── 360ImageTest/               # C# UWP sample app
└── docs/
    └── ARCHITECTURE.md         # Detailed architecture description
```

---

## Getting started

### Requirements

| Item | Version |
|---|---|
| Visual Studio | 2017 or later with **Universal Windows Platform development** and **C++ (v141) UWP tools** |
| Windows SDK | 10.0.14393 (target), 10.0.10586 (minimum) |
| GPU | Direct3D feature level 9.1+ (falls back to WARP) |

### Build and run the sample

1. Open `UWP360.sln` in Visual Studio.
2. Set **360ImageTest** as the startup project.
3. Choose a platform (`x86`, `x64` or `ARM`), then press **F5**.
4. Drag inside the window to look around. Click **Change image** to load another panorama.

### Use it in your own app

1. Add `DirectX360Image.vcxproj` to your solution and reference it from your UWP app.
2. If you load images from the web, declare the `internetClient` capability in `Package.appxmanifest`.
3. Add the namespace and the control to your page:

```xml
<Page
    xmlns:dx="using:DirectX360Image" ...>
    <Grid>
        <dx:Image360 x:Name="Viewer"
                     TextureSource="https://example.com/panorama.jpg" />
    </Grid>
</Page>
```

4. Optionally change the image from code:

```csharp
Viewer.TextureSource = new Uri("https://example.com/another-panorama.jpg");
```

---

## Public API

| Member | Type | Description |
|---|---|---|
| `TextureSource` | `Uri` (DependencyProperty) | Equirectangular image to display. Supports `http(s)` and package-relative paths. |
| `Camera` | `DirectX360Image.Camera` | The viewer camera. Exposes `Pitch`, `RotateY`, `LookAt`, `SetLens` and frustum properties. |
| `Transform` | `DirectX360Image.Transform` | Scale, rotation and translation of the sphere. |
| `Clear(Color)` | method | Clears the render target to the given color. |
| `ModelLoaded` | `EventHandler<bool>` | Raised when the sphere and the first texture are ready. |

---

## How it works

```mermaid
flowchart LR
    A[XAML sets TextureSource] --> B[Image360]
    B -->|first time| C[Load Sphere.cmo + texture]
    B -->|later| D[Swap texture only]
    C --> E[Render loop thread]
    D --> E
    E -->|every VBlank| F[Draw sphere from inside]
    F --> G[Present to SwapChainPanel]
    H[Drag gesture] --> I[Camera yaw/pitch] --> E
```

The image is mapped onto a sphere, and the camera sits at its center. Back-face culling is turned off, so the inner surface is visible. Dragging rotates the camera, which gives the effect of looking around a 360° scene.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for full details.

---

## Roadmap

- [ ] Publish as a NuGet package
- [ ] Gyroscope or accelerometer navigation (field is already reserved)
- [ ] Pinch-to-zoom (adjust field of view)
- [ ] Loading and error events for failed downloads

---

## Credits

- Rendering scaffolding comes from the Visual Studio **Direct3D starter kit** (`VSD3DStarter.h`, `Common/`).
- Texture loaders come from Microsoft **DirectXTK** (`DirectXTex/`).
