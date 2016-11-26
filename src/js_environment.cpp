#include <js_environment.h>

class JsEnvironment final : public IJsEnvironment
{
public:
	JsEnvironment() {}
};

IJsEnvironment *make_js_environment()
{
	return new JsEnvironment();
}

void destroy_js_environment(IJsEnvironment *jse)
{
	delete jse;
}
