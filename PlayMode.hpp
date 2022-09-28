#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H  

const float FONT_SIZE = 24;
const float LINE_HEIGHT = FONT_SIZE * 1.5;
const float SCREEN_WIDTH = 1280;
const float SCREEN_HEIGHT = 720;
const float LEFT_BORDER = 60;
const float TOP_BORDER = 120;

const glm::vec4 BACKGROUND_COLOR = glm::vec4(0.95, 0.85, 0.75, 1.0);
const glm::vec3 TEXT_COLOR = glm::vec3(0.8, 0.5, 0.5);

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// ----https://learnopengl.com/In-Practice/Text-Rendering----
	struct Character {
		unsigned int TextureID;  // ID handle of the glyph texture
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Offset to advance to next glyph
	};
	glm::mat4 projection = glm::ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT);
	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
	unsigned int VAO, VBO;

	FT_Library ft_library;
	FT_Face ft_face;
	hb_font_t *hb_font;
	hb_buffer_t *hb_buffer;

	std::map<char, Character> Characters;

	//----- game state -----

	void initStory();
	std::map<std::string, std::map<std::string, std::vector<std::string>>> plots;
	std::string curState = "begin";
	int curOption = 0;
	std::vector<std::pair<std::string, int>> prevStates = {};

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} down, up, ret, revert;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//camera:
	Scene::Camera *camera = nullptr;

};
