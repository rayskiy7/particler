#include <engine.h>
#include <settings.h>

#include "test.h"

const float test::SCREEN_WIDTH = ::SCREEN_WIDTH;
const float test::SCREEN_HEIGHT = ::SCREEN_HEIGHT;

std::unique_ptr<PhysicsEngine> engine;

void test::init(void)
{
	engine = std::make_unique<PhysicsEngine>();
	engine->run_async();
}

void test::term(void)
{
	engine->stop_async();
}

void test::render(void)
{
    engine->for_each_particle(platform::drawPoint);
}

void test::update(int dt)
{
    //* do nothing; using lock-free independent physics
}

void test::on_click(float x, float y)
{
	engine->interact(x, SCREEN_HEIGHT-y);
}