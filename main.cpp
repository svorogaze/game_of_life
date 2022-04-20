#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <string>
constexpr int WIN_SIZE_X = 1920;
constexpr int WIN_SIZE_Y = 1080;
int CELL_SIZE = 20;
sf::RenderWindow window(sf::VideoMode(WIN_SIZE_X, WIN_SIZE_Y), "The game of Life");

sf::Time sleep_time = sf::milliseconds(200);

struct Cell {
    bool is_alive = false;
};

std::vector<std::vector<Cell>> board(WIN_SIZE_Y / CELL_SIZE, std::vector<Cell>(WIN_SIZE_X / CELL_SIZE));

void RenderPixel(int x, int y) {
    sf::RectangleShape pixel(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    pixel.setFillColor(sf::Color(0, 255, 0));
    pixel.setPosition(sf::Vector2f(x, y));
    window.draw(pixel);
}

void OnResize() {
    sf::Vector2f size = static_cast<sf::Vector2f>(window.getSize());
    // Apply possible size changes
    window.setSize(static_cast<sf::Vector2u>(size));

    // Reset  GUI view
    auto mGUIView = sf::View(sf::FloatRect(0.f, 0.f, size.x, size.y));
    window.setView(mGUIView);
}

int main() {
    std::string controls_str = "Press Esc to open/close controls\n"
                               "P to pause/unpause the game\n"
                               "Left mouse button to add/delete cell\n"
                               "C to clear screen\n"
                               "Left/Right arrow to increase/decrease speed\n"
                               "Up/Down arrow to increase/decrease cell size\n";
    sf::Vector2i mouse_pos;
    sf::Event event{};
    window.setFramerateLimit(60);
    bool paused = true;
    bool controls = true;
    sf::Font help_font;
    help_font.loadFromFile("font.ttf");
    sf::Text help_text(controls_str, help_font, 24);
    help_text.setFillColor(sf::Color::White);
    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                mouse_pos = sf::Mouse::getPosition();
                board[(mouse_pos.y - window.getPosition().y) / CELL_SIZE][(mouse_pos.x - window.getPosition().x) / CELL_SIZE].is_alive ^= 1;
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                paused ^= 1;
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                sleep_time = sf::milliseconds(std::max(sleep_time.asMilliseconds() - 30, 0));
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
                sleep_time = sf::milliseconds(std::min(sleep_time.asMilliseconds() + 30, 1000));
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
                CELL_SIZE++;
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down) {
                CELL_SIZE--;
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
                for (int i = 0; i < board.size(); ++i) {
                    for (int j = 0; j < board[i].size(); ++j) {
                        board[i][j].is_alive = false;
                    }
                }
            }

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                controls ^= 1;
            }

            else if (event.type == sf::Event::Resized) {
                OnResize();
            }
        }

        window.clear();

        for (int i = 0; i < WIN_SIZE_Y; i += CELL_SIZE) {
            sf::RectangleShape line(sf::Vector2f(WIN_SIZE_X, 1));
            line.setPosition(0, i);
            line.setFillColor(sf::Color(64, 64, 64));
            window.draw(line);
        }
        for (int i = 0; i < WIN_SIZE_X; i += CELL_SIZE) {
            sf::RectangleShape line(sf::Vector2f(WIN_SIZE_X, 1));
            line.setPosition(i, 0);
            line.rotate(90);
            line.setFillColor(sf::Color(64, 64, 64));
            window.draw(line);
        }

        std::vector<std::vector<Cell>> new_board((window.getSize().y + CELL_SIZE - 1) / CELL_SIZE, std::vector<Cell>((window.getSize().x + CELL_SIZE - 1) / CELL_SIZE));
        if (paused) {
            for (int i = 0; i < std::min(board.size(), new_board.size()); ++i) {
                for (int j = 0; j < std::min(new_board[i].size(), board[i].size()); ++j) {
                    new_board[i][j] = board[i][j];
                }
            }
        }
        for (int i = 0; i < std::min(board.size(), new_board.size()); ++i) {
            for (int j = 0; j < std::min(board[i].size(), new_board[i].size()); ++j) {
                if (!paused) {
                    int count_alive = 0;
                    for (int k = -1; k <= 1; ++k) {
                        for (int l = -1; l <= 1; ++l) {
                            if ((l != 0 || k != 0) &&
                            board[(i + k + board.size()) % board.size()][(j + l + board[0].size()) % board[0].size()].is_alive) {
                                count_alive++;
                            }
                        }
                    }
                    if (count_alive == 3 || (count_alive == 2 && board[i][j].is_alive)) {
                        new_board[i][j].is_alive = true;
                    }
                    else {
                        new_board[i][j].is_alive = false;
                    }
                }
            }
        }

        board = new_board;
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[i].size(); ++j) {
                if (board[i][j].is_alive) {
                    RenderPixel(j * CELL_SIZE, i * CELL_SIZE);
                }
            }
        }
        if (controls) {
            window.draw(help_text);
        }
        window.display();
        if (!paused) {
        sf::sleep(sleep_time);
        }
    }
    return 0;
}
