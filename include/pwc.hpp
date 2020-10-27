#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct pwc;
struct pwcWidget : ModuleWidget
{
	pwcWidget(pwc *module);
};

struct pwc : Module
{
	enum ParamIds
	{
		PW,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	pwc() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PW, 0, 10, 5);
		pulsed.reset();
	}
	void process(const ProcessArgs &args) override;

private:
	dsp::SchmittTrigger trigger;
	PulseGenerator2 pulsed;
};
