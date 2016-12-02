#pragma once

#include <cstdint>
#define _JSRT_
#include "ChakraCore.h"

typedef void *(*GetApiFunction)(unsigned api);

namespace stingray_plugin_foundation {
	class Allocator;
}

class IJsEnvironment
{
public:
	// Call init in the specified boot script.
	virtual void init(const char *boot_script_name) = 0;
	
	// Call update in the loaded boot script.
	virtual void update(float dt) = 0;

	// Call render in the loaded boot script.
	virtual void render() = 0;

	// Call shutdown in the loaded boot script.
	virtual void shutdown() = 0;
	
	// Add module function.
	virtual void add_module_function(const wchar_t *module, const wchar_t *function, JsNativeFunction callback, void *callbackState) = 0;
	
	virtual ~IJsEnvironment() {}
};

IJsEnvironment *make_js_environment(stingray_plugin_foundation::Allocator &a, GetApiFunction get_api);

void destroy_js_environment(stingray_plugin_foundation::Allocator &a, IJsEnvironment *jse);