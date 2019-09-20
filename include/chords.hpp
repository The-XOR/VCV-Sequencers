#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

#define NUM_CHORDS		4
struct chords;
struct chordsWidget : ModuleWidget
{
	chordsWidget(chords *module);
};

struct chords : Module
{
	enum ParamIds
	{
		ST_1,
		NUM_PARAMS = ST_1+ NUM_CHORDS
	};
	enum InputIds
	{
		INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS= OUT_1+NUM_CHORDS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	chords() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < NUM_CHORDS; k++)
			configParam(ST_1 + k, -24, 24, 0, "Semitones", "#");
	}
	void process(const ProcessArgs &args) override;

private:

};
