#include <cmath>
#include "SrgbTransform.h"

namespace SrgbTransform {
	float srgbToLinear(float x) {
		if (x <= 0.0f)
			return 0.0f;
		else if (x >= 1.0f)
			return 1.0f;
		else if (x < 0.04045f)
			return x / 12.92f;
		else
			return std::pow((x + 0.055f) / 1.055f, 2.4f);
	}


	double srgbToLinear(double x) {
		if (x <= 0.0)
			return 0.0;
		else if (x >= 1.0)
			return 1.0;
		else if (x < 0.04045)
			return x / 12.92;
		else
			return std::pow((x + 0.055) / 1.055, 2.4);
	}

	float linearToSrgb(float x) {
		if (x <= 0.0f)
			return 0.0f;
		else if (x >= 1.0f)
			return 1.0f;
		else if (x < 0.0031308f)
			return x * 12.92f;
		else
			return std::pow(x, 1.0f / 2.4f) * 1.055f - 0.055f;
	}


	double linearToSrgb(double x) {
		if (x <= 0.0)
			return 0.0;
		else if (x >= 1.0)
			return 1.0;
		else if (x < 0.0031308)
			return x * 12.92;
		else
			return std::pow(x, 1.0 / 2.4) * 1.055 - 0.055;
	}

	uint8_t linearToSrgb8bit(float x) {
		if (x <= 0.0f) {
			return 0;
		} else if(x >= 1.0f) {
			return 255;
		}

		const float* TABLE = SRGB_8BIT_TO_LINEAR_FLOAT;
		uint8_t y = 0;

		for(uint8_t i = 128; i > 0; i >>= 1) {
			if(TABLE[y + i] <= x) {
				y += i;
			}
		}

		if(x - TABLE[y] <= TABLE[y + 1] - x) {
			return y;
		} else {
			return y + 1;
		}
	}
}