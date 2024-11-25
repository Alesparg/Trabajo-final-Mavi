#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

class GameStartScreen {
public:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText, instructionsText, exitText;
    sf::Texture salonTexture, miraTexture, civilTexture, enemyTexture;
    sf::Sprite salonSprite, miraSprite;
    bool showSalon = false;
    int score = 0;
    int lives = 3;
    int enemiesDefeated = 0;
    bool gameOver = false;
    sf::Text scoreText, livesText, endMessage;

    struct Entity {
        sf::Sprite sprite;
        sf::Clock lifetimeClock;
        sf::Vector2f position;
        bool hasShot = false;
    };

    std::vector<Entity> civiles;
    std::vector<Entity> enemies;
    std::vector<sf::Vector2f> spawnPositions;
    sf::Clock spawnClock;
    float spawnInterval = 2.0f;
    int maxCiviles = 2;
    int maxEnemies = 1;

    GameStartScreen() : window(sf::VideoMode(1150, 1200), "Pantalla de Inicio") {
        // Cargar fuentes
        if (!font.loadFromFile("C:\\Users\\Equipo\\Desktop\\Tipografia\\Manrope-VariableFont_wght.ttf")) {
            std::cerr << "Error al cargar la fuente." << std::endl;
            window.close();
        }

        // Configurar texto inicial
        titleText.setFont(font);
        titleText.setString("El pistolero ojo de oro");
        titleText.setCharacterSize(58);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setPosition(300, 50);

        instructionsText.setFont(font);
        instructionsText.setString(
            "Instrucciones:\n"
            "Se apunta y se dispara con el puntero del raton\n"
            "Cada disparo acertado a un enemigo suma un punto\n"
            "Cada disparo acertado a un civil resta una vida y un punto\n"
            "Si el enemigo dispara, resta una vida\n\n"
            "Como se gana o se pierde:\n"
            "Si se matan 10 enemigos, se gana\n"
            "Si se pierden las tres vidas, termina la partida\n"
            "Presiona Enter para salir"
        );
        instructionsText.setCharacterSize(24);
        instructionsText.setFillColor(sf::Color::White);
        instructionsText.setPosition(50, 150);

        exitText.setFont(font);
        exitText.setString("Presiona cualquier tecla para comenzar");
        exitText.setCharacterSize(30);
        exitText.setFillColor(sf::Color::Green);
        exitText.setPosition(250, 900);

        // Cargar texturas
        if (!salonTexture.loadFromFile("C:\\Users\\Equipo\\Desktop\\imagen trabajo\\salon.png")) {
            std::cerr << "Error al cargar la imagen del salon." << std::endl;
            window.close();
        }
        salonSprite.setTexture(salonTexture);
        salonSprite.setPosition(0, 0);
        salonSprite.setScale(
            static_cast<float>(window.getSize().x) / salonTexture.getSize().x,
            static_cast<float>(window.getSize().y) / salonTexture.getSize().y
        );

        if (!miraTexture.loadFromFile("C:\\Users\\Equipo\\Desktop\\imagen trabajo\\crosshair.png")) {
            std::cerr << "Error al cargar la imagen de la mira." << std::endl;
            window.close();
        }
        miraSprite.setTexture(miraTexture);
        miraSprite.setOrigin(miraTexture.getSize().x / 2, miraTexture.getSize().y / 2);
        window.setMouseCursorVisible(false);

        if (!civilTexture.loadFromFile("C:\\Users\\Equipo\\Desktop\\imagen trabajo\\civil.jpg")) {
            std::cerr << "Error al cargar la imagen del civil." << std::endl;
            window.close();
        }

        if (!enemyTexture.loadFromFile("C:\\Users\\Equipo\\Desktop\\imagen trabajo\\malo.png")) {
            std::cerr << "Error al cargar la imagen del enemigo." << std::endl;
            window.close();
        }

        // Configurar textos del juego
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Red);
        scoreText.setPosition(25, 25);

        livesText.setFont(font);
        livesText.setCharacterSize(24);
        livesText.setFillColor(sf::Color::Green);
        livesText.setPosition(window.getSize().x - 200, 25);

        endMessage.setFont(font);
        endMessage.setCharacterSize(40);
        endMessage.setFillColor(sf::Color::White);
        endMessage.setPosition(window.getSize().x / 4, window.getSize().y / 2);

