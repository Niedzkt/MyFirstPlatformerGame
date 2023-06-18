/// Definiuje sta³¹, aby wy³¹czyæ ostrze¿enia zwi¹zane z funkcjami, które Microsoft uwa¿a za niebezpieczne
#define _CRT_SECURE_NO_WARNINGS

/// Standardowa biblioteka C++ do obs³ugi strumieni danych
#include <iostream>

/// Biblioteka matematyczna w C++, zawiera funkcje takie jak sin, cos, sqrt, itp.
#include <cmath>

/// Kontener sekwencyjny przechowuj¹cy elementy
#include <vector>

/// Obs³uga ³añcuchów znaków w C++
#include <string>

/// Wsparcie dla zarz¹dzania dynamiczn¹ pamiêci¹ za pomoc¹ wskaŸników inteligentnych
#include <memory>

/// Pliki nag³ówkowe biblioteki SFML do obs³ugi grafiki 2D
#include <SFML/Graphics.hpp>

/// Pliki nag³ówkowe biblioteki SFML do obs³ugi dŸwiêku
#include <SFML/Audio.hpp>

/// Pliki nag³ówkowe biblioteki SFML do obs³ugi okien
#include <SFML/Window.hpp>

/// Pliki nag³ówkowe biblioteki SFML do obs³ugi systemu
#include <SFML/System.hpp>

/// Dodatkowe pliki nag³ówkowe biblioteki SFML do obs³ugi zdarzeñ okna
#include <SFML/Window/Event.hpp>

/// Dodatkowe pliki nag³ówkowe biblioteki SFML do obs³ugi renderowania okna
#include <SFML/Graphics/RenderWindow.hpp>

/// Dodatkowe pliki nag³ówkowe biblioteki SFML do obs³ugi dŸwiêku i muzyki
#include <SFML/Audio.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

/// Pliki nag³ówkowe biblioteki tmxlite, która umo¿liwia obs³ugê plików map w formacie TMX (Tiled Map XML)
/// u¿ywanych do tworzenia map kafelkowych w grach 2D
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/LayerGroup.hpp>
#include <tmxlite/ObjectGroup.hpp>

using namespace std;

/// Klasa reprezentuj¹ca pocisk w grze.
class Projectile {
public:
    /// Konstruktor, który tworzy nowy pocisk w okreœlonym miejscu, kieruje go w okreœlonym kierunku i wczytuje teksturê pocisku z pliku.
    Projectile(const sf::Vector2f& startPosition, int direction, const std::string& textureFile) {
        /// Wczytanie tekstury pocisku z pliku
        if (!texture.loadFromFile("graphics/bullet_texture.png")) {
            /// Je¿eli tekstura nie zosta³a poprawnie wczytana, wypisuje b³¹d
            cout << "ERROR::PROJECTILE::BRAK TEKSTURY" << "\n";
        }

        /// Ustawienie tekstury na sprite pocisku
        sprite.setTexture(texture);
        /// Ustawienie pocz¹tkowej pozycji pocisku
        sprite.setPosition(startPosition);
        /// Skalowanie pocisku
        sprite.setScale(0.1f, 0.05f);

        /// Ustawienie kierunku ruchu pocisku. Kierunek jest reprezentowany jako liczba ca³kowita (1 dla prawej strony, -1 dla lewej)
        velocity.x = (direction > 0) ? speed : -speed;
    }

    /// Operator przypisania, pozwalaj¹cy na przypisanie jednego pocisku do drugiego.
    Projectile& operator=(const Projectile& other) {
        if (this != &other) {  /// Sprawdzanie, czy nie jest to przypisanie do siebie samego
            /// Przypisywanie atrybutów pocisku
            texture = other.texture;
            sprite.setTexture(texture);
            sprite.setPosition(other.sprite.getPosition());
            sprite.setScale(other.sprite.getScale());
            velocity = other.velocity;
        }
        return *this;  /// Zwraca referencjê do obiektu, który jest obecnie przypisywany, umo¿liwiaj¹c przypisywanie ³añcuchowe
    }

    /// Aktualizuje pozycjê pocisku na podstawie jego prêdkoœci i up³ywaj¹cego czasu.
    void update(sf::Time deltaTime) {
        float deltaSeconds = deltaTime.asSeconds();  /// Konwersja czasu na sekundy
        sprite.move(velocity * deltaSeconds);  /// Aktualizacja pozycji pocisku
    }

    /// Rysuje pocisk na ekranie.
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);  /// Rysowanie sprite pocisku na ekranie
    }

    /// Getter do sprite pocisku.
    const sf::Sprite& getSprite() const {
        return sprite;  /// Zwraca sprite pocisku
    }

    /// Sprawdza, czy pocisk jest poza granicami okna.
    bool outOfBounds(const sf::Vector2u windowSize) const {
        sf::Vector2f position = sprite.getPosition();  /// Pobiera pozycjê pocisku
        /// Sprawdza, czy pocisk jest poza granicami okna
        return position.x < 0 || position.y < 0 || position.x > windowSize.x || position.y > windowSize.y;
    }

private:
    sf::Sprite sprite;  /// Sprite reprezentuj¹cy pocisk na ekranie
    sf::Texture texture;  /// Tekstura sprite pocisku
    sf::Vector2f velocity;  /// Wektor prêdkoœci pocisku
    const float speed = 990.0f;  /// Prêdkoœæ pocisku w jednostkach na sekundê
};

///Klasa reprezentuj¹ca postaæ gracza w grze.
class Player
{
    int lastDirection = 1; /// Zmienna przechowuj¹ca ostatni kierunek gracza, 1 oznacza prawo, -1 oznacza lewo
    vector<Projectile> projectiles; /// Wektor przechowuj¹cy pociski wystrzelone przez gracza
    sf::Clock clock; /// Zegar, który jest u¿ywany do obliczania czasu miêdzy klatkami (deltaTime)
    sf::Clock shootTimer; /// Zegar u¿ywany do kontrolowania prêdkoœci strzelania gracza
    sf::Music shootSound; /// Muzyka odtwarzana, gdy gracz strzela
    const float shootDelay = 0.25f; /// Sta³a oznaczaj¹ca opóŸnienie (w sekundach) miêdzy strza³ami gracza

public:
    /// Konstruktory
    Player() /// Konstruktor bezparametrowy klasy Player
    {
        initVariables(); /// Inicjalizuje zmienne zwi¹zane z graczem
        initTexture(); /// Inicjalizuje teksturê gracza
        initSprite(); /// Inicjalizuje sprite gracza
        initAnimations(); /// Inicjalizuje animacje gracza
        initPhysics(); /// Inicjalizuje fizykê gracza
    }

    /// Destruktor
    ~Player() /// Destruktor klasy Player
    {
    }

    /// Akcesory (gettery)

    /// Zwraca sprite gracza
    const sf::Sprite& getSprite() const
    {
        return sprite;
    }

    /// Zwraca wektor pocisków gracza
    const std::vector<Projectile>& getProjectiles() const {
        return projectiles;
    }

    /// Zmiana animacji
    bool getAnimSwitch()
    {
        /// Jeœli animacja jest w³¹czona, wy³¹cza j¹ i zwraca prawdê.
        /// Jeœli animacja jest wy³¹czona, zwraca fa³sz.
        if (animationSwitch)
        {
            animationSwitch = false;
            return true;
        }

        return false;
    }

    /// Zwraca pozycjê gracza
    sf::Vector2f getPosition() const
    {
        return sprite.getPosition();
    }

    /// Granice gracza
    const sf::FloatRect getGlobalBounds() const /// Zwraca granice globalne sprite'u gracza
    {
        return sprite.getGlobalBounds();
    }

    /// Modyfikatory

    /// Ustawienie pozycji gracza
    void setPosition(const float x, const float y) /// Ustawia pozycjê sprite'u gracza
    {
        sprite.setPosition(x, y);
    }

    /// Reset prêdkoœci w osi Y
    void resetVelocityY() /// Resetuje prêdkoœæ gracza w osi Y do 0
    {
        velocity.y = 0.f;
    }

    /// Funkcje publiczne

    /* Sekcja Update */

    /// Resetowanie czasu animacji
    void resetAnimationTimer() /// Resetuje czas animacji i ustawia prze³¹cznik animacji na true
    {
        animationTimer.restart();
        animationSwitch = true;
    }

    /// Poruszanie siê gracza
    void move(const float dir_x, const float dir_y) /// Zmienia prêdkoœæ gracza w zale¿noœci od kierunku i ogranicza j¹ do maksymalnej prêdkoœci
    {
        /// Przyspieszenie
        velocity.x += acceleration * dir_x;

        /// Limit prêdkoœci
        if (std::abs(velocity.x) > maxVelocity)
        {
            velocity.x = maxVelocity * ((velocity.x < 0.f) ? -1.f : 1.f);
        }
    }

