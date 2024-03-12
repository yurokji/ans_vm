#include <SFML/Graphics.hpp>
#include <vector>

const int DISPLAY_WIDTH = 512;
const int DISPLAY_HEIGHT = 512;
const int PIXEL_SIZE = 1;

class VirtualDisplay {
public:
    VirtualDisplay(int width, int height) : width(width), height(height) {
        pixelData.resize(width * height);
    }

    void setPixel(int x, int y, sf::Color color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixelData[y * width + x] = color;
        }
    }

    sf::Color getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return pixelData[y * width + x];
        }
        return sf::Color::Black;
    }

    void set(sf::RenderWindow& window) {
        sf::VertexArray vertices(sf::Points, width * height);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                vertices[y * width + x].position = sf::Vector2f(x * PIXEL_SIZE, y * PIXEL_SIZE);
                vertices[y * width + x].color = pixelData[y * width + x];
            }
        }
        window.draw(vertices);
    }
    void setImage(sf::Image& image, int destX, int destY, int srcX, int srcY, int width, int height) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                sf::Color color = image.getPixel(srcX + x, srcY + y);
                setPixel(destX + x, destY + y, color);
            }
        }
    }
    const std::vector<sf::Color>& getPixelData() const {
        return pixelData;
    }

    void init(int width, int height) {
        pixelData.resize(width * height);
    }
    



private:
    int width;
    int height;
    std::vector<sf::Color> pixelData;
};



// int main() {
//     sf::RenderWindow window(sf::VideoMode(DISPLAY_WIDTH * PIXEL_SIZE, DISPLAY_HEIGHT * PIXEL_SIZE), "Virtual Display");

//     VirtualDisplay display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

//     sf::Image image;
//     if (!image.loadFromFile("ascii_texture.jpg")) {
//         // 이미지를 로드하는데 실패했을 경우 처리
//         return 1;
//     }

//     // 이미지의 특정 50x50 픽셀 영역을 화면에 그립니다.
//     display.setImage(image, 100, 100, 149, 149, 50, 50);

//     while (window.isOpen()) {
//         sf::Event event;
//         while (window.pollEvent(event)) {
//             if (event.type == sf::Event::Closed) {
//                 window.close();
//             }
//         }

//         window.clear();
//         display.draw(window);
//         window.display();
//     }

//     return 0;
// }