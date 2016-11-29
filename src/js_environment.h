#pragma once

#include <cstdint>
#define _JSRT_
#include "ChakraCore.h"

class IJsEnvironment
{
public:
	// Call setup game in main script.
	virtual void setup_game() = 0;
	
	// Call setup game in main script.
	virtual void update_game(float dt) = 0;
	
	// Call shutdown game in main script.
	virtual void shutdown_game() = 0;
	
	// Add module function.
	virtual void add_module_function(const wchar_t *module, const wchar_t *function, JsNativeFunction callback, void *callbackState) = 0;
	
	virtual ~IJsEnvironment() {}
};

IJsEnvironment *make_js_environment();

void destroy_js_environment(IJsEnvironment *jse);