    void jump() /// Wywo³uje skok gracza, jeœli gracz nie skacze
    {
        if (!isJumping)
        {
            velocity.y = -sqrt(2.0f * gravity * jumpHeight);
            isJumping = true;
        }
    }

    void resetJump() /// Resetuje flagê skoku
    {
        isJumping = false;
    }

    /// Aktualizacja fizyki gracza
    void updatePhysics() /// Aktualizuje prêdkoœæ gracza zgodnie z grawitacj¹ i tarcie, a nastêpnie porusza sprite gracza
    {
        /// Grawitacja
        ///velocity.y += gravity * 1.0;

        velocity.y += gravity;
        /// Limit grawitacji
        if (std::abs(velocity.x) > maxVelocityY)
        {
            velocity.y = maxVelocityY * ((velocity.y < 0.f) ? -1.f : 1.f);
        }

        /// Zmniejszenie prêdkoœci
        velocity *= drag;

        /// Limit prêdkoœci
        if (std::abs(velocity.x) < minVelocity)
            velocity.x = 0.f;
        if (std::abs(velocity.y) < minVelocity)
            velocity.y = 0.f;

        sprite.move(velocity);
    }

    /// Animacje poruszania siê gracza
    void updateAnimations() /// Aktualizuje animacje gracza na podstawie aktualnego stanu animacji
    {
        if (animationState == PLAYER_ANIMATION_STATES::IDLE) /// Animacja stania
        {
            if (animationTimer.getElapsedTime().asSeconds() >= 0.5f || getAnimSwitch()) /// Czas animacji
            {
                currentFrame.top = 0.f;
                currentFrame.left += 25.f;
                if (currentFrame.left >= 100.f)
                    currentFrame.left = 0;

                animationTimer.restart();
                sprite.setTextureRect(currentFrame);
            }
        }
        else if (animationState == PLAYER_ANIMATION_STATES::MOVING_RIGHT) /// Animacja poruszania siê w prawo
        {
            if (animationTimer.getElapsedTime().asSeconds() >= 0.1f || getAnimSwitch()) /// Czas animacji
            {
                currentFrame.top = 0.f;
                currentFrame.left += 30.f;
                if (currentFrame.left >= 100.f)
                    currentFrame.left = 0;

                animationTimer.restart();
                sprite.setTextureRect(currentFrame);
            }
            sprite.setScale(1.5, 1.7);
            sprite.setOrigin(0.f, 0.f);
        }
        else if (animationState == PLAYER_ANIMATION_STATES::MOVING_LEFT) /// Animacja poruszania siê w lewo
        {
            if (animationTimer.getElapsedTime().asSeconds() >= 0.1f || getAnimSwitch()) /// Czas animacji
            {
                currentFrame.top = 0.f;
                currentFrame.left += 30.f;
                if (currentFrame.left >= 100.f)
                    currentFrame.left = 0;

                animationTimer.restart();
                sprite.setTextureRect(currentFrame);
            }
            sprite.setScale(-1.5, 1.7);
            sprite.setOrigin(sprite.getGlobalBounds().width / 3.f, 0.f);
        }
        else
            animationTimer.restart();
    }

    ///metoda odpowiedzialna za wywo³anie strza³u
    void shoot()
    {
        if (shootTimer.getElapsedTime().asSeconds() >= shootDelay) /// Czas do strza³u
        {
            /// Obliczenie startowej pozycji pocisku
            sf::Vector2f startPosition = sprite.getPosition();
            startPosition.x += sprite.getGlobalBounds().width / 2;
            startPosition.y += sprite.getGlobalBounds().height / 2;

            /// Wystrzelenie pocisku w kierunku, w którym gracz jest skierowany
            /// Przyjmuje siê, ¿e kierunek to 1 dla prawej strony i -1 dla lewej
            projectiles.push_back(Projectile(startPosition, this->lastDirection, "graphics/bullet_texture.png"));

            shootTimer.restart(); /// Reset czasu do strza³u
        }
    }

    /// Poruszanie siê gracza
    void updateMovement()
    {
        animationState = PLAYER_ANIMATION_STATES::IDLE; /// Ustawienie animacji na "stanie"
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) /// Je¿eli naciœniêty jest klawisz "A"
        {
            move(-1.f, 0.f); /// Porusz graczem w lewo
            animationState = PLAYER_ANIMATION_STATES::MOVING_LEFT; /// Ustawienie animacji na "ruch w lewo"
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) /// Je¿eli naciœniêty jest klawisz "D"
        {
            move(1.f, 0.f); /// Porusz graczem w prawo
            animationState = PLAYER_ANIMATION_STATES::MOVING_RIGHT; /// Ustawienie animacji na "ruch w prawo"
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) /// Je¿eli naciœniêty jest klawisz "W"
        {
            jump(); /// Wywo³anie skoku
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) /// Je¿eli naciœniêty jest klawisz "Space"
        {
            shoot(); /// Wywo³anie strza³u

            /// Odtwarzanie dŸwiêku strza³u
            if (!shootSound.openFromFile("music/shoot.wav")) {
                cout << "Failed to load shoot sound" << endl;
                return;
            }

            shootSound.setLoop(false); /// Ustawienie, ¿e dŸwiêk nie powtarza siê
            shootSound.play(); /// Odtworzenie dŸwiêku
        }
    }

    void updateJumping()
    {
        static sf::Clock groundedTimer; /// Czas od ostatniego skoku
        static float lastY = sprite.getPosition().y; /// Ostatnia pozycja gracza w osi Y

        if (abs(lastY - sprite.getPosition().y) < 0.1f) /// Je¿eli gracz nie porusza siê w górê ani w dó³
        {
            /// Gracz prawdopodobnie jest na ziemi
            if (groundedTimer.getElapsedTime().asSeconds() > 0.1f) /// Je¿eli od ostatniego skoku minê³o wiêcej ni¿ 0.1 sekundy
            {
                isJumping = false; /// Ustawienie, ¿e gracz nie skacze
                groundedTimer.restart(); /// Reset czasu od ostatniego skoku
            }
        }
        else /// Gracz porusza siê w górê lub w dó³
        {
            isJumping = true; /// Ustawienie, ¿e gracz skacze
            groundedTimer.restart(); /// Reset czasu od ostatniego skoku
        }

        lastY = sprite.getPosition().y; /// Zapisanie aktualnej pozycji gracza w osi Y
    }

    void updateLastDirection()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) /// Je¿eli naciœniêty jest klawisz "A"
        {
            lastDirection = -1; /// Ustawienie kierunku gracza na lewo
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) /// Je¿eli naciœniêty jest klawisz "D"
        {
            lastDirection = 1; /// Ustawienie kierunku gracza na prawo
        }
    }
    /// Update
    void update(sf::Vector2u windowSize)
    {
        sf::Time deltaTime = clock.restart(); /// Restart zegara

        /// Aktualizacja ró¿nych stanów gracza
        updateJumping(); /// Aktualizacja skoków
        updateMovement(); /// Aktualizacja ruchu
        updateAnimations(); /// Aktualizacja animacji
        updatePhysics(); /// Aktualizacja fizyki
        updateLastDirection(); /// Aktualizacja ostatniego kierunku

        /// Aktualizacja pocisków
        for (auto it = projectiles.begin(); it != projectiles.end(); ) {
            it->update(deltaTime); /// Aktualizacja czasu trwania ka¿dego pocisku
            /// Jeœli pocisk przekroczy³ pewne granice lub spe³ni³ inne warunki, mo¿esz go usun¹æ
            if (it->outOfBounds(windowSize)) { /// Je¿eli pocisk wyjdzie poza okno gry
                it = projectiles.erase(it); /// Usuniêcie pocisku
            }
            else {
                ++it;
            }
        }

    }

    /// Render
    void render(sf::RenderTarget& target)
    {
        target.draw(sprite); /// Rysowanie gracza

        /// Rysowanie pocisków
        for (auto& projectile : projectiles) {
            target.draw(projectile.getSprite());
        }
    }

