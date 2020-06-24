#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define C3VCNV 12
struct c3v;
struct c3vWidget : ModuleWidget
{
	c3vWidget(c3v *module);
};

struct c3v : Module
{
	enum ParamIds
	{
		CV_1,
		NUM_PARAMS = CV_1 + C3VCNV
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS = IN_1 + C3VCNV
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS 
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	c3v() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < C3VCNV; k++)
			configParam(OUT_1+k, LVL_MIN, LVL_MAX, LVL_OFF);
	}
	void process(const ProcessArgs &args) override;

private:
};
