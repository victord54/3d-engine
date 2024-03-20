#include "utils.hpp"
#include "model.hpp"

int main(int argc, char **argv)
{

    const int width = 1000;
    const int height = 1000;

    TGAImage image(width, height, TGAImage::RGB);

    std::string filename = "../obj/african_head/african_head.obj";

    if (argc >= 2)
    {
        filename = std::string(argv[1]);
    }

    Model model(filename, width, height);

    // model.drawTriangles(image, white);
    // model.drawVertices(image, green);
    // model.fillTriangles(image);
    // model.fillTrianglesLerp(image);
    model.fillTrianglesWithBackFaceCulling(image);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    // Create out folder if it doesn't exist
    std::filesystem::create_directory("out");
    image.write_tga_file("out/output.tga");
    return 0;
}