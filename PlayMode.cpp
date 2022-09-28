#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"
#include "ColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
	// ----https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c----
	/* Initialize FreeType and create FreeType font face. */
	if (FT_Init_FreeType (&ft_library))
		throw;
	if (FT_New_Face (ft_library, data_path("font.ttf").c_str(), 0, &ft_face))
		throw;
	if (FT_Set_Char_Size (ft_face, FONT_SIZE*64, FONT_SIZE*64, 0, 0))
		throw;

	/* Create hb-ft font. */
	hb_font = hb_ft_font_create (ft_face, NULL);

	/* Create hb-buffer. */
	hb_buffer = hb_buffer_create ();

	// ----https://learnopengl.com/In-Practice/Text-Rendering----
	// OpenGL state
    // ------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// setup the shader
    // ----------------
	glUseProgram(color_texture_program->program);
	glUniformMatrix4fv(color_texture_program->PROJECTION_mat4, 1, GL_FALSE, glm::value_ptr(projection)); 

	// set size to load glyphs as
    FT_Set_Pixel_Sizes(ft_face, 0, FONT_SIZE);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// configure VAO/VBO for texture quads
    // -----------------------------------
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);   

	initStory();  
}

PlayMode::~PlayMode() {
}

void PlayMode::initStory() {
	plots["begin"] = {
		{"description", {
			"Welcome to Game Design Simulator!", 
		}},
		{"options", {
			"(Press RETURN to continue)"
		}},
		{"states", {"begin2"}}
	};

	plots["begin2"] = {
		{"description", {
			"Welcome to Game Design Simulator!", 
			"",
			"Your goal is to design a game"
		}},
		{"options", {
			"(Press RETURN to continue)"
		}},
		{"states", {"begin3"}}
	};

	plots["begin3"] = {
		{"description", {
			"Welcome to Game Design Simulator!", 
			"",
			"Your goal is to design a game",
			"... which is the exact game that you're playing right now"
		}},
		{"options", {
			"(Press RETURN to continue)",
			"... wait what?"
		}},
		{"states", {"begin4", "begin4"}}
	};

	plots["begin4"] = {
		{"description", {
			"Good luck reaching the end of the game!"
		}},
		{"options", {
			"Is it supposed to make sense?"
		}},
		{"states", {"game name"}}
	};

	plots["game name"] = {
		{"description", {
			"What would you like to name your game?"
		}},
		{"options", {
			"Is it supposed to make sense?",
			"I haven't decided what the game should be yet"
		}},
		{"states", {"nothing makes sense", "game exists"}}
	};

	plots["nothing makes sense"] = {
		{"description", {
			"Nothing in this world makes sense, so why does this have to?"
		}},
		{"options", {
			"... if you say so"
		}},
		{"states", {"game name"}}
	};

	plots["game exists"] = {
		{"description", {
			"The game already exists - you're playing it right now"
		}},
		{"options", {
			"Umm..."
		}},
		{"states", {"game name 2"}}
	};

	plots["game name 2"] = {
		{"description", {
			"What would you like to name your game?"
		}},
		{"options", {
			"I don't f**king know???",
			"I don't want to play this game anymore",
			"Game ... Design ... Simulator ...?"
		}},
		{"states", {"game name 2", "rage quit", "game type"}}
	};

	plots["game type"] = {
		{"description", {
			"What type of game would you like it to be?"
		}},
		{"options", {
			"A choice-based game",
			"A rhythm game",
			"An adventure game",
			"A game that doesn't make sense",
			"So that's how it works ..."
		}},
		{"states", {"background color", "contradiction", "contradiction", "makes no sense", "game type"}}
	};

	plots["contradiction"] = {
		{"description", {
			"You have created a contradiction in the loop of time"
			"This universe is collapsing in 3, 2 -"
		}},
		{"options", {
			"wait wait waiT WaIT WAIT -"
		}},
		{"states", {"wait"}}
	};

	plots["wait"] = {
		{"description", {
			"Reminder: you can always press R to rewind the progress of this universe"
		}},
		{"options", {
			""
		}},
		{"states", {"wait"}}
	};

	plots["makes no sense"] = {
		{"description", {
			"Aaa aaa aaaa aaa aaaa aaaa aaaa aa aaaa aa aaaaa?"
		}},
		{"options", {
			"???"
		}},
		{"states", {"makes no sense 2"}}
	};

	plots["makes no sense 2"] = {
		{"description", {
			"Aaa aaa aaaaaa aaaa aaaaa aaaa?"
		}},
		{"options", {
			"????????"
		}},
		{"states", {"wait"}}
	};

	plots["background color"] = {
		{"description", {
			"What background color would you like to use?"
		}},
		{"options", {
			"White",
			"Grey",
			"Pale orange",
			"Colorful black",
		}},
		{"states", {"contradiction", "contradiction", "font color", "contradiction"}}
	};

	plots["font color"] = {
		{"description", {
			"What about font color?"
		}},
		{"options", {
			"Charcoal",
			"Bronze",
			"Coral",
		}},
		{"states", {"contradiction", "contradiction", "questions"}}
	};

	plots["questions"] = {
		{"description", {
			"How many questions will this game have?"
		}},
		{"options", {
			"3",
			"5",
			"42", // TODO
		}},
		{"states", {"contradiction", "early end", "TODO"}}
	};

	plots["early end"] = {
		{"description", {
			"You have reached one of the endings of this game. Congratulations!",
			"(I would highly suggest going back (use R to revert) to explore other options though :) )"
		}},
		{"options", {
			"", // TODO
		}},
		{"states", {"early end"}}
	};
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RETURN) {
			ret.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_r) {
			revert.pressed = true;
			return true;
		} 
	}

	return false;
}

