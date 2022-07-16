# SpiralTriangle

Artistic project aimed to stylize an input image.

Algorithm:
1. Takes input image and adds points randomly inside it (If it's has a transparent background, points aren't placed in the transparent parts)
2. Runs a delaunator over the points to form non-intersecting triangles between all points
3. Spirals the triangle inwards and colors newly formed lines based on the original image
