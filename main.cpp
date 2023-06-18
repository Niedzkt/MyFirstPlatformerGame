/// Definiuje sta��, aby wy��czy� ostrze�enia zwi�zane z funkcjami, kt�re Microsoft uwa�a za niebezpieczne
#define _CRT_SECURE_NO_WARNINGS

/// Standardowa biblioteka C++ do obs�ugi strumieni danych
#include <iostream>

/// Biblioteka matematyczna w C++, zawiera funkcje takie jak sin, cos, sqrt, itp.
#include <cmath>

/// Kontener sekwencyjny przechowuj�cy elementy
#include <vector>

/// Obs�uga �a�cuch�w znak�w w C++
#include <string>

/// Wsparcie dla zarz�dzania dynamiczn� pami�ci� za pomoc� wska�nik�w inteligentnych
#include <memory>

/// Pliki nag��wkowe biblioteki SFML do obs�ugi grafiki 2D
#include <SFML/Graphics.hpp>

/// Pliki nag��wkowe biblioteki SFML do obs�ugi d�wi�ku
#include <SFML/Audio.hpp>

/// Pliki nag��wkowe biblioteki SFML do obs�ugi okien
#include <SFML/Window.hpp>

/// Pliki nag��wkowe biblioteki SFML do obs�ugi systemu
#include <SFML/System.hpp>

/// Dodatkowe pliki nag��wkowe biblioteki SFML do obs�ugi zdarze� okna
#include <SFML/Window/Event.hpp>

/// Dodatkowe pliki nag��wkowe biblioteki SFML do obs�ugi renderowania okna
#include <SFML/Graphics/RenderWindow.hpp>

/// Dodatkowe pliki nag��wkowe biblioteki SFML do obs�ugi d�wi�ku i muzyki
#include <SFML/Audio.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

/// Pliki nag��wkowe biblioteki tmxlite, kt�ra umo�liwia obs�ug� plik�w map w formacie TMX (Tiled Map XML)
/// u�ywanych do tworzenia map kafelkowych w grach 2D
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/LayerGroup.hpp>
#include <tmxlite/ObjectGroup.hpp>

using namespace std;

/// Klasa reprezentuj�ca pocisk w grze.
class Projectile {
public:
    /// Konstruktor, kt�ry tworzy nowy pocisk w okre�lonym miejscu, kieruje go w okre�lonym kierunku i wczytuje tekstur� pocisku z pliku.
    Projectile(const sf::Vector2f& startPosition, int direction, const std::string& textureFile) {
        /// Wczytanie tekstury pocisku z pliku
        if (!texture.loadFromFile("graphics/bullet_texture.png")) {
            /// Je�eli tekstura nie zosta�a poprawnie wczytana, wypisuje b��d
            cout << "ERROR::PROJECTILE::BRAK TEKSTURY" << "\n";
        }

        /// Ustawienie tekstury na sprite pocisku
        sprite.setTexture(texture);
        /// Ustawienie pocz�tkowej pozycji pocisku
        sprite.setPosition(startPosition);
        /// Skalowanie pocisku
        sprite.setScale(0.1f, 0.05f);

        /// Ustawienie kierunku ruchu pocisku. Kierunek jest reprezentowany jako liczba ca�kowita (1 dla prawej strony, -1 dla lewej)
        velocity.x = (direction > 0) ? speed : -speed;
    }

    /// Operator przypisania, pozwalaj�cy na przypisanie jednego pocisku do drugiego.
    Projectile& operator=(const Projectile& other) {
        if (this != &other) {  /// Sprawdzanie, czy nie jest to przypisanie do siebie samego
            /// Przypisywanie atrybut�w pocisku
            texture = other.texture;
            sprite.setTexture(texture);
            sprite.setPosition(other.sprite.getPosition());
            sprite.setScale(other.sprite.getScale());
            velocity = other.velocity;
        }
        return *this;  /// Zwraca referencj� do obiektu, kt�ry jest obecnie przypisywany, umo�liwiaj�c przypisywanie �a�cuchowe
    }

    /// Aktualizuje pozycj� pocisku na podstawie jego pr�dko�ci i up�ywaj�cego czasu.
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
        sf::Vector2f position = sprite.getPosition();  /// Pobiera pozycj� pocisku
        /// Sprawdza, czy pocisk jest poza granicami okna
        return position.x < 0 || position.y < 0 || position.x > windowSize.x || position.y > windowSize.y;
    }

private:
    sf::Sprite sprite;  /// Sprite reprezentuj�cy pocisk na ekranie
    sf::Texture texture;  /// Tekstura sprite pocisku
    sf::Vector2f velocity;  /// Wektor pr�dko�ci pocisku
    const float speed = 990.0f;  /// Pr�dko�� pocisku w jednostkach na sekund�
};

