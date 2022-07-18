#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class ScaledImage {
private:
    Image img;
public:
    int width, height;
    float scale;
    ScaledImage(Image img, float scale) {
        this->img = img;
        this->width = img.getSize().x * scale;
        this->height = img.getSize().y * scale;
        this->scale = scale;
    }
    Color getPixel(Vector2f p) {
        return img.getPixel(p.x / scale, p.y / scale);
    }
    Color getPixel(int x, int y) {
        return img.getPixel(x / scale, y / scale);
    }
    Color getAvgColor(Vector2f p1, Vector2f p2) {
        Color c1 = getPixel(p1);
        Color c2 = getPixel(p2);
        return Color((c1.r + c2.r) / 2, (c1.g + c2.g) / 2, (c1.b + c2.b) / 2, (c1.a + c2.a) / 2);
    }
};