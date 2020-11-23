#include "common.hpp"

#define NUM_DEMULTIPLEX_OUTPUTS		(8)

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;


struct Dmplex;
struct DmplexWidget : SequencerWidget
{
	DmplexWidget(Dmplex * module);
};

struct Dmplex : Module
{
	enum ParamIds
	{
		BTUP, BTDN,
		OUTPUT_INC, OUTPUT_DEC,
		DIRECT_1,
		NUM_PARAMS = 	DIRECT_1 + NUM_DEMULTIPLEX_OUTPUTS
	};
	enum InputIds
	{
		INUP,
		INDN,
		RESET,
		RANDOM,
		IN_1,		
		CV,
		NUM_INPUTS 
	};
	enum OutputIds
	{
		OUT_1,
		NUM_OUTPUTS = OUT_1 + NUM_DEMULTIPLEX_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS 
	};

	Dmplex() : Module()
	{		
		pWidget = NULL;
		num_outputs_f = NUM_DEMULTIPLEX_OUTPUTS;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Dmplex::BTUP, 0.0, 1.0, 0.0);
		configParam(Dmplex::BTDN, 0.0, 1.0, 0.0);
		configParam(Dmplex::DIRECT_1, 0.0, 1.0, 1.0);

		load();
	}
	void setWidget(DmplexWidget *pwdg) { pWidget = pwdg; }

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "num_outputs", json_integer((int)num_outputs_f));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		Module::dataFromJson(rootJ); 
		json_t *bpmJson = json_object_get(rootJ, "num_outputs");
		if(bpmJson)
			num_outputs_f = (float)json_integer_value(bpmJson);
		on_loaded();
	}

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override 
	{
		set_output(getRand(roundf(num_outputs_f)));
	}
	float num_outputs_f;
	DmplexWidget *pWidget;

private:
	void load();
	void process_keys();
	void on_loaded();
	void set_output(int n);
	int getRand(int rndMax) { return int(random::uniform() * rndMax); }

	int cur_sel;
	dsp::SchmittTrigger direct[NUM_DEMULTIPLEX_OUTPUTS];
	dsp::SchmittTrigger upTrigger;
	dsp::SchmittTrigger dnTrigger;
	dsp::SchmittTrigger reset;
	dsp::SchmittTrigger random;
	dsp::SchmittTrigger outInc;
	dsp::SchmittTrigger outDec;
};
