#ifndef __FRAGMENTSHADER_H__
#define __FRAGMENTSHADER_H__

#include <Shader.h>

class FragmentShader : public Shader {

private:
protected:
public:
	FragmentShader (std::string souce_code)
	:	Shader (souce_code, GL_FRAGMENT_SHADER) {
	}

};

#endif