#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
const int POINTS_X = 30;
const int POINTS_Y = 20;
const float SPACING = 15.0f;
const sf::Vector2f GRAVITY(0.0f, 0.5f);
const float DAMPING = 0.99f;

struct Point
{
    sf::Vector2f position;
    sf::Vector2f oldPosition;
    bool locked;

    Point(float x, float y) : position(x, y), oldPosition(x, y), locked(false) {}
};

struct Stick
{
    Point *p0;
    Point *p1;
    float length;
    Stick(Point *p0, Point *p1) : p0(p0), p1(p1)
    {
        length = sqrtf((p1->position.x - p0->position.x) * (p1->position.x - p0->position.x) +
                       (p1->position.y - p0->position.y) * (p1->position.y - p0->position.y));
    }
};

void drawPoint(sf::RenderWindow &window, const vector<Point> &points)
{
    for (const auto &point : points)
    {
        sf::CircleShape shape(3);
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(point.position.x - 3, point.position.y - 3);
        window.draw(shape);
    }
}

void drawStick(sf::RenderWindow &window, const vector<Stick> &sticks)
{
    for (const auto &stick : sticks)
    {
        sf::Vertex line[] = {
            sf::Vertex(stick.p0->position, sf::Color::White),
            sf::Vertex(stick.p1->position, sf::Color::White)};
        window.draw(line, 2, sf::Lines);
    }
}

void constrainPoints(vector<Stick> &sticks)
{
    for (auto &stick : sticks)
    {
        sf::Vector2f delta = stick.p1->position - stick.p0->position;
        float deltaLength = sqrtf(delta.x * delta.x + delta.y * delta.y);
        float diff = (deltaLength - stick.length) / deltaLength * 0.5f;
        sf::Vector2f offset = delta * diff;

        if (!stick.p0->locked)
            stick.p0->position += offset;
        if (!stick.p1->locked)
            stick.p1->position -= offset;
    }
}

void updatePoints(vector<Point> &points)
{
    for (auto &point : points)
    {
        if (!point.locked)
        {
            sf::Vector2f velocity = point.position - point.oldPosition;
            point.oldPosition = point.position;
            point.position += velocity * DAMPING;
            point.position += GRAVITY;

            if (point.position.y > HEIGHT)
                point.position.y = HEIGHT;
        }
    }
}

void initializeCloth(vector<Point> &points, vector<Stick> &sticks)
{
    points.clear();
    sticks.clear();

    for (int y = 0; y < POINTS_Y; ++y)
    {
        for (int x = 0; x < POINTS_X; ++x)
        {
            points.emplace_back(x * SPACING + (WIDTH - POINTS_X * SPACING) / 2, y * SPACING);
        }
    }

    for (int y = 0; y < POINTS_Y; ++y)
    {
        for (int x = 0; x < POINTS_X; ++x)
        {
            if (x < POINTS_X - 1)
            {
                sticks.emplace_back(&points[x + y * POINTS_X], &points[x + y * POINTS_X + 1]);
            }
            if (y < POINTS_Y - 1)
            {
                sticks.emplace_back(&points[x + y * POINTS_X], &points[x + (y + 1) * POINTS_X]);
            }
        }
    }

    for (int i = 0; i < POINTS_X; ++i)
    {
        if (i % 2 == 0)
        {
            points[i].locked = true;
        }
    }
}

int main(int argc, char *argv[])
{
    vector<Point> points;
    vector<Stick> sticks;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Simulating cloth with Meo");

    // Tải ảnh nền
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("photo.jpg"))
    {
        return -1; // Xử lý lỗi nếu không tải được ảnh
    }

    // Tạo một sprite từ texture
    sf::Sprite backgroundSprite(backgroundTexture);

    // Kích thước của ảnh nền cần được điều chỉnh để phủ kín toàn bộ cửa sổ
    backgroundSprite.setScale(
        static_cast<float>(WIDTH) / backgroundTexture.getSize().x + 0.2,
        static_cast<float>(HEIGHT) / backgroundTexture.getSize().y);

    // Đặt độ trong suốt của ảnh nền
    sf::Color transparentColor(255, 255, 255, 128); // Độ trong suốt 50%
    backgroundSprite.setColor(transparentColor);

    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("RussoOne-Regular.ttf"))
    {
        return -1;
    }

    sf::RectangleShape resetButton(sf::Vector2f(100, 50));
    sf::Color customColor(165, 225, 19);
    resetButton.setFillColor(customColor);
    resetButton.setPosition(WIDTH - 110, 10);

    sf::Text buttonText("Reset", font, 20);
    buttonText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = buttonText.getLocalBounds();
    sf::FloatRect rectBounds = resetButton.getGlobalBounds();

    buttonText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    buttonText.setPosition(
        rectBounds.left + rectBounds.width / 2,
        rectBounds.top - 5 + rectBounds.height / 2);

    // Tải và phát nhạc nền
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("music.ogg"))
    {
        return -1; // Xử lý lỗi nếu không tải được nhạc
    }
    backgroundMusic.setLoop(true);
    backgroundMusic.play();

    initializeCloth(points, sticks);

    sf::Event event;
    bool dragging = false;
    Point *selectedPoint = nullptr;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if (resetButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        initializeCloth(points, sticks);
                    }
                    else
                    {
                        for (auto &point : points)
                        {
                            if (!point.locked)
                            {
                                if (sqrtf(pow(event.mouseButton.x - point.position.x, 2) +
                                          pow(event.mouseButton.y - point.position.y, 2)) < 10.0f)
                                {
                                    dragging = true;
                                    selectedPoint = &point;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    for (auto i = sticks.begin(); i != sticks.end();)
                    {
                        if (sqrtf(pow(event.mouseButton.x - i->p0->position.x, 2) +
                                  pow(event.mouseButton.y - i->p0->position.y, 2)) < 10.0f ||
                            sqrtf(pow(event.mouseButton.x - i->p1->position.x, 2) +
                                  pow(event.mouseButton.y - i->p1->position.y, 2)) < 10.0f)
                        {
                            i = sticks.erase(i);
                        }
                        else
                        {
                            i++;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    dragging = false;
                    selectedPoint = nullptr;
                }
            }

            if (event.type == sf::Event::MouseMoved)
            {
                if (dragging && selectedPoint)
                {
                    selectedPoint->position = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                }
            }
        }

        updatePoints(points);

        for (int i = 0; i < 5; ++i)
        {
            constrainPoints(sticks);
        }

        window.clear(sf::Color::Black);

        // Vẽ ảnh nền
        window.draw(backgroundSprite);

        drawPoint(window, points);
        drawStick(window, sticks);

        window.draw(resetButton);
        window.draw(buttonText);

        window.display();
    }

    return 0;
}