private:
    enum PLAYER_ANIMATION_STATES { IDLE, MOVING_LEFT, MOVING_RIGHT }; /// Stany animacji gracza

    sf::Sprite sprite; /// Sprite gracza
    sf::Texture textureSheet; /// Tekstura gracza
    sf::IntRect currentFrame; /// Aktualna klatka animacji
    sf::Clock animationTimer; /// Timer animacji
    bool animationSwitch; /// Prze³¹cznik animacji
    PLAYER_ANIMATION_STATES animationState; /// Stan animacji

    sf::Vector2f velocity; /// Prêdkoœæ gracza
    float maxVelocity; /// Maksymalna prêdkoœæ
    float minVelocity; /// Minimalna prêdkoœæ
    float acceleration; /// Przyspieszenie
    float drag; /// Opór
    float gravity; /// Grawitacja
    float maxVelocityY; /// Maksymalna prêdkoœæ w osi Y
    float jumpHeight; ///Maksymalna wysokoœæ skoku

    bool isJumping; ///czy skacze

    /// Inicjalizacja zmiennych
    void initVariables()
    {
        animationState = PLAYER_ANIMATION_STATES::IDLE; /// Ustawienie stanu animacji na IDLE
    }

    /// Inicjalizacja sprite'a
    void initSprite()
    {
        sprite.setTexture(textureSheet); /// Przypisanie tekstury do sprite'a
        currentFrame = sf::IntRect(0, 0, 32, 64); /// Ustalenie obszaru tekstury u¿ywanego dla sprite'a
        sprite.setTextureRect(currentFrame); /// Przypisanie obszaru tekstury do sprite'a
        sprite.setScale(1.5, 1.7); /// Skalowanie sprite'a
    }

    /// Inicjalizacja tekstury
    void initTexture()
    {
        if (!textureSheet.loadFromFile("graphics/test_player.png")) /// Wczytanie tekstury
        {
            cout << "ERROR::PLAYER::INITTEXTURE::NIE ZALADOWALO TEXTURY GRACZA!" << "\n"; /// Wiadomoœæ o b³êdzie, jeœli tekstura nie zostanie wczytana poprawnie
        }
    }

    /// Inicjalizacja animacji
    void initAnimations()
    {
        animationTimer.restart(); /// Restart timera animacji
        animationSwitch = true; /// Ustawienei prze³¹cznika animacji na true
    }

    /// Inicjalizacja fizyki
    void initPhysics()
    {
        maxVelocity = 10.f; /// Ustawienie maksymalnej prêdkoœci
        minVelocity = 1.f; /// Ustawienie minimalnej prêdkoœci
        acceleration = 3.f; /// Ustawienie przyspieszenia
        drag = 0.85f; /// Ustawienie oporu
        gravity = 4.f; /// Ustawienie grawitacji
        maxVelocityY = 15.f; /// Ustawienie maksymalnej prêdkoœci w osi Y
        jumpHeight = 300.f; /// Ustawienie maksymalnej wysokoœci skoku
    }
};

///Klasa TileMap Zajmuje siê wczytaniem mapy z pliku tmx.
class TileMap {
public:
    /// Konstruktor klasy TileMap - przyjmuje œcie¿kê do pliku z map¹ i skalê mapy jako argumenty
    TileMap(const std::string& filePath, float scale = 1.0f)
        : filePath(filePath), scale(scale)
    {
        loadFromFile(filePath); /// Wczytanie mapy z pliku
    }

    void loadFromFile(const std::string& filePath)
    {
        map = tmx::Map(); /// Inicjalizacja mapy
        /// Czyszczenie zawartoœci kontenerów przechowuj¹cych dane mapy
        tilesetTextures.clear();
        tileLayers.clear();
        transitionRects.clear();
        collisionRects.clear();

        /// Wczytanie mapy z pliku
        if (!map.load(filePath))  /// u¿yj argumentu filePath zamiast sta³ego stringa
        {
            /// Jeœli nie uda siê wczytaæ mapy, rzucenie wyj¹tku
            throw std::runtime_error("Failed to load tile map: " + filePath);
        }

        /// Wczytanie tekstur tilesetów
        const auto& tilesets = map.getTilesets();
        for (const auto& tileset : tilesets) {
            sf::Texture texture;
            if (!texture.loadFromFile(tileset.getImagePath())) {
                /// Jeœli nie uda siê wczytaæ tekstury tilesetu, rzucenie wyj¹tku
                throw std::runtime_error("Failed to load tileset texture: " + tileset.getImagePath());
            }
            tilesetTextures.push_back(std::move(texture)); /// Dodanie tekstury do wektora tekstur
        }

        /// Przechodzenie przez warstwy mapy
        for (const auto& layer : map.getLayers())
        {
            if (layer->getType() == tmx::Layer::Type::Tile)
            {
                const auto& tileLayer = dynamic_cast<const tmx::TileLayer&>(*layer);
                /// zapisz warstwê do wektora kafelków
                tileLayers.push_back(tileLayer.getTiles());

                /// PrzejdŸ przez wszystkie kafelki i dodaj prostok¹ty kolizji dla kafelków o ID 4
                int width = map.getBounds().width / map.getTileSize().x;
                for (std::size_t i = 0; i < tileLayer.getTiles().size(); ++i) {
                    if (tileLayer.getTiles()[i].ID == 4) {  /// tylko kafelki o ID 4 s¹ kolizyjne
                        int tilePosX = i % width;
                        int tilePosY = i / width;
                        sf::FloatRect rect(static_cast<float>(tilePosX * map.getTileSize().x * scale),
                            static_cast<float>(tilePosY * map.getTileSize().y * scale),
                            map.getTileSize().x * scale, map.getTileSize().y * scale);
                        collisionRects.push_back(rect); /// Dodanie prostok¹ta kolizji do wektora
                    }
                }
            }
        }

        /// Przechodzenie przez warstwy mapy
        for (const auto& layer : map.getLayers())
        {
            if (layer->getType() == tmx::Layer::Type::Tile)
            {
                const auto& tileLayer = dynamic_cast<const tmx::TileLayer&>(*layer);
                /// zapisz warstwê do wektora kafelków
                tileLayers.push_back(tileLayer.getTiles());

                /// PrzejdŸ przez wszystkie kafelki i dodaj prostok¹ty kolizji dla kafelków o ID 5
                int width = map.getBounds().width / map.getTileSize().x;
                for (std::size_t i = 0; i < tileLayer.getTiles().size(); ++i) {
                    if (tileLayer.getTiles()[i].ID == 5) {  /// tylko kafelki o ID 5 s¹ kolizyjne
                        int tilePosX = i % width;
                        int tilePosY = i / width;
                        sf::FloatRect rect(static_cast<float>(tilePosX * map.getTileSize().x * scale),
                            static_cast<float>(tilePosY * map.getTileSize().y * scale),
                            map.getTileSize().x * scale, map.getTileSize().y * scale);
                        collisionRects.push_back(rect); /// Dodanie prostok¹ta kolizji do wektora
                    }
                }
            }
        }

    }

    /// Funkcja do ponownego wczytania mapy
    void reload()
    {
        cout << "Reload Function works" << endl; /// Wyœwietlenie informacji o rozpoczêciu dzia³ania funkcji
        loadFromFile(filePath); /// Ponowne wczytanie mapy z pliku
        cout << "reload after loadFromFile" << endl; /// Wyœwietlenie informacji o zakoñczeniu dzia³ania funkcji
    }

    /// Funkcja aktualizuj¹ca logikê mapy (do zaimplementowania)
    void update(const sf::Time& deltaTime)
    {
        /// implementacja aktualizacji logiki mapy
    }

    /// Funkcja rysuj¹ca mapê na ekranie
    void draw(sf::RenderWindow& window)
    {
        /// Przechodzenie przez wszystkie warstwy kafelków
        for (std::size_t i = 0; i < tileLayers.size(); ++i) {
            const auto& tiles = tileLayers[i];

            /// Obliczanie szerokoœci mapy w kaflach
            int width = map.getBounds().width / map.getTileSize().x;
            /// Przechodzenie przez wszystkie kafelki danej warstwy
            for (std::size_t j = 0; j < tiles.size(); ++j) {
                /// Jeœli ID kafla wynosi 0, pomiñ go
                if (tiles[j].ID == 0) continue;

                /// Obliczanie pozycji kafla na mapie
                int tilePosX = j % width;
                int tilePosY = j / width;

                /// Okreœlenie odpowiedniej tekstury dla kafla na podstawie jego gid
                int gid = tiles[j].ID;
                /// Zak³adamy, ¿e ka¿dy tileset zawiera 256 kafelków, zmieñ to zgodnie ze swoimi tilesetami
                int tilesetIndex = (gid - 1) / 256;

                /// Pobranie odpowiedniej tekstury
                const auto& texture = tilesetTextures[tilesetIndex];

                /// Stworzenie nowego sprite'a dla ka¿dego kafla
                sf::Sprite sprite;
                sprite.setTexture(texture);

                /// Obliczenie pozycji kafla wewn¹trz tekstury
                int textureTilesPerRow = texture.getSize().x / map.getTileSize().x; /// Zak³adamy, ¿e kafelki s¹ kwadratowe
                int tileTexturePosX = (gid - 1) % textureTilesPerRow;
                int tileTexturePosY = (gid - 1) / textureTilesPerRow;
                sprite.setTextureRect(sf::IntRect(tileTexturePosX * map.getTileSize().x, tileTexturePosY * map.getTileSize().y, map.getTileSize().x, map.getTileSize().y));

                /// Skalowanie sprite'a przed ustaleniem jego pozycji
                sprite.setScale(scale, scale);
                sprite.setPosition(static_cast<float>(tilePosX * map.getTileSize().x * scale),
                    static_cast<float>(tilePosY * map.getTileSize().y * scale));

                /// Rysowanie sprite'a na ekranie
                window.draw(sprite);
            }
        }
    }

