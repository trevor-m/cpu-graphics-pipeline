#ifndef SHADER_H
#define SHADER_H

#include "Texture.h"

//base class
class Shader {
public:
	virtual void Shade(u08* outColor, float u, float v, Texture* tex, Vertex& normal, Vertex& viewPos, Vertex& modelPos) {
		tex->getInterpolatedColor(u, v, outColor);
	}
};

//creates a checkerboard pattern
class TileShader : public Shader {
public:
	virtual void Shade(u08* outColor, float u, float v, Texture* tex, Vertex& normal, Vertex& viewPos, Vertex& modelPos) {
		if ((int)(floor(u) + floor(v)) % 2 == 0) {
			outColor[0] = 255;
			outColor[1] = 255;
			outColor[2] = 255;
		}
		else {
			outColor[0] = 0;
			outColor[1] = 0;
			outColor[2] = 0;
		}
	}
};

//applies a point light
class LightShader : public Shader {
private:
	//light pos                                                       
	Vertex lightPos;
	float ambientColor[3];
	float diffuseColor[3];
	float specularColor[3];
	//attenuation constants: atten = 1/(ax^2 + bx + c)
	float a, b, c;

public:
	LightShader(float x, float y, float z, float a, float b, float c, float ambientR, float ambientG, float ambientB, float diffuseR, float diffuseG, float diffuseB, float specularR, float specularG, float specularB) {
		lightPos.x = x;
		lightPos.y = y;
		lightPos.z = z;
		this->a = a;
		this->b = b;
		this->c = c;
		ambientColor[0] = ambientR;
		ambientColor[1] = ambientG;
		ambientColor[2] = ambientB;
		diffuseColor[0] = diffuseR;
		diffuseColor[1] = diffuseG;
		diffuseColor[2] = diffuseB;
		specularColor[0] = specularR;
		specularColor[1] = specularG;
		specularColor[2] = specularB;
	}

	virtual void Shade(u08* outColor, float u, float v, Texture* tex, Vertex& normal, Vertex& viewPos, Vertex& modelPos) {
		//get texture color
		u08 originalColor[3];
		tex->getInterpolatedColor(u, v, originalColor);
		float inColor[3];
		inColor[0] = originalColor[0] / 255.0f;
		inColor[1] = originalColor[1] / 255.0f;
		inColor[2] = originalColor[2] / 255.0f;

		float finalColor[3];

		//ambient color
		finalColor[0] = ambientColor[0] * inColor[0];
		finalColor[1] = ambientColor[1] * inColor[1];
		finalColor[2] = ambientColor[2] * inColor[2];

		//diffuse color
		//get light dir
		Vertex lightDir = (lightPos - modelPos).normalize();
		//diffuse factor
		float diff = max(normal.dot3(lightDir), 0.0f);
		//add diffuse color
		finalColor[0] += diff * inColor[0] * diffuseColor[0];
		finalColor[1] += diff * inColor[1] * diffuseColor[1];
		finalColor[2] += diff * inColor[2] * diffuseColor[2];

		//specular color
		Vertex viewDir = (viewPos - modelPos).normalize();
		Vertex reflectDir = (-lightDir).reflect(normal);
		float spec = pow(max(viewDir.dot3(reflectDir), 0.0f), 32);
		//add specular color
		finalColor[0] += spec * 1.0f * specularColor[0];
		finalColor[1] += spec * 1.0f * specularColor[1];
		finalColor[2] += spec * 1.0f * specularColor[2];

		//attenuate by distance
		float distance = (lightPos - modelPos).length();
		float attenuation = 1.0f / (a*distance*distance + b*distance + c);
		finalColor[0] *= attenuation;
		finalColor[1] *= attenuation;
		finalColor[2] *= attenuation;
		
		//output final color
		for (int i = 0; i < 3; i++) {
			if (finalColor[i] > 1.0f)
				finalColor[i] = 1.0f;
			outColor[i] = finalColor[i] * 255;
		}
	}
};


#endif