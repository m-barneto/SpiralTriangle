#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
#include "delaunator.hpp"


std::vector<Vector2i> imageToPoints(const Image& img, const int& points) {
    const int width = img.getSize().x;
    const int height = img.getSize().y;


    // Vector to store points in
    std::vector<Vector2i> pointArray;
    pointArray.reserve(points);

    for (int i = 0; i < points; ++i) {
        // Random point inside image dimensions
        Vector2i randPoint = Vector2i(rand() % width, rand() % height);
        // While point is on a transparent pixel
        while (img.getPixel(randPoint.x, randPoint.y).a == 0) {
            // Assign point to new random point inside image dimensions
            randPoint = Vector2i(rand() % width, rand() % height);
        }
        pointArray.emplace_back(randPoint);
    }

    return pointArray;
}

const std::vector<double> getCoords(const std::vector<Vector2i>& particles) {
    //Vector for storing delaunator compliant positions
    std::vector<double> coords;
    coords.reserve(particles.size() * 2);

    // Unpacking Vector2 into it's two base components (x and y)
    for (int i = 0; i < particles.size(); i++) {
        coords.emplace_back(particles[i].x);
        coords.emplace_back(particles[i].y);
    }
    return coords;
}

inline float distSquared(const Vector2f& p1, const Vector2f& p2) {
    // Distance between two points without the square root (helps performance)
    // ((x2-x1)^2 + (y1-y2)^2)
    return ((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y));
}