    /// Funkcja zwracaj¹ca warstwê kolizji
    const std::vector<tmx::TileLayer::Tile>& getCollisionLayer()
    {
        /// Jeœli nie ma wystarczaj¹cej iloœci warstw, rzucamy wyj¹tek
        if (tileLayers.size() < 2) {
            throw std::runtime_error("No collision layer found.");
        }
        /// Zwracamy drug¹ warstwê jako warstwê kolizji
        return tileLayers[1];
    }

    /// Funkcja zwracaj¹ca prostok¹ty kolizji
    const std::vector<sf::FloatRect>& getCollisionRects() const {
        return collisionRects;
    }

    /// Funkcja zwracaj¹ca prostok¹ty przejœcia
    const std::vector<sf::FloatRect>& getTransitionRects() const {
        return transitionRects;
    }

private:
    /// Prywatne pola klasy
    std::string filePath; /// œcie¿ka do pliku z map¹
    tmx::Map map; /// obiekt mapy
    std::vector<sf::Texture> tilesetTextures; /// tekstury tilesetów
    std::vector<std::vector<tmx::TileLayer::Tile>> tileLayers; /// warstwy kafelków
    vector<sf::FloatRect> transitionRects; /// prostok¹ty przejœcia
    std::vector<sf::FloatRect> collisionRects; /// prostok¹ty kolizji
    float scale; /// skala mapy
};

class SceneManager;

/// Klasa Scene reprezentuje jedn¹ scenê w grze (np. menu, ekran gry, ekran koñca gry).
class Scene {
public:
    /// Konstruktor przyjmuje wskaŸnik do okna, w którym scena bêdzie wyœwietlana.
    Scene(sf::RenderWindow* window) : window(window) { }

    /// Wirtualny destruktor - potrzebny, aby kiedykolwiek dziedziczymy z klasy z metodami wirtualnymi,
    /// destruktor powinien byæ równie¿ wirtualny. Pozwala to na poprawne zwolnienie zasobów
    /// kiedy obiekt jest usuwany przez wskaŸnik do bazowej klasy.
    virtual ~Scene() = default;

    /// Metoda do ustawiania menad¿era scen - u¿ywana do zmiany scen w grze.
    void SetManager(SceneManager* manager) { this->manager = manager; }

    /// Czysto wirtualna metoda Run - ka¿da scena musi j¹ zaimplementowaæ.
    /// Definiuje logikê, która ma byæ wykonana podczas uruchamiania sceny.
    virtual void Run() = 0;

protected:
    /// WskaŸnik na okno, na którym scena jest wyœwietlana.
    sf::RenderWindow* window;

    /// WskaŸnik na menad¿era scen, który zarz¹dza t¹ scen¹.
    SceneManager* manager;
};

/// Klasa SceneManager zarz¹dza ró¿nymi scenami w grze.
class SceneManager {
public:
    /// Konstruktor przyjmuje wskaŸnik na okno, w którym bêd¹ wyœwietlane sceny.
    SceneManager(sf::RenderWindow* window) : window(window) { }

    /// Metoda Run uruchamia now¹ scenê. Jest to metoda szablonowa, która przyjmuje typ sceny T jako pierwszy argument,
    /// a nastêpnie dowoln¹ liczbê argumentów (Args), które s¹ przekazywane do konstruktora sceny T.
    template <typename T, typename... Args>
    void Run(Args&&... args) {
        scene = std::make_unique<T>(window, std::forward<Args>(args)...); /// tworzy now¹ scenê
        scene->SetManager(this); /// ustawia ten menad¿er scen jako menad¿er dla nowo utworzonej sceny
        scene->Run(); /// uruchamia nowo utworzon¹ scenê
    }

    /// Metoda SwitchScene s³u¿y do prze³¹czania na inn¹ scenê. Jest to metoda szablonowa, która przyjmuje typ sceny T jako pierwszy argument,
    /// a nastêpnie dowoln¹ liczbê argumentów (Args), które s¹ przekazywane do konstruktora sceny T.
    template <typename T, typename... Args>
    void SwitchScene(Args&&... args) {
        scene = std::make_unique<T>(window, std::forward<Args>(args)...); /// tworzy now¹ scenê
        scene->SetManager(this); /// ustawia ten menad¿er scen jako menad¿er dla nowo utworzonej sceny
    }

private:
    /// WskaŸnik na aktualnie aktywn¹ scenê.
    std::unique_ptr<Scene> scene;

    /// WskaŸnik na okno, na którym sceny s¹ wyœwietlane.
    sf::RenderWindow* window;
};

class Menu;

class VictoryScene;

class GameOver;

/// Klasa Enemy reprezentuje wroga w grze.
class Enemy {
public:
    /// Konstruktor tworzy wroga na okreœlonej pozycji startowej i z okreœlon¹ tekstur¹.
    Enemy(const sf::Vector2f& startPos, const std::string& textureFile) {
        if (!texture.loadFromFile("graphics/enemy.png")) {
            cout << "ERROR::ENEMY::Nie uda³o siê za³adowaæ tekstury." << "\n";
        }
        sprite.setTexture(texture);
        sprite.setPosition(startPos);
        sprite.setScale(0.25, 0.3);

        initPhysics();  /// Inicjalizuje parametry fizyki dla wroga.
    }

    /// Ta metoda inicjalizuje parametry fizyki dla wroga.
    void initPhysics() {
        maxVelocity = 1.f;  /// Maksymalna prêdkoœæ.
        minVelocity = 1.f;  /// Minimalna prêdkoœæ.
        acceleration = 3.f;  /// Przyspieszenie.
        drag = 0.85f;  /// Opór.
        gravity = 4.f;  /// Si³a grawitacji.
        maxVelocityY = 15.f;  /// Maksymalna prêdkoœæ w osi Y.
    }

    /// Ta metoda resetuje prêdkoœæ w osi Y do 0.
    void resetVelocityY() {
        velocity.y = 0.f;
    }

    /// Ta metoda aktualizuje stan wroga, bior¹c pod uwagê pozycjê gracza i delta czasu.
    void update(const sf::Vector2f& playerPosition, sf::Time deltaTime) {
        /// Aplikowanie grawitacji na prêdkoœæ w osi Y.
        velocity.y += gravity;

        /// Ograniczenie prêdkoœci do maxVelocityY.
        if (std::abs(velocity.y) > maxVelocityY)
        {
            velocity.y = maxVelocityY * ((velocity.y < 0.f) ? -1.f : 1.f);
        }

        /// Zastosowanie oporu do prêdkoœci.
        velocity *= drag;

        /// Ustalenie prêdkoœci na 0, jeœli jest poni¿ej minVelocity.
        if (std::abs(velocity.x) < minVelocity)
            velocity.x = 0.f;
        if (std::abs(velocity.y) < minVelocity)
            velocity.y = 0.f;

        sprite.move(velocity);  /// Przesuniêcie wroga na podstawie prêdkoœci.

        /// Obliczenie kierunku i dystansu do gracza.
        sf::Vector2f direction = playerPosition - sprite.getPosition();
        float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));

        /// Jeœli gracz jest w zasiêgu 200px, wroga kieruje siê w stronê gracza.
        if (distance < 250.0f) {
            direction /= distance;  /// Normalizacja wektora kierunku.
            sprite.move(direction * speed * deltaTime.asSeconds());  /// Przesuniêcie wroga w stronê gracza.
        }
    }

    /// Metoda rysuj¹ca wroga na ekranie.
    void render(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    /// Metoda sprawdzaj¹ca, czy wrogi kolizje z graczem.
    bool checkCollision(const sf::Sprite& playerSprite) {
        return sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds());
    }

    /// Metoda sprawdzaj¹ca, czy wroga zosta³ trafiony przez jakikolwiek pocisk.
    bool checkProjectileHit(const std::vector<Projectile>& projectiles) {
        for (auto& projectile : projectiles) {
            if (sprite.getGlobalBounds().intersects(projectile.getSprite().getGlobalBounds())) {
                return true;
            }
        }
        return false;
    }

    /// Metoda sprawdzaj¹ca, czy wroga kolizjonuje z jakimkolwiek kafelkiem mapy.
    bool checkTileCollision(const TileMap& map) {
        auto collisionRects = map.getCollisionRects();
        for (const auto& rect : collisionRects) {
            if (sprite.getGlobalBounds().intersects(rect)) {
                return true;
            }
        }
        return false;
    }

    /// Metoda zwracaj¹ca sprite wroga.
    sf::Sprite& getSprite() {
        return sprite;
    }

    /// Metoda zwracaj¹ca aktualn¹ pozycjê wroga.
    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