///Klasa reprezentuj�ca posta� gracza w grze.
class Player
{
    int lastDirection = 1; /// Zmienna przechowuj�ca ostatni kierunek gracza, 1 oznacza prawo, -1 oznacza lewo
    vector<Projectile> projectiles; /// Wektor przechowuj�cy pociski wystrzelone przez gracza
    sf::Clock clock; /// Zegar, kt�ry jest u�ywany do obliczania czasu mi�dzy klatkami (deltaTime)
    sf::Clock shootTimer; /// Zegar u�ywany do kontrolowania pr�dko�ci strzelania gracza
    sf::Music shootSound; /// Muzyka odtwarzana, gdy gracz strzela
    const float shootDelay = 0.25f; /// Sta�a oznaczaj�ca op�nienie (w sekundach) mi�dzy strza�ami gracza

public:
    /// Konstruktory
    Player() /// Konstruktor bezparametrowy klasy Player
    {
        initVariables(); /// Inicjalizuje zmienne zwi�zane z graczem
        initTexture(); /// Inicjalizuje tekstur� gracza
        initSprite(); /// Inicjalizuje sprite gracza
        initAnimations(); /// Inicjalizuje animacje gracza
        initPhysics(); /// Inicjalizuje fizyk� gracza
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

    /// Zwraca wektor pocisk�w gracza
    const std::vector<Projectile>& getProjectiles() const {
        return projectiles;
    }

    /// Zmiana animacji
    bool getAnimSwitch()
    {
        /// Je�li animacja jest w��czona, wy��cza j� i zwraca prawd�.
        /// Je�li animacja jest wy��czona, zwraca fa�sz.
        if (animationSwitch)
        {
            animationSwitch = false;
            return true;
        }

        return false;
    }

    /// Zwraca pozycj� gracza
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
    void setPosition(const float x, const float y) /// Ustawia pozycj� sprite'u gracza
    {
        sprite.setPosition(x, y);
    }

    /// Reset pr�dko�ci w osi Y
    void resetVelocityY() /// Resetuje pr�dko�� gracza w osi Y do 0
    {
        velocity.y = 0.f;
    }

    /// Funkcje publiczne

    /* Sekcja Update */

    /// Resetowanie czasu animacji
    void resetAnimationTimer() /// Resetuje czas animacji i ustawia prze��cznik animacji na true
    {
        animationTimer.restart();
        animationSwitch = true;
    }

    /// Poruszanie si� gracza
    void move(const float dir_x, const float dir_y) /// Zmienia pr�dko�� gracza w zale�no�ci od kierunku i ogranicza j� do maksymalnej pr�dko�ci
    {
        /// Przyspieszenie
        velocity.x += acceleration * dir_x;

        /// Limit pr�dko�ci
        if (std::abs(velocity.x) > maxVelocity)
        {
            velocity.x = maxVelocity * ((velocity.x < 0.f) ? -1.f : 1.f);
        }
    }

    void jump() /// Wywo�uje skok gracza, je�li gracz nie skacze
    {
        if (!isJumping)
        {
            velocity.y = -sqrt(2.0f * gravity * jumpHeight);
            isJumping = true;
        }
    }

    void resetJump() /// Resetuje flag� skoku
    {
        isJumping = false;
    }

    /// Aktualizacja fizyki gracza
    void updatePhysics() /// Aktualizuje pr�dko�� gracza zgodnie z grawitacj� i tarcie, a nast�pnie porusza sprite gracza
    {
        /// Grawitacja
        ///velocity.y += gravity * 1.0;

        velocity.y += gravity;
        /// Limit grawitacji
        if (std::abs(velocity.x) > maxVelocityY)
        {
            velocity.y = maxVelocityY * ((velocity.y < 0.f) ? -1.f : 1.f);
        }

        /// Zmniejszenie pr�dko�ci
        velocity *= drag;

        /// Limit pr�dko�ci
        if (std::abs(velocity.x) < minVelocity)
            velocity.x = 0.f;
        if (std::abs(velocity.y) < minVelocity)
            velocity.y = 0.f;

        sprite.move(velocity);
    }

    /// Animacje poruszania si� gracza
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
        else if (animationState == PLAYER_ANIMATION_STATES::MOVING_RIGHT) /// Animacja poruszania si� w prawo
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
        else if (animationState == PLAYER_ANIMATION_STATES::MOVING_LEFT) /// Animacja poruszania si� w lewo
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

    ///metoda odpowiedzialna za wywo�anie strza�u
    void shoot()
    {
        if (shootTimer.getElapsedTime().asSeconds() >= shootDelay) /// Czas do strza�u
        {
            /// Obliczenie startowej pozycji pocisku
            sf::Vector2f startPosition = sprite.getPosition();
            startPosition.x += sprite.getGlobalBounds().width / 2;
            startPosition.y += sprite.getGlobalBounds().height / 2;

            /// Wystrzelenie pocisku w kierunku, w kt�rym gracz jest skierowany
            /// Przyjmuje si�, �e kierunek to 1 dla prawej strony i -1 dla lewej
            projectiles.push_back(Projectile(startPosition, this->lastDirection, "graphics/bullet_texture.png"));

            shootTimer.restart(); /// Reset czasu do strza�u
        }
    }

    /// Poruszanie si� gracza
    void updateMovement()
    {
        animationState = PLAYER_ANIMATION_STATES::IDLE; /// Ustawienie animacji na "stanie"
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) /// Je�eli naci�ni�ty jest klawisz "A"
        {
            move(-1.f, 0.f); /// Porusz graczem w lewo
            animationState = PLAYER_ANIMATION_STATES::MOVING_LEFT; /// Ustawienie animacji na "ruch w lewo"
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) /// Je�eli naci�ni�ty jest klawisz "D"
        {
            move(1.f, 0.f); /// Porusz graczem w prawo
            animationState = PLAYER_ANIMATION_STATES::MOVING_RIGHT; /// Ustawienie animacji na "ruch w prawo"
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) /// Je�eli naci�ni�ty jest klawisz "W"
        {
            jump(); /// Wywo�anie skoku
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) /// Je�eli naci�ni�ty jest klawisz "Space"
        {
            shoot(); /// Wywo�anie strza�u

            /// Odtwarzanie d�wi�ku strza�u
            if (!shootSound.openFromFile("music/shoot.wav")) {
                cout << "Failed to load shoot sound" << endl;
                return;
            }

            shootSound.setLoop(false); /// Ustawienie, �e d�wi�k nie powtarza si�
            shootSound.play(); /// Odtworzenie d�wi�ku
        }
    }

    void updateJumping()
    {
        static sf::Clock groundedTimer; /// Czas od ostatniego skoku
        static float lastY = sprite.getPosition().y; /// Ostatnia pozycja gracza w osi Y

        if (abs(lastY - sprite.getPosition().y) < 0.1f) /// Je�eli gracz nie porusza si� w g�r� ani w d�
        {
            /// Gracz prawdopodobnie jest na ziemi
            if (groundedTimer.getElapsedTime().asSeconds() > 0.1f) /// Je�eli od ostatniego skoku min�o wi�cej ni� 0.1 sekundy
            {
                isJumping = false; /// Ustawienie, �e gracz nie skacze
                groundedTimer.restart(); /// Reset czasu od ostatniego skoku
            }
        }
        else /// Gracz porusza si� w g�r� lub w d�
        {
            isJumping = true; /// Ustawienie, �e gracz skacze
            groundedTimer.restart(); /// Reset czasu od ostatniego skoku
        }

        lastY = sprite.getPosition().y; /// Zapisanie aktualnej pozycji gracza w osi Y
    }

