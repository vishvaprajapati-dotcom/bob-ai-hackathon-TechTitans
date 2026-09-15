# Setup Guide

## Prerequisites
- Windows, Linux, or macOS
- GNU g++ with C++17 support
- A terminal / command prompt
- A PPM image for input

No OpenCV installation is required.

## Build

From the repository root:

### Windows
```cmd
g++ -std=c++17 -O2 src\solar_monitor.cpp -o solar_monitor.exe
g++ -std=c++17 -O2 src\generate_demo_image.cpp -o generate_demo_image.exe
```

### Linux/macOS
```bash
g++ -std=c++17 -O2 src/solar_monitor.cpp -o solar_monitor
g++ -std=c++17 -O2 src/generate_demo_image.cpp -o generate_demo_image
```

## Generate a Demo Input

Windows:
```cmd
generate_demo_image.exe demo_input.ppm
```

Linux/macOS:
```bash
./generate_demo_image demo_input.ppm
```

## Run the Detector

Windows:
```cmd
solar_monitor.exe demo_input.ppm demo_output.ppm report.txt
```

Linux/macOS:
```bash
./solar_monitor demo_input.ppm demo_output.ppm report.txt
```

## Verify
A successful run prints the detected region and creates:
- `demo_output.ppm`
- `report.txt`

Open the PPM image using an image viewer that supports PPM, or convert it to PNG for presentation.

## Environment Variables
No environment variables are required for the local prototype.

`.env.example` is included only as a placeholder for future IBM service integration.

## Troubleshooting

| Problem | Fix |
|---|---|
| `g++ is not recognized` | Install MinGW-w64/GCC and add its `bin` folder to PATH |
| Input file cannot be opened | Check the filename and current directory |
| Unsupported image | Convert the image to binary PPM (P6) |
| Output looks incorrect | Verify the input is a valid P6 PPM image |
| No defect is obvious | The prototype is a heuristic; test with a clearer abnormal region |

## Clean-Machine Test
Clone the repository on another machine, install only a C++17 compiler, follow the commands above, generate the demo image, and run the detector.