private:
    sf::Sprite sprite;  /// Sprite reprezentuj¹cy wroga.
    sf::Texture texture;  /// Tekstura wroga.
    const float speed = 200.0f;  /// Prêdkoœæ wroga.

    sf::Vector2f velocity;  /// Aktualna prêdkoœæ wroga.
    float maxVelocity;  /// Maksymalna prêdkoœæ wroga.
    float minVelocity;  /// Minimalna prêdkoœæ wroga.
    float acceleration;  /// Przyspieszenie wroga.
    float drag;  /// Opór dzia³aj¹cy na wroga.
    float gravity;  /// Si³a grawitacji.
    float maxVelocityY;  /// Maksymalna prêdkoœæ w osi Y.
};

/// Klasa Finish reprezentuje miejsce koñca (cel) w grze.
class Finish {
public:
    /// Konstruktor tworzy miejsce koñca na okreœlonej pozycji startowej i z okreœlon¹ tekstur¹.
    Finish(const sf::Vector2f& startPos, const string& textureFile) {
        /// £aduje teksturê z pliku.
        if (!texture.loadFromFile("graphics/spaceship.png")) {
            cout << "ERROR::FINISH::Nie uda³o siê za³adowaæ tekstury statku finish" << endl;
        }

        /// Ustawia teksturê dla sprite'a.
        sprite.setTexture(texture);
        /// Ustawia pocz¹tkow¹ pozycjê sprite'a.
        sprite.setPosition(startPos);
        /// Ustawia skalê sprite'a.
        sprite.setScale(1, 1);
    }

    /// Metoda do renderowania (rysowania) miejsca koñca na ekranie.
    void render(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    /// Metoda sprawdzaj¹ca, czy dosz³o do kolizji z graczem.
    bool checkCollision(const sf::Sprite& playerSprite) {
        return sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds());
    }

    /// Metoda zwracaj¹ca sprite miejsca koñca.
    sf::Sprite& getSprite() {
        return sprite;
    }

private:
    /// Sprite reprezentuj¹cy miejsce koñca.
    sf::Sprite sprite;
    /// Tekstura miejsca koñca.
    sf::Texture texture;
};

///Klasa Space odpowiada za granicê mapy (otch³añ kosmosu) w któr¹ jak wpadniemy, przegrywamy i ³api¹ nas kosmici.
class Space {
public:
    /// Konstruktor klasy Space
    Space(const sf::Vector2f& startPos, const string& textureFile) {
        /// Za³adowanie tekstury obiektu pró¿ni
        if (!texture.loadFromFile("graphics/space.png")) {
            cout << "ERROR::SPACE::Nie uda³o siê za³adowaæ tekstury obiektu pró¿ni!" << endl;
        }

        /// Ustawienie tekstury dla sprite'a
        sprite.setTexture(texture);
        /// Ustawienie pozycji sprite'a na starcie
        sprite.setPosition(startPos);
        /// Skalowanie sprite'a
        sprite.setScale(20, 1);
    }

    /// Renderowanie sprite'a na oknie
    void render(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    /// Sprawdzanie kolizji miêdzy obiektem pró¿ni a sprite'em gracza
    bool checkCollision(const sf::Sprite& playerSprite) {
        return sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds());
    }

    /// Pobieranie referencji do sprite'a
    sf::Sprite& getSprite() {
        return sprite;
    }

private:
    sf::Sprite sprite;      /// Sprite obiektu pró¿ni
    sf::Texture texture;    /// Tekstura obiektu pró¿ni
};

///Klasa Game odpowiada za ca³¹ logikê gry.
class Game : public Scene {
public:
    /// Metoda zwracaj¹ca wskaŸnik do okna
    sf::RenderWindow* getWindow() {
        return window;
    }

    /// Metoda zwracaj¹ca pozycjê gracza
    sf::Vector2f getPlayerPosition() {
        return player->getPosition();
    }

    /// Konstruktor klasy Game, inicjalizuje zmienne, okno, gracza, przeciwników, metê, obszar kosmiczny i muzykê
    Game(sf::RenderWindow* window) : Scene(window) {
        initVariables();
        initWindow();
        initPlayer();
        initEnemies();
        initFinish();
        initSpace();
        initStartMusic();
        initMusic();
        initTileMap();
    }

    /// Destruktor klasy Game, zwalnia zaalokowan¹ pamiêæ dla okna, gracza, przeciwników, mety i obszaru kosmicznego
    ~Game() {
        delete window;
        delete player;
        for (auto& enemy : enemies) {
            delete enemy;
        }
        for (auto& finish : finishes) {
            delete finish;
        }
        for (auto& space : spaces) {
            delete space;
        }
    }

    /// Metoda zwracaj¹ca informacjê, czy gra jest uruchomiona (okno jest otwarte)
    const bool running() const {
        return window->isOpen();
    }

