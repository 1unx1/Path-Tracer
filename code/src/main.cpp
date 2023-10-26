#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "render.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 5)
    {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file> <samples> <pt depth>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2]; // only bmp is allowed.
    int samples = std::atoi(argv[3]);
    int pt_depth = std::atoi(argv[4]);

    // Main RayCasting Logic
    // First, parse the scene using SceneParser.
    SceneParser parser(inputFile.c_str());
    // Camera *camera = parser.getCamera();
    // Image img(camera->getWidth(), camera->getHeight());
    // // Then loop over each pixel in the image, shooting a ray
    // for (int x = 0; x < camera->getWidth(); x++)
    // {
    //     for (int y = 0; y < camera->getHeight(); y++)
    //     {
    //         Ray r = camera->generateRay(Vector2f(x, y));
    //         // through that pixel and finding its intersection with
    //         // the scene.  Write the color at the intersection to that
    //         // pixel in your output image.
    //         Group *baseGroup = parser.getGroup();
    //         Hit h;
    //         bool isIntersect = baseGroup->intersect(r, h, 0);
    //         if (isIntersect)
    //         {
    //             Vector3f finalColor = Vector3f::ZERO;
    //             for (int i = 0; i < parser.getNumLights(); i++)
    //             {
    //                 Light *light = parser.getLight(i);
    //                 Vector3f dirToLight, lightColor;
    //                 light->getIllumination(r.pointAtParameter(h.getT()), dirToLight, lightColor);
    //                 finalColor += h.getMaterial()->Shade(r, h, dirToLight, lightColor);
    //             }
    //             img.SetPixel(x, y, finalColor);
    //         }
    //         else
    //             img.SetPixel(x, y, parser.getBackgroundColor());
    //     }
    // }
    // img.SaveImage(outputFile.c_str());
    PathTracer pt(parser, samples, pt_depth, outputFile.c_str());
    pt.render();
    return 0;
}
