#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define C2VCNV 12
struct c2v;
struct c2vWidget : ModuleWidget
{
	c2vWidget(c2v *module);
};

struct c2v : Module
{
	enum ParamIds
	{
		CV_1,
		NUM_PARAMS = CV_1 + C2VCNV
	};
	enum InputIds
	{
		IN_1,
		NUM_INPUTS = IN_1 + C2VCNV
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + C2VCNV
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	c2v() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < C2VCNV; k++)
			configParam(OUT_1+k, LVL_MIN, LVL_MAX, LVL_OFF);
	}
	void process(const ProcessArgs &args) override;

private:
};
