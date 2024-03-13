#include "utils.hpp"
#include "model.hpp"

int main(int argc, char **argv)
{

    const int width = 2000;
    const int height = 2000;

    TGAImage image(width, height, TGAImage::RGB);

    std::string filename = "../obj/african_head.obj";

    if (argc >= 2)
    {
        filename = std::string(argv[1]);
    }

    Model model(filename, width, height);

    // model.drawTriangles(image, white);
    model.drawVertices(image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}