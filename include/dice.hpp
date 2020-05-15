#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct dice;
struct diceWidget : ModuleWidget
{
	diceWidget(dice *module);
};

struct dice : Module
{
	enum ParamIds
	{
		MIN,
		MAX,
		MODE,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN,
		MIN_IN,
		MAX_IN,
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

	dice() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MIN, LVL_MIN, LVL_MAX, LVL_MIN);
		configParam(MAX, LVL_MIN, LVL_MAX, LVL_MAX);
	}
	void process(const ProcessArgs &args) override;

private:
	dsp::SchmittTrigger trigger;
};
