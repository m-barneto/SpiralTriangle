#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>


std::vector<Vector2i> imageToPoints(const Image& img, int points = 1000) {
    std::vector<Vector2i> pointArray = std::vector<Vector2i>();
    pointArray.reserve(points);

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

float distSquared(const Vector2i& p1, const Vector2i& p2) {
    // ((x2-x1)^2 + (y1-y2)^2)^.5
    return ((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y));
}

int main() {
    printf("Enter the image filepath: ");
    std::string imagepath = "C:\\Code\\C++\\SpiralTriangle\\x64\\Debug\\Daedra_heart.png";
    //std::getline(std::cin, imagepath);

    Image img;
    img.loadFromFile(imagepath);
    std::vector<Vector2i> points = imageToPoints(img);


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

        window.display();
    }
}
