#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
#include "delaunator.hpp"


std::vector<Vector2i> imageToPoints(const Image& img, int points = 1000) {
    std::vector<Vector2i> pointArray = std::vector<Vector2i>();
    pointArray.reserve(points);
    
    /*
    Vector2i tl = Vector2i(0, 0);
    Vector2i tr = Vector2i(img.getSize().x - 1, 0);
    Vector2i bl = Vector2i(0, img.getSize().y - 1);
    Vector2i br = Vector2i(img.getSize().x - 1, img.getSize().y - 1);

    pointArray.emplace_back(tl);
    pointArray.emplace_back(tr);
    pointArray.emplace_back(bl);
    pointArray.emplace_back(br);

    points -= 5;
    //*/
    while (points > 0) {
        bool valid = false;
        Vector2i p = Vector2i(rand() % img.getSize().x, rand() % img.getSize().y);;
        while (img.getPixel(p.x, p.y).a == 0) {
            p = Vector2i(rand() % img.getSize().x, rand() % img.getSize().y);
        }
        pointArray.emplace_back(p);
        points--;
    }
    return pointArray;
}

const std::vector<double> getCoords(const std::vector<Vector2i>& particles) {
    std::vector<double> coords;
    coords.reserve(particles.size() * 2);
    for (int i = 0; i < particles.size(); i++) {
        coords.emplace_back(particles[i].x);
        coords.emplace_back(particles[i].y);
    }
    return coords;
}

float distSquared(const Vector2i& p1, const Vector2i& p2) {
    // ((x2-x1)^2 + (y1-y2)^2)^.5
    return ((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y));
}

float distSquared(const Vector2f& p1, const Vector2f& p2) {
    // ((x2-x1)^2 + (y1-y2)^2)^.5
    return ((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y));
}

int main() {
    printf("Enter the image filepath: ");
    std::string imagepath = "C:\\Code\\C++\\SpiralTriangle\\x64\\Debug\\Daedra_heart.png";
    //std::getline(std::cin, imagepath);
    srand(time(NULL));
    Image img;
    img.loadFromFile(imagepath);
    std::vector<Vector2i> points = imageToPoints(img, 200);

    std::vector<double> coords = getCoords(points);

    delaunator::Delaunator d(coords);

    std::vector<Vertex> vertices = std::vector<Vertex>();

    for (int i = 0; i < d.triangles.size(); i += 3) {
        ///*
        Vector2f points[3] = {
            Vector2f(d.coords[2 * d.triangles[i]], d.coords[2 * d.triangles[i] + 1]),
            Vector2f(d.coords[2 * d.triangles[i + 1]], d.coords[2 * d.triangles[i + 1] + 1]),
            Vector2f(d.coords[2 * d.triangles[i + 2]], d.coords[2 * d.triangles[i + 2] + 1])
        };
        ///*
        ///*
        bool flag = true;
        float distance = 150.f;
        // get distance of all lines
        float l1 = distSquared(points[0], points[1]);
        float l2 = distSquared(points[1], points[2]);
        float l3 = distSquared(points[2], points[0]);
        float triangleLargestDist = fmaxf(l1, fmaxf(l2, l3));
        if (triangleLargestDist > distance * distance) {
            flag = false;
        }
        /*
        for (int i = 0; i < 3 && flag; i++) {
            float temp;
            if (i == 2) {
                temp = distSquared(points[i], points[0]);
            }
            else temp = distSquared(points[i], points[i + 1]);

            if (temp < distance) {
                distance *= temp;
            }
            else {
                flag = false;
            }
        }
        //*/
        if (!flag) continue;
        //*/
        vertices.push_back(Vertex(points[0], img.getPixel(points[0].x, points[0].y)));
        vertices.push_back(Vertex(points[1], img.getPixel(points[1].x, points[1].y)));
        vertices.push_back(Vertex(points[1], img.getPixel(points[1].x, points[1].y)));
        vertices.push_back(Vertex(points[2], img.getPixel(points[2].x, points[2].y)));
        vertices.push_back(Vertex(points[2], img.getPixel(points[2].x, points[2].y)));
        vertices.push_back(Vertex(points[0], img.getPixel(points[0].x, points[0].y)));
    }



    RenderWindow window(VideoMode::VideoMode(img.getSize().x, img.getSize().y), "Points", Style::Default, ContextSettings::ContextSettings(0U, 0U, 8U));
    window.setFramerateLimit(0);

    float distance = 50.f;

    Clock clock;
    float deltaTime;
    while (window.isOpen()) {
        deltaTime = clock.restart().asMilliseconds();
        Event e;
        while (window.pollEvent(e)) {
            switch (e.type) {
            case Event::Closed:
                window.close();
                break;
            case Event::KeyPressed:
                switch (e.key.code) {
                    case Keyboard::Right:
                        distance += 1.f;
                        break;
                    case Keyboard::Left:
                        distance -= 1.f;
                        break;
                }
                distance = fmaxf(distance, 1.f);
                break;
            }
        }

        window.clear(Color::White);

        /*
        std::vector<Vertex> vertices = std::vector<Vertex>();
        for (int i = 0; i < points.size(); i++) {
            for (int j = i; j < points.size(); j++) {
                if (i == j) continue;
                // draw line
                if (distSquared(points[i], points[j]) < distance * distance) {
                    Color c1 = img.getPixel(points[i].x, points[i].y);
                    c1.a = 100;
                    Color c2 = img.getPixel(points[j].x, points[j].y);
                    c2.a = 100;
                    

                    vertices.push_back(Vertex((Vector2f)points[i], c1));
                    vertices.push_back(Vertex((Vector2f)points[j], c2));
                }
            }
        }

        window.draw(&vertices[0], vertices.size(), PrimitiveType::Lines);
        */

        

        window.draw(&vertices[0], vertices.size(), PrimitiveType::Lines);

        window.display();
    }
}
