#include "cue.h"
#include "audio.h"
#include "mathUtils.h"

Cue::Cue(const sf::RenderWindow &window, Ball &cueBall) : m_Window(window), m_CueBall(cueBall) {
    m_Texture.loadFromFile("assets/textures/cue.png");
    const sf::Vector2f textureSize(m_Texture.getSize());

    m_Sprite.setTexture(m_Texture, true);
    m_Sprite.setScale(8,5);

    m_Sprite.setOrigin(textureSize.x, textureSize.y * 0.5f);
}

void Cue::update(float dt) {
    if(m_CueBall.m_InPocket)
        m_Aiming = false;

    if(!m_Aiming)
        return;

    if(m_HitAnimation) {
        hitAnimationStep(dt);
        return;
    }

    const sf::Vector2f mousePositionWorld = m_Window.mapPixelToCoords(sf::Mouse::getPosition(m_Window));
    const sf::Vector2f delta = m_CueBall.m_Position - mousePositionWorld;
    m_Direction = MathUtils::normalized(delta);

    m_Force = std::clamp(MathUtils::length(delta), 0.0f, 100.0f);
    const sf::Vector2 position = m_CueBall.m_Position- m_Direction * m_Force;

    m_Sprite.setPosition(position);
    m_Sprite.setRotation(glm::degrees(std::atan2(delta.y, delta.x)));
}

void Cue::render(sf::RenderTarget &renderTarget) const {
    if(!m_Aiming)
        return;

    renderTarget.draw(m_Sprite);
}

void Cue::hitAnimationStep(float dt) {
    const sf::Vector2f delta = m_CueBall.m_Position - m_Sprite.getPosition();
    const sf::Vector2f direction = MathUtils::normalized(delta);
    const float distance = MathUtils::length(delta);

    m_Sprite.move(direction * 2000.0f * dt);

    if(distance <= Ball::RADIUS) {
        m_CueBall.m_Velocity += m_Direction * m_Force * 10.0f;
        m_Aiming = false;
        m_HitAnimation = false;
        Audio::play(m_Sound, Audio::AudioType::CUE_HIT, 100.0f, 1.0f);
    }
}

void Cue::startAiming() {
    if(m_CueBall.m_InPocket)
        return;

    m_Aiming = true;
}

void Cue::hit() {
    if(!m_Aiming || m_CueBall.m_InPocket)
        return;

    m_HitAnimation = true;
}
