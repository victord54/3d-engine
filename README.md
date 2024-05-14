# 3D engine

This is a 3D engine that I am developing in C++ using only standard libraries.
The engine is based on the [Github repository](https://github.com/ssloy/tinyrenderer) of Dmitry V. Sokolov.

This project takes place in the context of the course "How a 3D rendering engine works" of the University of Lorraine for the Master 1 in Computer Science.

## How to use

To compile and execute the project, you can use the following command:

```sh
cmake -B build
make -C build
./build/engine [degree]
```

## Evolution of the project

To render this image, I had to implement the following features:
-   Bresenham's line algorithm
-   World Matrix
-   Camera Matrix
-   Projection Matrix
-   Perspective Projection

![African head with no backface culling](gif/african_head_no-backface_culling.gif)

Then I implemented the backface culling algorithm to render only the visible faces of the object.

![African head with backface culling](gif/african_head_backface_culling.gif)

I implemented the filling triangle algorithm to render the faces of the object.
After that, I implemented the z-buffer algorithm to render the faces of the object with the correct depth and also the Gouraud shading algorithm to render the faces with the correct color.

![African head with Gouraud shading](gif/african_head_gouraud_shading_2.gif)

I added the texture mapping algorithm to render the faces of the object with a texture.
You can see the result below.

![African head with texture mapping](gif/african_head_texture_mapping_2.gif)

And now with the Gouraud shading algorithm and the texture mapping algorithm.

![African head with Gouraud shading and texture mapping](gif/african_head_gouraud_shading_texture_mapping_2.gif)

The final addition to the project was the implementation of the normal mapping and specular mapping algorithms to render the faces of the object with a normal map and a specular map.

![African head with normal mapping and specular mapping](gif/african_head_final.gif)