    /// Metoda wykonuj¹ca g³ówn¹ pêtlê gry
    void Run() override {
        try {
            while (running()) {
                pollEvents();
                update();
                render();
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Exception occurred: " << ex.what() << '\n';
        }
    }

private:
    sf::RenderWindow* window;       /// WskaŸnik do obiektu okna
    sf::VideoMode videoMode;        /// Obiekt przechowuj¹cy ustawienia trybu wideo
    sf::Event event;                /// Obiekt przechowuj¹cy zdarzenia
    Player* player;                 /// WskaŸnik do obiektu gracza
    Finish* finish;                 /// WskaŸnik do obiektu mety
    Space* space;                   /// WskaŸnik do obiektu obszaru kosmicznego
    vector<Enemy*> enemies;         /// Wektor wskaŸników do obiektów przeciwników
    vector<Finish*> finishes;       /// Wektor wskaŸników do obiektów mety (dla obs³ugi wielu met)
    vector<Space*> spaces;          /// Wektor wskaŸników do obiektów obszaru kosmicznego
    unique_ptr<TileMap> tileMap;    /// Unikalny wskaŸnik do obiektu mapy kafelków
    sf::Clock globalClock;          /// Zegar globalny

    sf::Music startMissionMusic;    /// Muzyka rozpoczêcia misji
    sf::Music enemyDeath;           /// DŸwiêk œmierci przeciwnika
    sf::Music backgroundMusic;      /// Muzyka w tle

    /// Inicjalizacja zmiennych
    void initVariables() {
        window = nullptr;
    }

    /// Inicjalizacja okna
    void initWindow() {
        videoMode.height = 600;
        videoMode.width = 800;
        window = new sf::RenderWindow(videoMode, "Kapitan Bomba The Game", sf::Style::Titlebar | sf::Style::Close);
        window->setFramerateLimit(60);
    }

    /// Inicjalizacja gracza
    void initPlayer() {
        player = new Player();
    }

    /// Inicjalizacja przeciwników
    void initEnemies() {
        /// Dodawanie przeciwników do wektora enemies
        /// Ustawienie pozycji przeciwników wzglêdem pozycji gracza
        /// U¿ywanie odpowiednich tekstur dla przeciwników
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 590, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 790, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 990, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1090, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1190, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1290, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1490, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1590, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1790, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 1990, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2090, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2190, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2390, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2490, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2690, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2890, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 2990, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3090, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3290, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3490, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3590, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3690, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3790, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 3990, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 4090, player->getPosition().y), "enemyTextureFile"));
        enemies.push_back(new Enemy(sf::Vector2f(player->getPosition().x + 4190, player->getPosition().y), "enemyTextureFile"));
        /// ...
        /// Dodawanie pozosta³ych przeciwników
    }

    /// Inicjalizacja mety
    void initFinish() {
        /// Dodawanie mety do wektora finishes
        /// Ustawienie pozycji mety wzglêdem pozycji gracza
        /// U¿ywanie odpowiedniej tekstury dla mety
        finishes.push_back(new Finish(sf::Vector2f(player->getPosition().x + 4200, player->getPosition().y + 150), "finishTextureFile"));
        /// ...
        /// Dodawanie pozosta³ych met
    }

    /// Inicjalizacja obszaru kosmicznego
    void initSpace() {
        /// Dodawanie obszaru kosmicznego do wektora spaces
        /// Ustawienie pozycji obszaru kosmicznego wzglêdem pozycji gracza
        /// U¿ywanie odpowiedniej tekstury dla obszaru kosmicznego
        spaces.push_back(new Space(sf::Vector2f(player->getPosition().x - 1000, player->getPosition().y + 900), "SpaceTextureFile"));
    }

    /// Inicjalizacja mapy kafelków
    void initTileMap() {
        /// Inicjalizacja obiektu tileMap za pomoc¹ unikalnego wskaŸnika
        /// Przekazanie œcie¿ki do pliku mapy kafelków i skali mapy
        tileMap = unique_ptr<TileMap>(new TileMap("tiled/map_1.tmx", 3.0f));
    }

    /// Inicjalizacja muzyki rozpoczêcia misji
    void initStartMusic() {
        /// Otwieranie pliku dŸwiêkowego i sprawdzanie, czy siê uda³o
        /// Jeœli nie uda³o siê, wyœwietlanie komunikatu o b³êdzie
        if (!startMissionMusic.openFromFile("music/start_mission.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        startMissionMusic.setLoop(false);
        startMissionMusic.play();
    }

    /// Inicjalizacja muzyki w tle
    void initMusic() {
        /// Otwieranie pliku dŸwiêkowego i sprawdzanie, czy siê uda³o
        /// Jeœli nie uda³o siê, wyœwietlanie komunikatu o b³êdzie
        if (!backgroundMusic.openFromFile("music/mission.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        backgroundMusic.setLoop(true);  /// Ustawienie zapêtlenia muzyki
        backgroundMusic.play();         /// Odtwarzanie muzyki
    }

    /// Obs³uga zdarzeñ, takich jak zamkniêcie okna lub wciœniêcie klawisza
    void pollEvents() {
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();       /// Zamkniêcie okna
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    window->close();   /// Zamkniêcie okna
                    manager->Run<Menu>();  /// Prze³¹czenie na scenê menu
                }
                if (event.key.code == sf::Keyboard::R) {
                    cout << "Map just got reloaded" << endl;    /// Wyœwietlenie komunikatu o prze³adowaniu mapy
                    tileMap->reload();  /// Prze³adowanie mapy kafelków
                }
                //Poni¿ej przyciski Testowe!!
                if (event.key.code == sf::Keyboard::V) {
                    cout << "Victory screen just got loaded" << endl;   /// Wyœwietlenie komunikatu o za³adowaniu ekranu zwyciêstwa
                    window->close();
                    manager->Run<VictoryScene>();   /// Prze³¹czenie na scenê zwyciêstwa
                }
                if (event.key.code == sf::Keyboard::G) {
                    cout << "GAMEOVER screen just got loaded" << endl;   /// Wyœwietlenie komunikatu o za³adowaniu ekranu zakoñczenia gry
                    window->close();
                    manager->Run<GameOver>();   /// Prze³¹czenie na scenê zakoñczenia gry
                }
                break;
            }
        }
    }

    /// Aktualizacja stanu gracza
    void updatePlayer() {
        player->update(window->getSize());   /// Wywo³anie metody update() na obiekcie gracza, przekazanie rozmiaru okna jako argument
    }

    /// Aktualizacja stanu przeciwników
    void updateEnemies(sf::Time duration) {
        for (auto& enemy : enemies) {
            /// Zapamiêtaj poprzedni¹ pozycjê wroga przed aktualizacj¹
            sf::Vector2f oldPosition = enemy->getSprite().getPosition();

            /// Wykonaj aktualizacjê, która potencjalnie przesuwa wroga w kierunku gracza
            enemy->update(player->getPosition(), duration);

            sf::FloatRect enemyBounds = enemy->getSprite().getGlobalBounds();

            for (const auto& rect : tileMap->getCollisionRects()) {
                if (enemyBounds.intersects(rect)) {
                    /// Wróg koliduje z jednym z kafelków. Wykonaj odpowiedni¹ akcjê, np.:

                    enemy->resetVelocityY();
                    enemy->getSprite().setPosition(enemy->getSprite().getPosition().x, rect.top - enemy->getSprite().getGlobalBounds().height);

                    bool isEnemyLeftOfPlayer = enemy->getSprite().getPosition().x < player->getPosition().x;

                    if (isEnemyLeftOfPlayer) {
                        /// Je¿eli wróg jest na lewo od gracza, pozwól mu siê poruszaæ w prawo (czyli do gracza)
                        /// ale nie w lewo (z dala od gracza)
                        if (enemy->getSprite().getPosition().x < oldPosition.x) {
                            enemy->getSprite().setPosition(oldPosition.x, enemy->getSprite().getPosition().y);
                        }
                    }
                    else {
                        /// Je¿eli wróg jest na prawo od gracza, pozwól mu siê poruszaæ w lewo (czyli do gracza)
                        /// ale nie w prawo (z dala od gracza)
                        if (enemy->getSprite().getPosition().x > oldPosition.x) {
                            enemy->getSprite().setPosition(oldPosition.x, enemy->getSprite().getPosition().y);
                        }
                    }

                    break;
                }
            }
        }
    }

    /// Aktualizacja stanu mety
    void updateFinish() {
        for (auto& finish : finishes) {
            if (finish->checkCollision(player->getSprite())) {
                cout << "Gracz dotar³ do mety!" << endl;
                window->close();
                manager->Run<VictoryScene>();
            }
        }
    }

    /// Aktualizacja stanu obszaru kosmicznego
    void updateSpace() {
        for (auto& space : spaces) {
            if (space->checkCollision(player->getSprite())) {
                cout << "Gracz wpad³ w kosmos!" << endl;
                window->close();
                manager->Run<GameOver>();
            }
        }
    }

    /// Aktualizacja kolizji gracza z kafelkami i przejœciami
    void updateCollision() {
        sf::FloatRect playerBounds = player->getGlobalBounds();

        /// Sprawdzenie kolizji z kafelkami
        for (const auto& rect : tileMap->getCollisionRects()) {
            if (playerBounds.intersects(rect)) {
                /// Gracz koliduje z jednym z kafelków. Wykonaj odpowiedni¹ akcjê, np.:
                player->resetVelocityY();
                player->setPosition(player->getPosition().x, rect.top - player->getGlobalBounds().height);
                break;
            }
        }

        /// Sprawdzenie kolizji z przejœciami
        for (const auto& rect : tileMap->getTransitionRects()) {
            if (playerBounds.intersects(rect)) {
                /// Gracz wpad³ na przejœcie. Zmieñ okno na inne.
                window->close();
                manager->Run<VictoryScene>();
                break;
            }
        }
    }

    /// Sprawdzenie kolizji gracza z przeciwnikami
    void checkPlayerEnemyCollisions(sf::Time duration) {
        for (auto it = enemies.begin(); it != enemies.end();) {
            /// Aktualizacja wroga
            (*it)->update(player->getPosition(), duration);

            /// Sprawdzanie kolizji z graczem
            if ((*it)->checkCollision(player->getSprite())) {
                /// Sprawdzanie, czy gracz jest powy¿ej wroga
                if (player->getPosition().y < (*it)->getPosition().y) {
                    if (!enemyDeath.openFromFile("music/enemy.wav")) {
                        cout << "Failed to load background music" << endl;
                        return;
                    }

                    enemyDeath.setLoop(false);
                    enemyDeath.play();
                    /// Usuwanie wroga
                    cout << "Gracz pokona³ wroga skacz¹c mu po g³owie!" << endl;
                    delete* it; /// Zwalnianie pamiêci
                    it = enemies.erase(it); /// Usuwanie elementu z wektora i aktualizacja iteratora
                }
                else {
                    /// Wykryto kolizjê miêdzy graczem a wrogem!
                    cout << "Gracz koliduje z wrogiem!" << endl;
                    window->close();
                    manager->Run<GameOver>();
                    return; /// Przerwanie pêtli, poniewa¿ gra zosta³a zakoñczona
                }
            }
            else {
                ++it; /// Przechodzenie do nastêpnego wroga
            }
        }
    }

    /// Sprawdzenie kolizji pocisków gracza z przeciwnikami
    void checkProjectileEnemyCollisions() {
        std::vector<Projectile*> projectilesToRemove;

        /// Pêtla po wszystkich wrogach
        for (auto& enemy : enemies) {
            /// Sprawdzanie kolizji dla ka¿dego pocisku gracza
            for (auto& projectile : player->getProjectiles()) {
                if (enemy->getSprite().getGlobalBounds().intersects(projectile.getSprite().getGlobalBounds())) {
                    /// Wykryto kolizjê miêdzy pociskiem a wrogiem!
                    /// Zdecyduj, co ma siê zdarzyæ w tej sytuacji...
                    cout << "Pocisk trafi³ wroga!" << endl;
                }
            }
        }
    }

    /// Aktualizacja widoku kamery
    void updateView() {
        sf::View view = window->getView(); /// Pobierz bie¿¹cy widok
        sf::Vector2f playerPosition = getPlayerPosition(); /// Pobierz aktualn¹ pozycjê gracza

        /// Zmienne przechowuj¹ce wielkoœæ okna
        float windowHeight = static_cast<float>(window->getSize().y);
        float windowWidth = static_cast<float>(window->getSize().x);

        /// Zmienne przechowuj¹ce wielkoœæ mapy
        float mapHeight = 900; /// Wysokoœæ mapy
        float mapWidth = 4500; /// Szerokoœæ mapy

        /// Ustal œrodek widoku na pozycjê gracza, ale nie przesuwaj widoku poza granice mapy
        playerPosition.x = std::max(playerPosition.x, windowWidth / 2);
        playerPosition.y = std::max(playerPosition.y, windowHeight / 2);
        playerPosition.x = std::min(playerPosition.x, mapWidth - windowWidth / 2);
        playerPosition.y = std::min(playerPosition.y, mapHeight - windowHeight / 2);

        /// Aktualizuj œrodek widoku na pozycjê gracza
        view.setCenter(playerPosition);

        /// Zastosuj nowy widok do okna
        window->setView(view);
    }

    /// Aktualizacja gry
    void update() {
        pollEvents();

        /// Resetuj animacjê gracza po zwolnieniu klawiszy A, D lub W
        if (event.key.code == sf::Event::KeyReleased &&
            (event.key.code == sf::Keyboard::A ||
                event.key.code == sf::Keyboard::D ||
                event.key.code == sf::Keyboard::W
                )) {
            player->resetAnimationTimer();
        }

        sf::Time duration = globalClock.restart();
        tileMap->update(duration);
        updatePlayer();
        updateEnemies(duration);
        updateFinish();
        updateSpace();
        updateCollision();
        checkPlayerEnemyCollisions(duration);
        checkProjectileEnemyCollisions();
        updateView();
    }

    /// Rysowanie gracza
    void renderPlayer() {
        player->render(*window);
    }

    /// Rysowanie przeciwników
    void renderEnemies() {
        for (auto& enemy : enemies) {
            enemy->render(*window);
        }
    }

    /// Rysowanie mety
    void renderFinishes() {
        for (auto& finish : finishes) {
            finish->render(*window);
        }
    }

    /// Rysowanie kosmosu
    void renderSpaces() {
        for (auto& space : spaces) {
            space->render(*window);
        }
    }

    /// Rysowanie gry
    void render() {
        window->clear(sf::Color::Blue);
        tileMap->draw(*window);
        renderPlayer();
        renderEnemies();
        renderFinishes();
        renderSpaces();
        window->display();
    }
};