        spawnPositions = {
            {225, 330}, {800, 330}, {205, 750}, {855, 750}, {530, 825}
        };

        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            if (!gameOver) {
                update();
            }
            render();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (!showSalon) {
                    // Si se presiona Enter y estamos en el menú inicial, cerrar el juego.
                    if (event.key.code == sf::Keyboard::Enter) {
                        window.close();
                    }
                    else {
                        showSalon = true;
                    }
                }
                else if (gameOver || event.key.code == sf::Keyboard::Enter) {
                    window.close();
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (showSalon && !gameOver) {
                    checkShoot();
                }
            }
        }
    }


    void update() {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        miraSprite.setPosition(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

        if (showSalon && spawnClock.getElapsedTime().asSeconds() >= spawnInterval) {
            if (civiles.size() < maxCiviles) spawnCivil();
            if (enemies.size() < maxEnemies) spawnEnemy();
            spawnClock.restart();
        }

        updateEnemies();
        removeExpiredEntities();

        scoreText.setString("Puntos: " + std::to_string(score));
        livesText.setString("Vidas: " + std::to_string(lives));

        if (enemiesDefeated >= 10) {
            endMessage.setString("¡Has ganado!\nPuntos finales: " + std::to_string(score) +
                "\nVidas restantes: " + std::to_string(lives) + "\nPresiona cualquier tecla para salir");
            gameOver = true;
        }
    }

    void updateEnemies() {
        for (auto& enemy : enemies) {
            if (enemy.lifetimeClock.getElapsedTime().asSeconds() >= 1.0f && !enemy.hasShot) {
                loseLife();
                enemy.hasShot = true;
            }
        }
    }

    void checkShoot() {
        sf::Vector2f miraPosition = miraSprite.getPosition();

        for (auto it = enemies.begin(); it != enemies.end();) {
            if (it->sprite.getGlobalBounds().contains(miraPosition)) {
                score++;
                enemiesDefeated++;
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }

        for (auto it = civiles.begin(); it != civiles.end();) {
            if (it->sprite.getGlobalBounds().contains(miraPosition)) {
                score -= 2;
                lives--;
                it = civiles.erase(it);
            }
            else {
                ++it;
            }
        }

        if (lives <= 0) {
            loseLife();
        }
    }

    void loseLife() {
        if (lives > 0) {
            lives--;
        }
        if (lives == 0) {
            endMessage.setString("Game Over\nPuntos finales: " + std::to_string(score) +
                "\nVidas restantes: " + std::to_string(lives) + "\nPresiona cualquier tecla para salir");
            gameOver = true;
        }
    }

    void spawnCivil() {
        Entity civil;
        civil.sprite.setTexture(civilTexture);
        civil.sprite.setScale(0.5f, 0.5f);
        civil.position = getAvailablePosition();
        if (civil.position != sf::Vector2f(-1, -1)) {
            civil.sprite.setPosition(civil.position);
            civil.lifetimeClock.restart();
            civiles.push_back(civil);
        }
    }

    void spawnEnemy() {
        Entity enemy;
        enemy.sprite.setTexture(enemyTexture);
        enemy.sprite.setScale(0.3f, 0.3f);
        enemy.position = getAvailablePosition();
        if (enemy.position != sf::Vector2f(-1, -1)) {
            enemy.sprite.setPosition(enemy.position);
            enemy.lifetimeClock.restart();
            enemies.push_back(enemy);
        }
    }

    sf::Vector2f getAvailablePosition() {
        int tries = 0;
        while (tries < 10) {
            sf::Vector2f position = spawnPositions[std::rand() % spawnPositions.size()];
            bool isOccupied = false;
            for (const auto& civil : civiles) {
                if (civil.sprite.getPosition() == position) {
                    isOccupied = true;
                    break;
                }
            }
            for (const auto& enemy : enemies) {
                if (enemy.sprite.getPosition() == position) {
                    isOccupied = true;
                    break;
                }
            }
            if (!isOccupied) {
                return position;
            }
            tries++;
        }
        return sf::Vector2f(-1, -1);
    }

    void removeExpiredEntities() {
        civiles.erase(std::remove_if(civiles.begin(), civiles.end(),
            [](const Entity& civil) { return civil.lifetimeClock.getElapsedTime().asSeconds() >= 5.0f; }),
            civiles.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Entity& enemy) { return enemy.lifetimeClock.getElapsedTime().asSeconds() >= 7.0f; }),
            enemies.end());
    }

    void render() {
        window.clear();
        if (!showSalon) {
            window.draw(titleText);
            window.draw(instructionsText);
            window.draw(exitText);
        }
        else {
            window.draw(salonSprite);
            if (!gameOver) {
                for (const auto& civil : civiles) window.draw(civil.sprite);
                for (const auto& enemy : enemies) window.draw(enemy.sprite);
                window.draw(scoreText);
                window.draw(livesText);
            }
            else {
                window.draw(endMessage);
            }
            window.draw(miraSprite);
        }
        window.display();
    }
};

int main() {
    GameStartScreen game;
    game.run();
    return 0;
}













