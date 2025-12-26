namespace test
{
	extern const float SCREEN_WIDTH;
	extern const float SCREEN_HEIGHT;

	void render(void);
	void update(int dt);
	void on_click(float x, float y);

	void init(void);
	void term(void);
};

namespace platform
{
	void drawPoint(float x, float y, float r, float g, float b, float a);
};