///Klasa VictoryScene odpowiada za scenê zwyciêstwa i wygrania gry.
class VictoryScene : public Scene {
public:
    /// Konstruktor klasy VictoryScene, inicjalizuje okno i elementy sceny
    VictoryScene(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initVictoryMusic();
    }

    /// Destruktor klasy VictoryScene, zwalnia zaalokowan¹ pamiêæ dla tekstur przycisków
    ~VictoryScene() {
        delete backgroundTexture;
        delete nextButtonTexture;
        delete menuButtonTexture;
    }

    /// Metoda wykonuj¹ca g³ówn¹ pêtlê sceny Victory
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;            /// WskaŸnik do obiektu okna
    sf::Texture* backgroundTexture;      /// WskaŸnik do tekstury t³a
    sf::Sprite backgroundSprite;          /// Sprite t³a
    sf::Texture* nextButtonTexture;       /// WskaŸnik do tekstury przycisku "Dalej"
    sf::RectangleShape nextButton;        /// Prostok¹tny kszta³t przycisku "Dalej"
    sf::Texture* menuButtonTexture;       /// WskaŸnik do tekstury przycisku "Menu"
    sf::RectangleShape menuButton;        /// Prostok¹tny kszta³t przycisku "Menu"
    sf::Music victorySound;               /// DŸwiêk zwyciêstwa

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "Victory");
    }

    /// Inicjalizacja t³a
    void initBackground() {
        backgroundTexture = new sf::Texture();
        if (!backgroundTexture->loadFromFile("graphics/victory.png")) {
            cout << "Failed to load background victory texture!" << endl;
            return;
        }

        backgroundSprite.setTexture(*backgroundTexture);
        backgroundSprite.setPosition(0, 0);
        backgroundSprite.setScale(1, 1);
    }

    /// Inicjalizacja przycisków
    void initButtons() {
        nextButtonTexture = new sf::Texture();
        if (!nextButtonTexture->loadFromFile("graphics/exit_button.png")) {
            cout << "Failed to load next button texture!" << endl;
            return;
        }

        nextButton.setSize(sf::Vector2f(200, 50));
        nextButton.setTexture(nextButtonTexture);
        nextButton.setPosition(0, 500);

        menuButtonTexture = new sf::Texture();
        if (!menuButtonTexture->loadFromFile("graphics/menu_button.png")) {
            cout << "Failed to load menu button texture!" << endl;
            return;
        }

        menuButton.setSize(sf::Vector2f(200, 50));
        menuButton.setTexture(menuButtonTexture);
        menuButton.setPosition(600, 500);
    }

    /// Inicjalizacja dŸwiêku zwyciêstwa
    void initVictoryMusic() {
        if (!victorySound.openFromFile("music/victory.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        victorySound.setLoop(true);
        victorySound.play();
    }

    /// Reakcja na zdarzenia, takie jak zamkniêcie okna lub klikniêcie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Dalej" zosta³ klikniêty
                    if (nextButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "next Button pressed!" << endl;
                        window->close();
                        /// manager->Run<Menu>();  /// Odkomentuj tê liniê dla aktualizacji 0.2 alfa
                    }
                    /// Sprawdzenie, czy przycisk "Menu" zosta³ klikniêty
                    else if (menuButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "menu button Pressed!" << endl;
                        window->close();
                        manager->Run<Menu>();
                    }
                }
                break;
            }
        }
    }

    /// Renderowanie sceny
    void render() {
        window->clear();
        window->draw(backgroundSprite);
        window->draw(nextButton);
        window->draw(menuButton);
        window->display();
    }
};

///Klasa GameOver odpowiada za scenê pora¿ki i przegrania gry.
class GameOver : public Scene {
public:
    /// Konstruktor klasy GameOver, inicjalizuje okno i elementy sceny
    GameOver(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initDeathSound();
    }

    /// Destruktor klasy GameOver, zwalnia zaalokowan¹ pamiêæ dla tekstur przycisków
    ~GameOver() {
        delete backgroundTexture;
        delete restartButtonTexture;
        delete menuButtonTexture;
    }

    /// Metoda wykonuj¹ca g³ówn¹ pêtlê sceny GameOver
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;            /// WskaŸnik do obiektu okna
    sf::Texture* backgroundTexture;      /// WskaŸnik do tekstury t³a
    sf::Sprite backgroundSprite;          /// Sprite t³a
    sf::Texture* restartButtonTexture;    /// WskaŸnik do tekstury przycisku "Restart"
    sf::RectangleShape restartButton;     /// Prostok¹tny kszta³t przycisku "Restart"
    sf::Texture* menuButtonTexture;       /// WskaŸnik do tekstury przycisku "Menu"
    sf::RectangleShape menuButton;        /// Prostok¹tny kszta³t przycisku "Menu"
    sf::Music deathSound;                 /// DŸwiêk œmierci

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "GAME OVER");
    }

    /// Inicjalizacja t³a
    void initBackground() {
        backgroundTexture = new sf::Texture();
        if (!backgroundTexture->loadFromFile("graphics/gameover.png")) {
            cout << "Failed to load GameOver background texture" << endl;
            return;
        }

        backgroundSprite.setTexture(*backgroundTexture);
        backgroundSprite.setPosition(0, 0);
        backgroundSprite.setScale(1, 1);
    }

    /// Inicjalizacja przycisków
    void initButtons() {
        restartButtonTexture = new sf::Texture();
        if (!restartButtonTexture->loadFromFile("graphics/restart_button.png")) {
            cout << "Failed to load restart button texture!" << endl;
            return;
        }

        restartButton.setSize(sf::Vector2f(200, 50));
        restartButton.setTexture(restartButtonTexture);
        restartButton.setPosition(600, 500);

        menuButtonTexture = new sf::Texture();
        if (!menuButtonTexture->loadFromFile("graphics/menu_button.png")) {
            cout << "Failed to load menu button texture!" << endl;
            return;
        }

        menuButton.setSize(sf::Vector2f(200, 50));
        menuButton.setTexture(menuButtonTexture);
        menuButton.setPosition(0, 500);
    }

    /// Inicjalizacja dŸwiêku œmierci
    void initDeathSound() {
        if (!deathSound.openFromFile("music/death_sound.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        deathSound.setLoop(false);
        deathSound.play();
    }

    /// Reakcja na zdarzenia, takie jak zamkniêcie okna lub klikniêcie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Restart" zosta³ klikniêty
                    if (restartButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "restart Button pressed!" << endl;
                        window->close();
                        manager->Run<Game>();
                    }
                    /// Sprawdzenie, czy przycisk "Menu" zosta³ klikniêty
                    else if (menuButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "menu button Pressed!" << endl;
                        window->close();
                        manager->Run<Menu>();
                    }
                }
                break;
            }
        }
    }

    /// Renderowanie sceny
    void render() {
        window->clear();
        window->draw(backgroundSprite);
        window->draw(restartButton);
        window->draw(menuButton);
        window->display();
    }
};

