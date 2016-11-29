#pragma once

#include <cstdint>
#define _JSRT_
#include "ChakraCore.h"

class IJsEnvironment
{
public:
	virtual void setup_game() = 0;
	virtual void update_game(float dt) = 0;
	virtual void shutdown_game() = 0;
	virtual void add_module_function(const wchar_t *module, const wchar_t *function, JsNativeFunction callback, void *callbackState) = 0;
	virtual ~IJsEnvironment() {}
};

IJsEnvironment *make_js_environment();

void destroy_js_environment(IJsEnvironment *jse);