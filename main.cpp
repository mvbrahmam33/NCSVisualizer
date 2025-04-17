#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include "PerlinNoise.hpp"

const int GRID_SIZE = 220; // Reduced for performance
const float DOT_SPACING = 0.9f;
const float DOT_OFFSET = -DOT_SPACING / 2.0f;
const float SPHERE_RADIUS = 0.9f;
const float FEATHER = 0.015f;
const float NOISE_FREQ = 4.0f;
const float NOISE_AMP = 0.32f * 0.9f;

sf::Vector2f toScreen(float x, float y, float width, float height) {
    return sf::Vector2f((x * 0.5f + 0.5f) * width, (y * 0.5f + 0.5f) * height);
}

float calculateRMS(const sf::Int16* samples, std::size_t start, std::size_t count) {
    double sum = 0.0;
    for (std::size_t i = start; i < start + count; ++i) {
        sum += samples[i] * samples[i];
    }
    return static_cast<float>(std::sqrt(sum / count) / 32768.0f);
}

int main() {
    const int WIDTH = 800, HEIGHT = 800;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Optimized NCS Visualizer");
    window.setFramerateLimit(60);
    sf::Clock clock;

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("faded.wav")) {
        std::cerr << "Failed to load audio file.\n";
        return -1;
    }

    const sf::Int16* samples = buffer.getSamples();
    std::size_t sampleCount = buffer.getSampleCount();
    unsigned int sampleRate = buffer.getSampleRate();
    unsigned int channelCount = buffer.getChannelCount();

    sf::Sound music;
    music.setBuffer(buffer);
    music.setLoop(true);
    music.play();

    const unsigned int seed = 42;
    siv::PerlinNoise perlin(seed);

    sf::VertexArray dots(sf::Points);

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        float t = clock.getElapsedTime().asSeconds();
        float noiseOffset = t * 0.75f;

        // Audio amplitude
        sf::Time currentTime = music.getPlayingOffset();
        std::size_t currentSample = static_cast<std::size_t>(currentTime.asSeconds() * sampleRate * channelCount);
        std::size_t windowSize = sampleRate * channelCount / 10;
        float amplitude = calculateRMS(samples, currentSample, windowSize);
        float ampScaled = 6.0f * amplitude;

        dots.clear();

        const float featherRadius = SPHERE_RADIUS - FEATHER;

        for (int y = 0; y < GRID_SIZE; ++y) {
            for (int x = 0; x < GRID_SIZE; ++x) {
                float u = static_cast<float>(x) / GRID_SIZE;
                float v = static_cast<float>(y) / GRID_SIZE;

                float n = perlin.noise3D(u * NOISE_FREQ, v * NOISE_FREQ, noiseOffset) * NOISE_AMP;

                float px = u * DOT_SPACING + DOT_OFFSET + n;
                float py = v * DOT_SPACING + DOT_OFFSET + n;
                float pz = (n + 0.5f * NOISE_AMP) * ampScaled * 0.4f;

                sf::Vector3f pos(px, py, pz);
                float len = std::sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
                if (len == 0) continue;

                pos.x /= len;
                pos.y /= len;
                pos.z /= len;

                float radius = std::min(SPHERE_RADIUS, len);
                pos *= radius;

                float featherStrength = 1.0f - std::clamp((radius - featherRadius) / FEATHER, 0.0f, 1.0f);
                float scale = featherStrength * (SPHERE_RADIUS / radius - 1.0f) + 1.0f;

                pos.x *= scale;
                pos.y *= -scale;

                sf::Vector2f screenPos = toScreen(pos.x, pos.y, WIDTH, HEIGHT);

                if (screenPos.x < 0 || screenPos.x > WIDTH || screenPos.y < 0 || screenPos.y > HEIGHT)
                    continue;

                sf::Vertex vertex;
                vertex.position = screenPos;
                vertex.color = sf::Color(255, 255, 255, 30 + static_cast<sf::Uint8>(ampScaled * 20));
                dots.append(vertex);
                sf::CircleShape dotShape(2.0f); // Set the radius of the dot (adjust size as needed)
                dotShape.setOrigin(2.0f, 2.0f); // Center the circle

                for (int y = 0; y < GRID_SIZE; ++y) {
                    for (int x = 0; x < GRID_SIZE; ++x) {
                        float u = static_cast<float>(x) / GRID_SIZE;
                        float v = static_cast<float>(y) / GRID_SIZE;

                        float n = perlin.noise3D(u * NOISE_FREQ, v * NOISE_FREQ, noiseOffset) * NOISE_AMP;

                        float px = u * DOT_SPACING + DOT_OFFSET + n;
                        float py = v * DOT_SPACING + DOT_OFFSET + n;
                        float pz = (n + 0.5f * NOISE_AMP) * ampScaled * 0.4f;

                        sf::Vector3f pos(px, py, pz);
                        float len = std::sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
                        if (len == 0) continue;

                        pos.x /= len;
                        pos.y /= len;
                        pos.z /= len;

                        float radius = std::min(SPHERE_RADIUS, len);
                        pos *= radius;

                        float featherStrength = 1.0f - std::clamp((radius - featherRadius) / FEATHER, 0.0f, 1.0f);
                        float scale = featherStrength * (SPHERE_RADIUS / radius - 1.0f) + 1.0f;

                        pos.x *= scale;
                        pos.y *= -scale;

                        sf::Vector2f screenPos = toScreen(pos.x, pos.y, WIDTH, HEIGHT);

                        if (screenPos.x < 0 || screenPos.x > WIDTH || screenPos.y < 0 || screenPos.y > HEIGHT)
                            continue;

                        dotShape.setPosition(screenPos);
                        dotShape.setFillColor(sf::Color(255, 255, 255, 30 + static_cast<sf::Uint8>(ampScaled * 20)));
                        window.draw(dotShape);
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(dots);
        window.display();
    }

    return 0;
}