///Klasa KeyboardOption odpowiada za scenê klawiszologii (opisanie poruszania siê w grze).
class KeyboardOption : public Scene {
public:
    /// Konstruktor klasy KeyboardOption, inicjalizuje okno i elementy sceny
    KeyboardOption(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initMusic();
    }

    /// Destruktor klasy KeyboardOption, zwalnia zaalokowan¹ pamiêæ dla tekstur i okna
    ~KeyboardOption() {
        delete backgroundTexture;
        delete menuButtonTexture;
        delete window;
    }

    /// Metoda wykonuj¹ca g³ówn¹ pêtlê sceny KeyboardOption
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;            /// WskaŸnik do obiektu okna
    sf::Texture* backgroundTexture;      /// WskaŸnik do tekstury t³a
    sf::Sprite backgroundSprite;          /// Sprite t³a
    sf::Texture* menuButtonTexture;       /// WskaŸnik do tekstury przycisku "Menu"
    sf::RectangleShape menuButton;        /// Prostok¹tny kszta³t przycisku "Menu"
    sf::Music backgroundMusic;            /// Muzyka w tle

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "KLAWISZOLOGIA");
    }

    /// Inicjalizacja t³a
    void initBackground() {
        backgroundTexture = new sf::Texture();
        if (!backgroundTexture->loadFromFile("graphics/keyboard.png")) {
            cout << "Failed to load Keyboard Option texture" << endl;
            return;
        }

        backgroundSprite.setTexture(*backgroundTexture);
        backgroundSprite.setPosition(0, 0);
        backgroundSprite.setScale(1, 1);
    }

    /// Inicjalizacja przycisku "Menu"
    void initButtons() {
        menuButtonTexture = new sf::Texture();
        if (!menuButtonTexture->loadFromFile("graphics/exit_button.png")) {
            cout << "Failed to load menu button texture!" << endl;
            return;
        }

        menuButton.setSize(sf::Vector2f(200, 50));
        menuButton.setTexture(menuButtonTexture);
        menuButton.setPosition(0, 500);
    }

    /// Inicjalizacja muzyki w tle
    void initMusic() {
        if (!backgroundMusic.openFromFile("music/background_music.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    /// Reakcja na zdarzenia, takie jak zamkniêcie okna lub klikniêcie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Menu" zosta³ klikniêty
                    if (menuButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "menu button Pressed!" << endl;
                        window->close();
                        manager->Run<Menu>();
                    }
                }
                break;
            }
        }
    }

    /// Renderowanie sceny
    void render() {
        window->clear();
        window->draw(backgroundSprite);
        window->draw(menuButton);
        window->display();
    }
};

///Klasa Menu odpowiada za scenê g³ównego Menu gry - wybieramy w niej START - start gry || KLAWISZE - opcje sterowania || EXIT - wyjœcie z gry i zamkniêcie programu
class Menu : public Scene {
public:
    /// Konstruktor klasy Menu, inicjalizuje okno i elementy sceny
    Menu(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initMusic();
    }

    /// Metoda wykonuj¹ca g³ówn¹ pêtlê sceny Menu
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;                   /// WskaŸnik do obiektu okna
    std::unique_ptr<sf::Texture> backgroundTexture;       /// Unikalny wskaŸnik do tekstury t³a
    sf::Sprite backgroundSprite;                       /// Sprite t³a
    std::unique_ptr<sf::Texture> startButtonTexture;     /// Unikalny wskaŸnik do tekstury przycisku "Start"
    sf::RectangleShape startButton;                     /// Prostok¹tny kszta³t przycisku "Start"
    std::unique_ptr<sf::Texture> exitButtonTexture;      /// Unikalny wskaŸnik do tekstury przycisku "Exit"
    sf::RectangleShape exitButton;                       /// Prostok¹tny kszta³t przycisku "Exit"
    std::unique_ptr<sf::Texture> KeyboardButtonTexture;  /// Unikalny wskaŸnik do tekstury przycisku "Keyboard Option"
    sf::RectangleShape KeyboardButton;                   /// Prostok¹tny kszta³t przycisku "Keyboard Option"

    sf::Music backgroundMusic;                /// Muzyka w tle

    bool startButtonClicked;                   /// Flaga informuj¹ca, czy przycisk "Start" zosta³ klikniêty

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "Menu");
    }

    /// Inicjalizacja t³a
    void initBackground() {
        backgroundTexture = std::make_unique<sf::Texture>();
        if (!backgroundTexture->loadFromFile("graphics/menu.jpg")) {
            cout << "Failed to load background texture!" << endl;
            return;
        }

        backgroundSprite.setTexture(*backgroundTexture);
        backgroundSprite.setPosition(-250, -270);
        backgroundSprite.setScale(2.7, 3.2);
    }

    /// Inicjalizacja przycisków
    void initButtons() {
        startButtonTexture = std::make_unique<sf::Texture>();
        if (!startButtonTexture->loadFromFile("graphics/start_button.png")) {
            std::cout << "Failed to load start button texture!" << std::endl;
            return;
        }

        startButton.setSize(sf::Vector2f(200, 50));
        startButton.setTexture(startButtonTexture.get());
        startButton.setPosition(300, 300);

        exitButtonTexture = std::make_unique<sf::Texture>();
        if (!exitButtonTexture->loadFromFile("graphics/exit_button.png")) {
            std::cout << "Failed to load exit button texture!" << std::endl;
            return;
        }

        exitButton.setSize(sf::Vector2f(200, 50));
        exitButton.setTexture(exitButtonTexture.get());
        exitButton.setPosition(300, 500);

        KeyboardButtonTexture = std::make_unique<sf::Texture>();
        if (!KeyboardButtonTexture->loadFromFile("graphics/keyboard_button.png")) {
            std::cout << "Failed to load level select button texture!" << std::endl;
            return;
        }

        KeyboardButton.setSize(sf::Vector2f(200, 50));
        KeyboardButton.setTexture(KeyboardButtonTexture.get());
        KeyboardButton.setPosition(300, 400);
    }

    /// Inicjalizacja muzyki w tle
    void initMusic() {
        if (!backgroundMusic.openFromFile("music/background_music.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        backgroundMusic.setLoop(true);
        backgroundMusic.play();
    }

    /// Reakcja na zdarzenia, takie jak zamkniêcie okna lub klikniêcie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Start" zosta³ klikniêty
                    if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "Start button pressed!" << std::endl;
                        window->close();
                        manager->Run<Game>(); /// Prze³¹cz na scenê gry
                    }
                    /// Sprawdzenie, czy przycisk "Exit" zosta³ klikniêty
                    else if (exitButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "Exit button pressed!" << std::endl;
                        window->close();
                    }
                    /// Sprawdzenie, czy przycisk "Keyboard Option" zosta³ klikniêty
                    else if (KeyboardButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "Keyboard Option button pressed!" << std::endl;
                        window->close();
                        manager->Run<KeyboardOption>();

                        /// Tutaj mo¿esz dodaæ kod do przejœcia do ekranu wyboru poziomu
                    }
                }
                break;
            }
        }
    }

    /// Renderowanie sceny
    void render() {
        window->clear();
        window->draw(backgroundSprite);
        window->draw(startButton);
        window->draw(KeyboardButton);
        window->draw(exitButton);
        window->display();
    }
};

/// G³ówna funkcja programu
int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Game"); /// Inicjalizacja okna gry o rozmiarze 800x600 pikseli
    SceneManager manager(&window); /// Inicjalizacja mened¿era scen, który zarz¹dza prze³¹czaniem miêdzy scenami

    manager.Run<Menu>(); /// Uruchomienie gry, zaczynaj¹c od sceny Menu

    return 0; /// Zakoñczenie programu
}