// https://www.tutorialspoint.com/program-for-point-of-intersection-of-two-lines-in-cplusplus
inline Vector2f intersection(Vector2f A, Vector2f B, Vector2f C, Vector2f D) {
    // Line AB represented as a1x + b1y = c1
    double a = B.y - A.y;
    double b = A.x - B.x;
    double c = a * (A.x) + b * (A.y);
    // Line CD represented as a2x + b2y = c2
    double a1 = D.y - C.y;
    double b1 = C.x - D.x;
    double c1 = a1 * (C.x) + b1 * (C.y);
    double det = a * b1 - a1 * b;
    double x = (b1 * c - b * c1) / det;
    double y = (a * c1 - a1 * c) / det;
    return Vector2f(x, y);
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline Vector2f lerpPoint(Vector2f a, Vector2f b, float t) {
    return Vector2f(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

int main() {
    printf("Enter the image filepath: \n");
    std::string imagepath = "test_image.png";
    //std::getline(std::cin, imagepath);

    // Set random seed based on time
    time_t timeSeed = time(NULL);
    srand(1658025992);
    //printf("Seed: %d\n", (int)timeSeed);

    // Load image
    Image img;
    img.loadFromFile(imagepath);

    // Get points on image
    std::vector<Vector2i> imgPoints = imageToPoints(img, 75);
    /*std::vector<Vector2i> imgPoints = {
        Vector2i(200, 200),
        Vector2i(100, 350),
        Vector2i(300, 350)
    };*/

    // Convert points to format useable by delaunator
    std::vector<double> coords = getCoords(imgPoints);

    // Create delaunator
    delaunator::Delaunator d(coords);

    // Vector used to store the seed triangle lines
    std::vector<Vertex> vertices;

    // Distance between points allowed before hiding the line
    float distanceSquared = 138.6f;
    distanceSquared *= distanceSquared;

    // Iterate over triangles generated by delaunator
    for (int i = 0; i < d.triangles.size(); i += 3) {
        // Organize points of each triangle into something easier to work with
        Vector2f points[3] = {
            Vector2f(d.coords[2 * d.triangles[i]], d.coords[2 * d.triangles[i] + 1]),
            Vector2f(d.coords[2 * d.triangles[i + 1]], d.coords[2 * d.triangles[i + 1] + 1]),
            Vector2f(d.coords[2 * d.triangles[i + 2]], d.coords[2 * d.triangles[i + 2] + 1])
        };


        // Get distance of all lines
        float line1 = distSquared(points[0], points[1]);
        float line2 = distSquared(points[1], points[2]);
        float line3 = distSquared(points[2], points[0]);

        // Get the largest line
        float triangleLargestDist = fmaxf(line1, fmaxf(line2, line3));

        // If the largest line is over the max distance, skip the entire triangle
        // (If we only skipped the largest line, it would lead to open triangles)
        if (triangleLargestDist > distanceSquared) {
            // TODO - Need to remove this when done testing spiral algorithm
            continue;
        }

        // Line 1
        vertices.push_back(Vertex(points[0], img.getPixel(points[0].x, points[0].y)));
        vertices.push_back(Vertex(points[1], img.getPixel(points[1].x, points[1].y)));

        // Line 2
        vertices.push_back(Vertex(points[1], img.getPixel(points[1].x, points[1].y)));
        vertices.push_back(Vertex(points[2], img.getPixel(points[2].x, points[2].y)));

        // Line 3
        vertices.push_back(Vertex(points[2], img.getPixel(points[2].x, points[2].y)));
        vertices.push_back(Vertex(points[0], img.getPixel(points[0].x, points[0].y)));
    }

    int iterations = 64;
    float t = 1.f / 16.f;

    CircleShape c(50.f);

    // go over each triangle and apply spiral algorithm
    for (int i = 0; i < d.triangles.size(); i += 3) {
        // Organize points of each triangle into something easier to work with
        Vector2f points[3] = {
            Vector2f(d.coords[2 * d.triangles[i]], d.coords[2 * d.triangles[i] + 1]),
            Vector2f(d.coords[2 * d.triangles[i + 1]], d.coords[2 * d.triangles[i + 1] + 1]),
            Vector2f(d.coords[2 * d.triangles[i + 2]], d.coords[2 * d.triangles[i + 2] + 1])
        };

        // Get distance of all lines
        float line1 = distSquared(points[0], points[1]);
        float line2 = distSquared(points[1], points[2]);
        float line3 = distSquared(points[2], points[0]);

        // Get the largest line
        float triangleLargestDist = fmaxf(line1, fmaxf(line2, line3));

        // If the largest line is over the max distance, skip the entire triangle
        // (If we only skipped the largest line, it would lead to open triangles)
        if (triangleLargestDist > distanceSquared) {
            // TODO - Need to remove this when done testing spiral algorithm
            continue;
        }

        for (int j = 0; j < iterations; j++) {
            // Take first line
            Vector2f p1 = points[0];
            // Get point on second line at t
            Vector2f p2 = lerpPoint(points[1], points[2], t);
            // Add line to lines
            vertices.push_back(Vertex(p1, img.getPixel(p1.x, p1.y)));
            vertices.push_back(Vertex(p2, img.getPixel(p2.x, p2.y)));

            // start at p2 and go to midpoint on line 2-3?
            Vector2f p3 = p2;
            Vector2f p4 = lerpPoint(points[2], points[0], t);
            vertices.push_back(Vertex(p3, img.getPixel(p3.x, p3.y)));
            vertices.push_back(Vertex(p4, img.getPixel(p4.x, p4.y)));

            Vector2f p5 = p4;
            Vector2f p6 = lerpPoint(p1, p2, t);
            vertices.push_back(Vertex(p5, img.getPixel(p5.x, p5.y)));
            vertices.push_back(Vertex(p6, img.getPixel(p6.x, p6.y)));

            points[0] = p2;
            points[1] = p4;
            points[2] = p6;
        }
    }

    // Create window with 8x antialiasing
    RenderWindow window(VideoMode::VideoMode(img.getSize().x, img.getSize().y), "Points", Style::Default, ContextSettings::ContextSettings(0U, 0U, 8U));


    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            switch (e.type) {
            case Event::Closed:
                Texture texture;
                texture.create(window.getSize().x, window.getSize().y);
                texture.update(window);
                if (texture.copyToImage().saveToFile("output.png")) {
                    std::cout << "screenshot saved to " << "output.png" << std::endl;
                }
                window.close();
                break;
            }
        }

        window.clear(Color::Transparent);

        window.draw(&vertices[0], vertices.size(), PrimitiveType::Lines);
        //window.draw(c);
        window.display();
    }
}