    void updateLastDirection()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) /// Je�eli naci�ni�ty jest klawisz "A"
        {
            lastDirection = -1; /// Ustawienie kierunku gracza na lewo
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) /// Je�eli naci�ni�ty jest klawisz "D"
        {
            lastDirection = 1; /// Ustawienie kierunku gracza na prawo
        }
    }
    /// Update
    void update(sf::Vector2u windowSize)
    {
        sf::Time deltaTime = clock.restart(); /// Restart zegara

        /// Aktualizacja r�nych stan�w gracza
        updateJumping(); /// Aktualizacja skok�w
        updateMovement(); /// Aktualizacja ruchu
        updateAnimations(); /// Aktualizacja animacji
        updatePhysics(); /// Aktualizacja fizyki
        updateLastDirection(); /// Aktualizacja ostatniego kierunku

        /// Aktualizacja pocisk�w
        for (auto it = projectiles.begin(); it != projectiles.end(); ) {
            it->update(deltaTime); /// Aktualizacja czasu trwania ka�dego pocisku
            /// Je�li pocisk przekroczy� pewne granice lub spe�ni� inne warunki, mo�esz go usun��
            if (it->outOfBounds(windowSize)) { /// Je�eli pocisk wyjdzie poza okno gry
                it = projectiles.erase(it); /// Usuni�cie pocisku
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

        /// Rysowanie pocisk�w
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
    bool animationSwitch; /// Prze��cznik animacji
    PLAYER_ANIMATION_STATES animationState; /// Stan animacji

    sf::Vector2f velocity; /// Pr�dko�� gracza
    float maxVelocity; /// Maksymalna pr�dko��
    float minVelocity; /// Minimalna pr�dko��
    float acceleration; /// Przyspieszenie
    float drag; /// Op�r
    float gravity; /// Grawitacja
    float maxVelocityY; /// Maksymalna pr�dko�� w osi Y
    float jumpHeight; ///Maksymalna wysoko�� skoku

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
        currentFrame = sf::IntRect(0, 0, 32, 64); /// Ustalenie obszaru tekstury u�ywanego dla sprite'a
        sprite.setTextureRect(currentFrame); /// Przypisanie obszaru tekstury do sprite'a
        sprite.setScale(1.5, 1.7); /// Skalowanie sprite'a
    }

    /// Inicjalizacja tekstury
    void initTexture()
    {
        if (!textureSheet.loadFromFile("graphics/test_player.png")) /// Wczytanie tekstury
        {
            cout << "ERROR::PLAYER::INITTEXTURE::NIE ZALADOWALO TEXTURY GRACZA!" << "\n"; /// Wiadomo�� o b��dzie, je�li tekstura nie zostanie wczytana poprawnie
        }
    }

    /// Inicjalizacja animacji
    void initAnimations()
    {
        animationTimer.restart(); /// Restart timera animacji
        animationSwitch = true; /// Ustawienei prze��cznika animacji na true
    }

    /// Inicjalizacja fizyki
    void initPhysics()
    {
        maxVelocity = 10.f; /// Ustawienie maksymalnej pr�dko�ci
        minVelocity = 1.f; /// Ustawienie minimalnej pr�dko�ci
        acceleration = 3.f; /// Ustawienie przyspieszenia
        drag = 0.85f; /// Ustawienie oporu
        gravity = 4.f; /// Ustawienie grawitacji
        maxVelocityY = 15.f; /// Ustawienie maksymalnej pr�dko�ci w osi Y
        jumpHeight = 300.f; /// Ustawienie maksymalnej wysoko�ci skoku
    }
};

///Klasa TileMap Zajmuje si� wczytaniem mapy z pliku tmx.
class TileMap {
public:
    /// Konstruktor klasy TileMap - przyjmuje �cie�k� do pliku z map� i skal� mapy jako argumenty
    TileMap(const std::string& filePath, float scale = 1.0f)
        : filePath(filePath), scale(scale)
    {
        loadFromFile(filePath); /// Wczytanie mapy z pliku
    }

    void loadFromFile(const std::string& filePath)
    {
        map = tmx::Map(); /// Inicjalizacja mapy
        /// Czyszczenie zawarto�ci kontener�w przechowuj�cych dane mapy
        tilesetTextures.clear();
        tileLayers.clear();
        transitionRects.clear();
        collisionRects.clear();

        /// Wczytanie mapy z pliku
        if (!map.load(filePath))  /// u�yj argumentu filePath zamiast sta�ego stringa
        {
            /// Je�li nie uda si� wczyta� mapy, rzucenie wyj�tku
            throw std::runtime_error("Failed to load tile map: " + filePath);
        }

        /// Wczytanie tekstur tileset�w
        const auto& tilesets = map.getTilesets();
        for (const auto& tileset : tilesets) {
            sf::Texture texture;
            if (!texture.loadFromFile(tileset.getImagePath())) {
                /// Je�li nie uda si� wczyta� tekstury tilesetu, rzucenie wyj�tku
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
                /// zapisz warstw� do wektora kafelk�w
                tileLayers.push_back(tileLayer.getTiles());

                /// Przejd� przez wszystkie kafelki i dodaj prostok�ty kolizji dla kafelk�w o ID 4
                int width = map.getBounds().width / map.getTileSize().x;
                for (std::size_t i = 0; i < tileLayer.getTiles().size(); ++i) {
                    if (tileLayer.getTiles()[i].ID == 4) {  /// tylko kafelki o ID 4 s� kolizyjne
                        int tilePosX = i % width;
                        int tilePosY = i / width;
                        sf::FloatRect rect(static_cast<float>(tilePosX * map.getTileSize().x * scale),
                            static_cast<float>(tilePosY * map.getTileSize().y * scale),
                            map.getTileSize().x * scale, map.getTileSize().y * scale);
                        collisionRects.push_back(rect); /// Dodanie prostok�ta kolizji do wektora
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
                /// zapisz warstw� do wektora kafelk�w
                tileLayers.push_back(tileLayer.getTiles());

                /// Przejd� przez wszystkie kafelki i dodaj prostok�ty kolizji dla kafelk�w o ID 5
                int width = map.getBounds().width / map.getTileSize().x;
                for (std::size_t i = 0; i < tileLayer.getTiles().size(); ++i) {
                    if (tileLayer.getTiles()[i].ID == 5) {  /// tylko kafelki o ID 5 s� kolizyjne
                        int tilePosX = i % width;
                        int tilePosY = i / width;
                        sf::FloatRect rect(static_cast<float>(tilePosX * map.getTileSize().x * scale),
                            static_cast<float>(tilePosY * map.getTileSize().y * scale),
                            map.getTileSize().x * scale, map.getTileSize().y * scale);
                        collisionRects.push_back(rect); /// Dodanie prostok�ta kolizji do wektora
                    }
                }
            }
        }

    }

    /// Funkcja do ponownego wczytania mapy
    void reload()
    {
        cout << "Reload Function works" << endl; /// Wy�wietlenie informacji o rozpocz�ciu dzia�ania funkcji
        loadFromFile(filePath); /// Ponowne wczytanie mapy z pliku
        cout << "reload after loadFromFile" << endl; /// Wy�wietlenie informacji o zako�czeniu dzia�ania funkcji
    }

    /// Funkcja aktualizuj�ca logik� mapy (do zaimplementowania)
    void update(const sf::Time& deltaTime)
    {
        /// implementacja aktualizacji logiki mapy
    }

    /// Funkcja rysuj�ca map� na ekranie
    void draw(sf::RenderWindow& window)
    {
        /// Przechodzenie przez wszystkie warstwy kafelk�w
        for (std::size_t i = 0; i < tileLayers.size(); ++i) {
            const auto& tiles = tileLayers[i];

            /// Obliczanie szeroko�ci mapy w kaflach
            int width = map.getBounds().width / map.getTileSize().x;
            /// Przechodzenie przez wszystkie kafelki danej warstwy
            for (std::size_t j = 0; j < tiles.size(); ++j) {
                /// Je�li ID kafla wynosi 0, pomi� go
                if (tiles[j].ID == 0) continue;

                /// Obliczanie pozycji kafla na mapie
                int tilePosX = j % width;
                int tilePosY = j / width;

                /// Okre�lenie odpowiedniej tekstury dla kafla na podstawie jego gid
                int gid = tiles[j].ID;
                /// Zak�adamy, �e ka�dy tileset zawiera 256 kafelk�w, zmie� to zgodnie ze swoimi tilesetami
                int tilesetIndex = (gid - 1) / 256;

                /// Pobranie odpowiedniej tekstury
                const auto& texture = tilesetTextures[tilesetIndex];

                /// Stworzenie nowego sprite'a dla ka�dego kafla
                sf::Sprite sprite;
                sprite.setTexture(texture);

                /// Obliczenie pozycji kafla wewn�trz tekstury
                int textureTilesPerRow = texture.getSize().x / map.getTileSize().x; /// Zak�adamy, �e kafelki s� kwadratowe
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

    /// Funkcja zwracaj�ca warstw� kolizji
    const std::vector<tmx::TileLayer::Tile>& getCollisionLayer()
    {
        /// Je�li nie ma wystarczaj�cej ilo�ci warstw, rzucamy wyj�tek
        if (tileLayers.size() < 2) {
            throw std::runtime_error("No collision layer found.");
        }
        /// Zwracamy drug� warstw� jako warstw� kolizji
        return tileLayers[1];
    }

    /// Funkcja zwracaj�ca prostok�ty kolizji
    const std::vector<sf::FloatRect>& getCollisionRects() const {
        return collisionRects;
    }

    /// Funkcja zwracaj�ca prostok�ty przej�cia
    const std::vector<sf::FloatRect>& getTransitionRects() const {
        return transitionRects;
    }

private:
    /// Prywatne pola klasy
    std::string filePath; /// �cie�ka do pliku z map�
    tmx::Map map; /// obiekt mapy
    std::vector<sf::Texture> tilesetTextures; /// tekstury tileset�w
    std::vector<std::vector<tmx::TileLayer::Tile>> tileLayers; /// warstwy kafelk�w
    vector<sf::FloatRect> transitionRects; /// prostok�ty przej�cia
    std::vector<sf::FloatRect> collisionRects; /// prostok�ty kolizji
    float scale; /// skala mapy
};

class SceneManager;

/// Klasa Scene reprezentuje jedn� scen� w grze (np. menu, ekran gry, ekran ko�ca gry).
class Scene {
public:
    /// Konstruktor przyjmuje wska�nik do okna, w kt�rym scena b�dzie wy�wietlana.
    Scene(sf::RenderWindow* window) : window(window) { }

    /// Wirtualny destruktor - potrzebny, aby kiedykolwiek dziedziczymy z klasy z metodami wirtualnymi,
    /// destruktor powinien by� r�wnie� wirtualny. Pozwala to na poprawne zwolnienie zasob�w
    /// kiedy obiekt jest usuwany przez wska�nik do bazowej klasy.
    virtual ~Scene() = default;

    /// Metoda do ustawiania menad�era scen - u�ywana do zmiany scen w grze.
    void SetManager(SceneManager* manager) { this->manager = manager; }

    /// Czysto wirtualna metoda Run - ka�da scena musi j� zaimplementowa�.
    /// Definiuje logik�, kt�ra ma by� wykonana podczas uruchamiania sceny.
    virtual void Run() = 0;

protected:
    /// Wska�nik na okno, na kt�rym scena jest wy�wietlana.
    sf::RenderWindow* window;

    /// Wska�nik na menad�era scen, kt�ry zarz�dza t� scen�.
    SceneManager* manager;
};

/// Klasa SceneManager zarz�dza r�nymi scenami w grze.
class SceneManager {
public:
    /// Konstruktor przyjmuje wska�nik na okno, w kt�rym b�d� wy�wietlane sceny.
    SceneManager(sf::RenderWindow* window) : window(window) { }

    /// Metoda Run uruchamia now� scen�. Jest to metoda szablonowa, kt�ra przyjmuje typ sceny T jako pierwszy argument,
    /// a nast�pnie dowoln� liczb� argument�w (Args), kt�re s� przekazywane do konstruktora sceny T.
    template <typename T, typename... Args>
    void Run(Args&&... args) {
        scene = std::make_unique<T>(window, std::forward<Args>(args)...); /// tworzy now� scen�
        scene->SetManager(this); /// ustawia ten menad�er scen jako menad�er dla nowo utworzonej sceny
        scene->Run(); /// uruchamia nowo utworzon� scen�
    }

    /// Metoda SwitchScene s�u�y do prze��czania na inn� scen�. Jest to metoda szablonowa, kt�ra przyjmuje typ sceny T jako pierwszy argument,
    /// a nast�pnie dowoln� liczb� argument�w (Args), kt�re s� przekazywane do konstruktora sceny T.
    template <typename T, typename... Args>
    void SwitchScene(Args&&... args) {
        scene = std::make_unique<T>(window, std::forward<Args>(args)...); /// tworzy now� scen�
        scene->SetManager(this); /// ustawia ten menad�er scen jako menad�er dla nowo utworzonej sceny
    }

private:
    /// Wska�nik na aktualnie aktywn� scen�.
    std::unique_ptr<Scene> scene;

    /// Wska�nik na okno, na kt�rym sceny s� wy�wietlane.
    sf::RenderWindow* window;
};

class Menu;

class VictoryScene;

class GameOver;

/// Klasa Enemy reprezentuje wroga w grze.
class Enemy {
public:
    /// Konstruktor tworzy wroga na okre�lonej pozycji startowej i z okre�lon� tekstur�.
    Enemy(const sf::Vector2f& startPos, const std::string& textureFile) {
        if (!texture.loadFromFile("graphics/enemy.png")) {
            cout << "ERROR::ENEMY::Nie uda�o si� za�adowa� tekstury." << "\n";
        }
        sprite.setTexture(texture);
        sprite.setPosition(startPos);
        sprite.setScale(0.25, 0.3);

        initPhysics();  /// Inicjalizuje parametry fizyki dla wroga.
    }

    /// Ta metoda inicjalizuje parametry fizyki dla wroga.
    void initPhysics() {
        maxVelocity = 1.f;  /// Maksymalna pr�dko��.
        minVelocity = 1.f;  /// Minimalna pr�dko��.
        acceleration = 3.f;  /// Przyspieszenie.
        drag = 0.85f;  /// Op�r.
        gravity = 4.f;  /// Si�a grawitacji.
        maxVelocityY = 15.f;  /// Maksymalna pr�dko�� w osi Y.
    }

    /// Ta metoda resetuje pr�dko�� w osi Y do 0.
    void resetVelocityY() {
        velocity.y = 0.f;
    }

    /// Ta metoda aktualizuje stan wroga, bior�c pod uwag� pozycj� gracza i delta czasu.
    void update(const sf::Vector2f& playerPosition, sf::Time deltaTime) {
        /// Aplikowanie grawitacji na pr�dko�� w osi Y.
        velocity.y += gravity;

        /// Ograniczenie pr�dko�ci do maxVelocityY.
        if (std::abs(velocity.y) > maxVelocityY)
        {
            velocity.y = maxVelocityY * ((velocity.y < 0.f) ? -1.f : 1.f);
        }

        /// Zastosowanie oporu do pr�dko�ci.
        velocity *= drag;

        /// Ustalenie pr�dko�ci na 0, je�li jest poni�ej minVelocity.
        if (std::abs(velocity.x) < minVelocity)
            velocity.x = 0.f;
        if (std::abs(velocity.y) < minVelocity)
            velocity.y = 0.f;

        sprite.move(velocity);  /// Przesuni�cie wroga na podstawie pr�dko�ci.

        /// Obliczenie kierunku i dystansu do gracza.
        sf::Vector2f direction = playerPosition - sprite.getPosition();
        float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));

        /// Je�li gracz jest w zasi�gu 200px, wroga kieruje si� w stron� gracza.
        if (distance < 250.0f) {
            direction /= distance;  /// Normalizacja wektora kierunku.
            sprite.move(direction * speed * deltaTime.asSeconds());  /// Przesuni�cie wroga w stron� gracza.
        }
    }

    /// Metoda rysuj�ca wroga na ekranie.
    void render(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    /// Metoda sprawdzaj�ca, czy wrogi kolizje z graczem.
    bool checkCollision(const sf::Sprite& playerSprite) {
        return sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds());
    }

    /// Metoda sprawdzaj�ca, czy wroga zosta� trafiony przez jakikolwiek pocisk.
    bool checkProjectileHit(const std::vector<Projectile>& projectiles) {
        for (auto& projectile : projectiles) {
            if (sprite.getGlobalBounds().intersects(projectile.getSprite().getGlobalBounds())) {
                return true;
            }
        }
        return false;
    }

    /// Metoda sprawdzaj�ca, czy wroga kolizjonuje z jakimkolwiek kafelkiem mapy.
    bool checkTileCollision(const TileMap& map) {
        auto collisionRects = map.getCollisionRects();
        for (const auto& rect : collisionRects) {
            if (sprite.getGlobalBounds().intersects(rect)) {
                return true;
            }
        }
        return false;
    }

    /// Metoda zwracaj�ca sprite wroga.
    sf::Sprite& getSprite() {
        return sprite;
    }

    /// Metoda zwracaj�ca aktualn� pozycj� wroga.
    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

private:
    sf::Sprite sprite;  /// Sprite reprezentuj�cy wroga.
    sf::Texture texture;  /// Tekstura wroga.
    const float speed = 200.0f;  /// Pr�dko�� wroga.

    sf::Vector2f velocity;  /// Aktualna pr�dko�� wroga.
    float maxVelocity;  /// Maksymalna pr�dko�� wroga.
    float minVelocity;  /// Minimalna pr�dko�� wroga.
    float acceleration;  /// Przyspieszenie wroga.
    float drag;  /// Op�r dzia�aj�cy na wroga.
    float gravity;  /// Si�a grawitacji.
    float maxVelocityY;  /// Maksymalna pr�dko�� w osi Y.
};

/// Klasa Finish reprezentuje miejsce ko�ca (cel) w grze.
class Finish {
public:
    /// Konstruktor tworzy miejsce ko�ca na okre�lonej pozycji startowej i z okre�lon� tekstur�.
    Finish(const sf::Vector2f& startPos, const string& textureFile) {
        /// �aduje tekstur� z pliku.
        if (!texture.loadFromFile("graphics/spaceship.png")) {
            cout << "ERROR::FINISH::Nie uda�o si� za�adowa� tekstury statku finish" << endl;
        }

        /// Ustawia tekstur� dla sprite'a.
        sprite.setTexture(texture);
        /// Ustawia pocz�tkow� pozycj� sprite'a.
        sprite.setPosition(startPos);
        /// Ustawia skal� sprite'a.
        sprite.setScale(1, 1);
    }

    /// Metoda do renderowania (rysowania) miejsca ko�ca na ekranie.
    void render(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    /// Metoda sprawdzaj�ca, czy dosz�o do kolizji z graczem.
    bool checkCollision(const sf::Sprite& playerSprite) {
        return sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds());
    }

    /// Metoda zwracaj�ca sprite miejsca ko�ca.
    sf::Sprite& getSprite() {
        return sprite;
    }

private:
    /// Sprite reprezentuj�cy miejsce ko�ca.
    sf::Sprite sprite;
    /// Tekstura miejsca ko�ca.
    sf::Texture texture;
};

///Klasa Space odpowiada za granic� mapy (otch�a� kosmosu) w kt�r� jak wpadniemy, przegrywamy i �api� nas kosmici.
class Space {
public:
    /// Konstruktor klasy Space
    Space(const sf::Vector2f& startPos, const string& textureFile) {
        /// Za�adowanie tekstury obiektu pr�ni
        if (!texture.loadFromFile("graphics/space.png")) {
            cout << "ERROR::SPACE::Nie uda�o si� za�adowa� tekstury obiektu pr�ni!" << endl;
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

    /// Sprawdzanie kolizji mi�dzy obiektem pr�ni a sprite'em gracza
    bool checkCollision(const sf::Sprite& playerSprite) {
        return sprite.getGlobalBounds().intersects(playerSprite.getGlobalBounds());
    }

    /// Pobieranie referencji do sprite'a
    sf::Sprite& getSprite() {
        return sprite;
    }

private:
    sf::Sprite sprite;      /// Sprite obiektu pr�ni
    sf::Texture texture;    /// Tekstura obiektu pr�ni
};

///Klasa Game odpowiada za ca�� logik� gry.
class Game : public Scene {
public:
    /// Metoda zwracaj�ca wska�nik do okna
    sf::RenderWindow* getWindow() {
        return window;
    }

    /// Metoda zwracaj�ca pozycj� gracza
    sf::Vector2f getPlayerPosition() {
        return player->getPosition();
    }

    /// Konstruktor klasy Game, inicjalizuje zmienne, okno, gracza, przeciwnik�w, met�, obszar kosmiczny i muzyk�
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

    /// Destruktor klasy Game, zwalnia zaalokowan� pami�� dla okna, gracza, przeciwnik�w, mety i obszaru kosmicznego
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

    /// Metoda zwracaj�ca informacj�, czy gra jest uruchomiona (okno jest otwarte)
    const bool running() const {
        return window->isOpen();
    }

    /// Metoda wykonuj�ca g��wn� p�tl� gry
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
    sf::RenderWindow* window;       /// Wska�nik do obiektu okna
    sf::VideoMode videoMode;        /// Obiekt przechowuj�cy ustawienia trybu wideo
    sf::Event event;                /// Obiekt przechowuj�cy zdarzenia
    Player* player;                 /// Wska�nik do obiektu gracza
    Finish* finish;                 /// Wska�nik do obiektu mety
    Space* space;                   /// Wska�nik do obiektu obszaru kosmicznego
    vector<Enemy*> enemies;         /// Wektor wska�nik�w do obiekt�w przeciwnik�w
    vector<Finish*> finishes;       /// Wektor wska�nik�w do obiekt�w mety (dla obs�ugi wielu met)
    vector<Space*> spaces;          /// Wektor wska�nik�w do obiekt�w obszaru kosmicznego
    unique_ptr<TileMap> tileMap;    /// Unikalny wska�nik do obiektu mapy kafelk�w
    sf::Clock globalClock;          /// Zegar globalny

    sf::Music startMissionMusic;    /// Muzyka rozpocz�cia misji
    sf::Music enemyDeath;           /// D�wi�k �mierci przeciwnika
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

    /// Inicjalizacja przeciwnik�w
    void initEnemies() {
        /// Dodawanie przeciwnik�w do wektora enemies
        /// Ustawienie pozycji przeciwnik�w wzgl�dem pozycji gracza
        /// U�ywanie odpowiednich tekstur dla przeciwnik�w
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
        /// Dodawanie pozosta�ych przeciwnik�w
    }

    /// Inicjalizacja mety
    void initFinish() {
        /// Dodawanie mety do wektora finishes
        /// Ustawienie pozycji mety wzgl�dem pozycji gracza
        /// U�ywanie odpowiedniej tekstury dla mety
        finishes.push_back(new Finish(sf::Vector2f(player->getPosition().x + 4200, player->getPosition().y + 150), "finishTextureFile"));
        /// ...
        /// Dodawanie pozosta�ych met
    }

    /// Inicjalizacja obszaru kosmicznego
    void initSpace() {
        /// Dodawanie obszaru kosmicznego do wektora spaces
        /// Ustawienie pozycji obszaru kosmicznego wzgl�dem pozycji gracza
        /// U�ywanie odpowiedniej tekstury dla obszaru kosmicznego
        spaces.push_back(new Space(sf::Vector2f(player->getPosition().x - 1000, player->getPosition().y + 900), "SpaceTextureFile"));
    }

    /// Inicjalizacja mapy kafelk�w
    void initTileMap() {
        /// Inicjalizacja obiektu tileMap za pomoc� unikalnego wska�nika
        /// Przekazanie �cie�ki do pliku mapy kafelk�w i skali mapy
        tileMap = unique_ptr<TileMap>(new TileMap("tiled/map_1.tmx", 3.0f));
    }

    /// Inicjalizacja muzyki rozpocz�cia misji
    void initStartMusic() {
        /// Otwieranie pliku d�wi�kowego i sprawdzanie, czy si� uda�o
        /// Je�li nie uda�o si�, wy�wietlanie komunikatu o b��dzie
        if (!startMissionMusic.openFromFile("music/start_mission.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        startMissionMusic.setLoop(false);
        startMissionMusic.play();
    }

    /// Inicjalizacja muzyki w tle
    void initMusic() {
        /// Otwieranie pliku d�wi�kowego i sprawdzanie, czy si� uda�o
        /// Je�li nie uda�o si�, wy�wietlanie komunikatu o b��dzie
        if (!backgroundMusic.openFromFile("music/mission.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        backgroundMusic.setLoop(true);  /// Ustawienie zap�tlenia muzyki
        backgroundMusic.play();         /// Odtwarzanie muzyki
    }

    /// Obs�uga zdarze�, takich jak zamkni�cie okna lub wci�ni�cie klawisza
    void pollEvents() {
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();       /// Zamkni�cie okna
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    window->close();   /// Zamkni�cie okna
                    manager->Run<Menu>();  /// Prze��czenie na scen� menu
                }
                if (event.key.code == sf::Keyboard::R) {
                    cout << "Map just got reloaded" << endl;    /// Wy�wietlenie komunikatu o prze�adowaniu mapy
                    tileMap->reload();  /// Prze�adowanie mapy kafelk�w
                }
                //Poni�ej przyciski Testowe!!
                if (event.key.code == sf::Keyboard::V) {
                    cout << "Victory screen just got loaded" << endl;   /// Wy�wietlenie komunikatu o za�adowaniu ekranu zwyci�stwa
                    window->close();
                    manager->Run<VictoryScene>();   /// Prze��czenie na scen� zwyci�stwa
                }
                if (event.key.code == sf::Keyboard::G) {
                    cout << "GAMEOVER screen just got loaded" << endl;   /// Wy�wietlenie komunikatu o za�adowaniu ekranu zako�czenia gry
                    window->close();
                    manager->Run<GameOver>();   /// Prze��czenie na scen� zako�czenia gry
                }
                break;
            }
        }
    }

    /// Aktualizacja stanu gracza
    void updatePlayer() {
        player->update(window->getSize());   /// Wywo�anie metody update() na obiekcie gracza, przekazanie rozmiaru okna jako argument
    }

    /// Aktualizacja stanu przeciwnik�w
    void updateEnemies(sf::Time duration) {
        for (auto& enemy : enemies) {
            /// Zapami�taj poprzedni� pozycj� wroga przed aktualizacj�
            sf::Vector2f oldPosition = enemy->getSprite().getPosition();

            /// Wykonaj aktualizacj�, kt�ra potencjalnie przesuwa wroga w kierunku gracza
            enemy->update(player->getPosition(), duration);

            sf::FloatRect enemyBounds = enemy->getSprite().getGlobalBounds();

            for (const auto& rect : tileMap->getCollisionRects()) {
                if (enemyBounds.intersects(rect)) {
                    /// Wr�g koliduje z jednym z kafelk�w. Wykonaj odpowiedni� akcj�, np.:

                    enemy->resetVelocityY();
                    enemy->getSprite().setPosition(enemy->getSprite().getPosition().x, rect.top - enemy->getSprite().getGlobalBounds().height);

                    bool isEnemyLeftOfPlayer = enemy->getSprite().getPosition().x < player->getPosition().x;

                    if (isEnemyLeftOfPlayer) {
                        /// Je�eli wr�g jest na lewo od gracza, pozw�l mu si� porusza� w prawo (czyli do gracza)
                        /// ale nie w lewo (z dala od gracza)
                        if (enemy->getSprite().getPosition().x < oldPosition.x) {
                            enemy->getSprite().setPosition(oldPosition.x, enemy->getSprite().getPosition().y);
                        }
                    }
                    else {
                        /// Je�eli wr�g jest na prawo od gracza, pozw�l mu si� porusza� w lewo (czyli do gracza)
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
                cout << "Gracz dotar� do mety!" << endl;
                window->close();
                manager->Run<VictoryScene>();
            }
        }
    }

    /// Aktualizacja stanu obszaru kosmicznego
    void updateSpace() {
        for (auto& space : spaces) {
            if (space->checkCollision(player->getSprite())) {
                cout << "Gracz wpad� w kosmos!" << endl;
                window->close();
                manager->Run<GameOver>();
            }
        }
    }

    /// Aktualizacja kolizji gracza z kafelkami i przej�ciami
    void updateCollision() {
        sf::FloatRect playerBounds = player->getGlobalBounds();

        /// Sprawdzenie kolizji z kafelkami
        for (const auto& rect : tileMap->getCollisionRects()) {
            if (playerBounds.intersects(rect)) {
                /// Gracz koliduje z jednym z kafelk�w. Wykonaj odpowiedni� akcj�, np.:
                player->resetVelocityY();
                player->setPosition(player->getPosition().x, rect.top - player->getGlobalBounds().height);
                break;
            }
        }

        /// Sprawdzenie kolizji z przej�ciami
        for (const auto& rect : tileMap->getTransitionRects()) {
            if (playerBounds.intersects(rect)) {
                /// Gracz wpad� na przej�cie. Zmie� okno na inne.
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
                /// Sprawdzanie, czy gracz jest powy�ej wroga
                if (player->getPosition().y < (*it)->getPosition().y) {
                    if (!enemyDeath.openFromFile("music/enemy.wav")) {
                        cout << "Failed to load background music" << endl;
                        return;
                    }

                    enemyDeath.setLoop(false);
                    enemyDeath.play();
                    /// Usuwanie wroga
                    cout << "Gracz pokona� wroga skacz�c mu po g�owie!" << endl;
                    delete* it; /// Zwalnianie pami�ci
                    it = enemies.erase(it); /// Usuwanie elementu z wektora i aktualizacja iteratora
                }
                else {
                    /// Wykryto kolizj� mi�dzy graczem a wrogem!
                    cout << "Gracz koliduje z wrogiem!" << endl;
                    window->close();
                    manager->Run<GameOver>();
                    return; /// Przerwanie p�tli, poniewa� gra zosta�a zako�czona
                }
            }
            else {
                ++it; /// Przechodzenie do nast�pnego wroga
            }
        }
    }

    /// Sprawdzenie kolizji pocisk�w gracza z przeciwnikami
    void checkProjectileEnemyCollisions() {
        std::vector<Projectile*> projectilesToRemove;

        /// P�tla po wszystkich wrogach
        for (auto& enemy : enemies) {
            /// Sprawdzanie kolizji dla ka�dego pocisku gracza
            for (auto& projectile : player->getProjectiles()) {
                if (enemy->getSprite().getGlobalBounds().intersects(projectile.getSprite().getGlobalBounds())) {
                    /// Wykryto kolizj� mi�dzy pociskiem a wrogiem!
                    /// Zdecyduj, co ma si� zdarzy� w tej sytuacji...
                    cout << "Pocisk trafi� wroga!" << endl;
                }
            }
        }
    }

    /// Aktualizacja widoku kamery
    void updateView() {
        sf::View view = window->getView(); /// Pobierz bie��cy widok
        sf::Vector2f playerPosition = getPlayerPosition(); /// Pobierz aktualn� pozycj� gracza

        /// Zmienne przechowuj�ce wielko�� okna
        float windowHeight = static_cast<float>(window->getSize().y);
        float windowWidth = static_cast<float>(window->getSize().x);

        /// Zmienne przechowuj�ce wielko�� mapy
        float mapHeight = 900; /// Wysoko�� mapy
        float mapWidth = 4500; /// Szeroko�� mapy

        /// Ustal �rodek widoku na pozycj� gracza, ale nie przesuwaj widoku poza granice mapy
        playerPosition.x = std::max(playerPosition.x, windowWidth / 2);
        playerPosition.y = std::max(playerPosition.y, windowHeight / 2);
        playerPosition.x = std::min(playerPosition.x, mapWidth - windowWidth / 2);
        playerPosition.y = std::min(playerPosition.y, mapHeight - windowHeight / 2);

        /// Aktualizuj �rodek widoku na pozycj� gracza
        view.setCenter(playerPosition);

        /// Zastosuj nowy widok do okna
        window->setView(view);
    }

    /// Aktualizacja gry
    void update() {
        pollEvents();

        /// Resetuj animacj� gracza po zwolnieniu klawiszy A, D lub W
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

    /// Rysowanie przeciwnik�w
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

///Klasa VictoryScene odpowiada za scen� zwyci�stwa i wygrania gry.
class VictoryScene : public Scene {
public:
    /// Konstruktor klasy VictoryScene, inicjalizuje okno i elementy sceny
    VictoryScene(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initVictoryMusic();
    }

    /// Destruktor klasy VictoryScene, zwalnia zaalokowan� pami�� dla tekstur przycisk�w
    ~VictoryScene() {
        delete backgroundTexture;
        delete nextButtonTexture;
        delete menuButtonTexture;
    }

    /// Metoda wykonuj�ca g��wn� p�tl� sceny Victory
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;            /// Wska�nik do obiektu okna
    sf::Texture* backgroundTexture;      /// Wska�nik do tekstury t�a
    sf::Sprite backgroundSprite;          /// Sprite t�a
    sf::Texture* nextButtonTexture;       /// Wska�nik do tekstury przycisku "Dalej"
    sf::RectangleShape nextButton;        /// Prostok�tny kszta�t przycisku "Dalej"
    sf::Texture* menuButtonTexture;       /// Wska�nik do tekstury przycisku "Menu"
    sf::RectangleShape menuButton;        /// Prostok�tny kszta�t przycisku "Menu"
    sf::Music victorySound;               /// D�wi�k zwyci�stwa

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "Victory");
    }

    /// Inicjalizacja t�a
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

    /// Inicjalizacja przycisk�w
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

    /// Inicjalizacja d�wi�ku zwyci�stwa
    void initVictoryMusic() {
        if (!victorySound.openFromFile("music/victory.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        victorySound.setLoop(true);
        victorySound.play();
    }

    /// Reakcja na zdarzenia, takie jak zamkni�cie okna lub klikni�cie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Dalej" zosta� klikni�ty
                    if (nextButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "next Button pressed!" << endl;
                        window->close();
                        /// manager->Run<Menu>();  /// Odkomentuj t� lini� dla aktualizacji 0.2 alfa
                    }
                    /// Sprawdzenie, czy przycisk "Menu" zosta� klikni�ty
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

///Klasa GameOver odpowiada za scen� pora�ki i przegrania gry.
class GameOver : public Scene {
public:
    /// Konstruktor klasy GameOver, inicjalizuje okno i elementy sceny
    GameOver(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initDeathSound();
    }

    /// Destruktor klasy GameOver, zwalnia zaalokowan� pami�� dla tekstur przycisk�w
    ~GameOver() {
        delete backgroundTexture;
        delete restartButtonTexture;
        delete menuButtonTexture;
    }

    /// Metoda wykonuj�ca g��wn� p�tl� sceny GameOver
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;            /// Wska�nik do obiektu okna
    sf::Texture* backgroundTexture;      /// Wska�nik do tekstury t�a
    sf::Sprite backgroundSprite;          /// Sprite t�a
    sf::Texture* restartButtonTexture;    /// Wska�nik do tekstury przycisku "Restart"
    sf::RectangleShape restartButton;     /// Prostok�tny kszta�t przycisku "Restart"
    sf::Texture* menuButtonTexture;       /// Wska�nik do tekstury przycisku "Menu"
    sf::RectangleShape menuButton;        /// Prostok�tny kszta�t przycisku "Menu"
    sf::Music deathSound;                 /// D�wi�k �mierci

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "GAME OVER");
    }

    /// Inicjalizacja t�a
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

    /// Inicjalizacja przycisk�w
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

    /// Inicjalizacja d�wi�ku �mierci
    void initDeathSound() {
        if (!deathSound.openFromFile("music/death_sound.wav")) {
            cout << "Failed to load background music" << endl;
            return;
        }

        deathSound.setLoop(false);
        deathSound.play();
    }

    /// Reakcja na zdarzenia, takie jak zamkni�cie okna lub klikni�cie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Restart" zosta� klikni�ty
                    if (restartButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        cout << "restart Button pressed!" << endl;
                        window->close();
                        manager->Run<Game>();
                    }
                    /// Sprawdzenie, czy przycisk "Menu" zosta� klikni�ty
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

///Klasa KeyboardOption odpowiada za scen� klawiszologii (opisanie poruszania si� w grze).
class KeyboardOption : public Scene {
public:
    /// Konstruktor klasy KeyboardOption, inicjalizuje okno i elementy sceny
    KeyboardOption(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initMusic();
    }

    /// Destruktor klasy KeyboardOption, zwalnia zaalokowan� pami�� dla tekstur i okna
    ~KeyboardOption() {
        delete backgroundTexture;
        delete menuButtonTexture;
        delete window;
    }

    /// Metoda wykonuj�ca g��wn� p�tl� sceny KeyboardOption
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;            /// Wska�nik do obiektu okna
    sf::Texture* backgroundTexture;      /// Wska�nik do tekstury t�a
    sf::Sprite backgroundSprite;          /// Sprite t�a
    sf::Texture* menuButtonTexture;       /// Wska�nik do tekstury przycisku "Menu"
    sf::RectangleShape menuButton;        /// Prostok�tny kszta�t przycisku "Menu"
    sf::Music backgroundMusic;            /// Muzyka w tle

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "KLAWISZOLOGIA");
    }

    /// Inicjalizacja t�a
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

    /// Reakcja na zdarzenia, takie jak zamkni�cie okna lub klikni�cie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Menu" zosta� klikni�ty
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

///Klasa Menu odpowiada za scen� g��wnego Menu gry - wybieramy w niej START - start gry || KLAWISZE - opcje sterowania || EXIT - wyj�cie z gry i zamkni�cie programu
class Menu : public Scene {
public:
    /// Konstruktor klasy Menu, inicjalizuje okno i elementy sceny
    Menu(sf::RenderWindow* window) : Scene(window) {
        initWindow();
        initBackground();
        initButtons();
        initMusic();
    }

    /// Metoda wykonuj�ca g��wn� p�tl� sceny Menu
    void Run() override {
        while (window->isOpen()) {
            pollEvents();
            render();
        }
    }

private:
    sf::RenderWindow* window;                   /// Wska�nik do obiektu okna
    std::unique_ptr<sf::Texture> backgroundTexture;       /// Unikalny wska�nik do tekstury t�a
    sf::Sprite backgroundSprite;                       /// Sprite t�a
    std::unique_ptr<sf::Texture> startButtonTexture;     /// Unikalny wska�nik do tekstury przycisku "Start"
    sf::RectangleShape startButton;                     /// Prostok�tny kszta�t przycisku "Start"
    std::unique_ptr<sf::Texture> exitButtonTexture;      /// Unikalny wska�nik do tekstury przycisku "Exit"
    sf::RectangleShape exitButton;                       /// Prostok�tny kszta�t przycisku "Exit"
    std::unique_ptr<sf::Texture> KeyboardButtonTexture;  /// Unikalny wska�nik do tekstury przycisku "Keyboard Option"
    sf::RectangleShape KeyboardButton;                   /// Prostok�tny kszta�t przycisku "Keyboard Option"

    sf::Music backgroundMusic;                /// Muzyka w tle

    bool startButtonClicked;                   /// Flaga informuj�ca, czy przycisk "Start" zosta� klikni�ty

    /// Inicjalizacja okna
    void initWindow() {
        window = new sf::RenderWindow(sf::VideoMode(800, 600), "Menu");
    }

    /// Inicjalizacja t�a
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

    /// Inicjalizacja przycisk�w
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

    /// Reakcja na zdarzenia, takie jak zamkni�cie okna lub klikni�cie przycisku
    void pollEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /// Sprawdzenie, czy przycisk "Start" zosta� klikni�ty
                    if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "Start button pressed!" << std::endl;
                        window->close();
                        manager->Run<Game>(); /// Prze��cz na scen� gry
                    }
                    /// Sprawdzenie, czy przycisk "Exit" zosta� klikni�ty
                    else if (exitButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "Exit button pressed!" << std::endl;
                        window->close();
                    }
                    /// Sprawdzenie, czy przycisk "Keyboard Option" zosta� klikni�ty
                    else if (KeyboardButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "Keyboard Option button pressed!" << std::endl;
                        window->close();
                        manager->Run<KeyboardOption>();

                        /// Tutaj mo�esz doda� kod do przej�cia do ekranu wyboru poziomu
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

/// G��wna funkcja programu
int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Game"); /// Inicjalizacja okna gry o rozmiarze 800x600 pikseli
    SceneManager manager(&window); /// Inicjalizacja mened�era scen, kt�ry zarz�dza prze��czaniem mi�dzy scenami

    manager.Run<Menu>(); /// Uruchomienie gry, zaczynaj�c od sceny Menu

    return 0; /// Zako�czenie programu
}