void PlayMode::update(float elapsed) {
	if (down.pressed) {
		curOption = (curOption + 1) % plots[curState]["states"].size();
		down.pressed = false;
	}
	if (up.pressed) {
		curOption = (curOption - 1) % plots[curState]["states"].size();
		up.pressed = false;
	}

	if (ret.pressed) {
		// TODO
		if (plots[curState]["states"][curOption] != curState) {
			prevStates.push_back(std::pair(curState, curOption));
		}
		curState = plots[curState]["states"][curOption];
		curOption = 0;

		ret.pressed = false;

		if (curState == "rage quit") {
			Mode::current = nullptr;
		}
	}

	if (revert.pressed) {
		if (!prevStates.empty()) {
			curState = prevStates.back().first;
			curOption = prevStates.back().second;
			prevStates.pop_back();
		}

		revert.pressed = false;
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
	glClear(GL_COLOR_BUFFER_BIT);

	float x = LEFT_BORDER;
	float y = SCREEN_HEIGHT - TOP_BORDER;
	for (auto text: plots[curState]["description"]) {
		renderText(text, x, y, 1.0f, TEXT_COLOR);
		y -= LINE_HEIGHT;
	}

	y = SCREEN_HEIGHT / 2 - LINE_HEIGHT;
	for (int i = 0; i < plots[curState]["options"].size(); i++) {
		std::string text = plots[curState]["options"][i];
		if (i == curOption && text.size() > 0) {
			text = "-> " + text;
		}

		renderText(text, x, y, 1.0f, TEXT_COLOR);
		y -= LINE_HEIGHT;
	}	

	GL_ERRORS();
}

void PlayMode::renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
	// activate corresponding render state, see https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp
	glUniform3f(glGetUniformLocation(color_texture_program->program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// ----https://github.com/tangrams/harfbuzz-example/blob/master/src/hbshaper.h----
	hb_buffer_reset(hb_buffer);
    hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR); // https://harfbuzz.github.io/setting-buffer-properties.html
    hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
    hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));
    size_t length = text.size();
	hb_buffer_add_utf8(hb_buffer, text.c_str(), length, 0, length);
	hb_shape(hb_font, hb_buffer, NULL, 0);
	unsigned int glyphCount;
    hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(hb_buffer, &glyphCount);

	for (int i = 0; i < glyphCount; ++i) {
		auto c = glyphInfo[i].codepoint;
		// cache the glyphs
		if (Characters.find(c) == Characters.end()) {
			// load character glyph, see https://freetype.org/freetype2/docs/tutorial/step1.html
			if (FT_Load_Glyph(ft_face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				throw;
			}
			// generate texture, see //https://learnopengl.com/In-Practice/Text-Rendering
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				ft_face->glyph->bitmap.width,
				ft_face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				ft_face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture, 
				glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows),
				glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top),
				static_cast<unsigned int>(ft_face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}

		Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
