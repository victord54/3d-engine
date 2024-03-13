#include <vector>
#include <cmath>

#include "utils.hpp"
#include "model.hpp"

int main(int argc, char** argv) {

    TGAImage image(width, height, TGAImage::RGB);

    //draw_line(10, 10, 100, 100, image, white);
    Model model("../obj/african_head.obj", width, height);

    model.drawTriangles(image, red);
    model.drawVertices(image, white);
    Point a = Point(147.631, 110.014, 0.000);
    Point b = Point(145.252,109.155, 0.000);
    Point c = Point(144.801, 111.084, 0.000);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}