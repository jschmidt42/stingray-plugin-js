#pragma once

class IJsEnvironment
{
public:	
	virtual ~IJsEnvironment() {}
};

IJsEnvironment *make_js_environment();

void destroy_js_environment(IJsEnvironment *jse);