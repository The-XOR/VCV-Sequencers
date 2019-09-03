#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct Nordschleife;
struct NordschleifeWidget : ModuleWidget
{
	NordschleifeWidget(Nordschleife *module);
};

struct Nordschleife : Module
{
	enum ParamIds
	{
		NUM_PARAMS
	};
	enum InputIds
	{
		NUM_INPUTS
	};
	enum OutputIds
	{
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	Nordschleife() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;

private:

};
