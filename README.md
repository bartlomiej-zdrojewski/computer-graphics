# Computer Graphics

This project is a simple implementation of basic computer graphics concepts. It aims to show how 3D space is transformed and projected onto 2D screen using as little OpenGL functions as possible. The SFML library, which is based on OpenGL, is used mainly for window management, event handling and fundamental operations, like clearing screen, drawing lines, etc. Bear in mind that this project is for educational purposes only. If you need efficient, well tested software, go with OpenGL instead.

Covered computer graphics concepts:
1. Vector and matrix multiplication in file `matrix4x4.cpp`
2. Transformation matrixes (identity, translation, rotation, perspective projection) in file `transform.cpp`
3. Vertex transformation in file `transform_engine.cpp`
4. Vertex clipping in file `clip_engine.cpp`
5. Depth aware polygon color filling in file `render_engine.cpp`

## Building

### Installing dependencies

```
sudo apt install cmake
sudo apt-get install libsfml-dev
```

For non Unix platforms see:
https://www.sfml-dev.org/tutorials/2.5/#getting-started

### Compiling

```
mkdir build
cd build
cmake ../
make
cd ..
```

## Launching

```
./build/computer-graphics [model-file-path] [font-file-path]
```

Parameters:
- `[model-file-path]` is path to model file, defaults to `assets/cube.txt`
- `[font-file-path]` is path to font file, defaults to `assets/font.ttf`

## Controls

```
W - move forwards
S - move backwards
A - turn left
D - turn right
Q - decrease field of view
E - increase field of view
Z - show/hide statistics
X - save window to png file
ESC - exit
```

## Model file format

Model file is a text file consisting of multiple lines. Non printable characters at beginning and end of the line are ignored. Supported line endings are LF and CRLF.

Each line either:
- starts with 6 real values separated by non printable characters, then it is considered a vertex with positional coordinates (x, y, z) and color coordinates (r, g, b), where color coordinates are real numbers from 0.00 to 1.00,
- starts with 3 real values separated by non printable characters, then it is considered a vertex with positional coordinates (x, y, z) and white color,
- starts with `#` character, then it is considered a comment,
- is empty, then it is ignored.

If a line does not fall into any of this categories, file has a wrong format and model will not be loaded.

Every 3 consecutive vertexes are interpreted as a **triangle** in 3 dimensional space.

The example file defining a 2x2x2 cube can be found in `assets/cube.txt`.

## Sources and futher reading

1. https://open.gl/transformations
2. http://www.songho.ca/opengl/gl_projectionmatrix.html
3. https://www.khronos.org/opengl/wiki/Vertex_Post-Processing
4. http://www.sunshine2k.de/coding/java/SutherlandHodgman/SutherlandHodgman.html
5. https://stackoverflow.com/questions/724219/how-to-convert-a-3d-point-into-2d-perspective-projection
6. http://wazniak.mimuw.edu.pl/index.php?title=GKIW_Modu%C5%82_7_-_Eliminacja_powierzchni_zas%C5%82oni%C4%99tych

## License

MIT License, Copyright (c) 2020 Bartłomiej Zdrojewski

See `LICENSE` file for details.

