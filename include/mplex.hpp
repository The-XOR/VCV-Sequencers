#include "common.hpp"

#define NUM_MPLEX_INPUTS		(8)

////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;


struct Mplex;
struct MplexWidget : SequencerWidget
{
	MplexWidget(Mplex * module);
};

struct Mplex : Module
{
	enum ParamIds
	{
		BTUP, BTDN,
		OUTPUT_INC, OUTPUT_DEC,
		DIRECT_1,
		NUM_PARAMS = 	DIRECT_1 + NUM_MPLEX_INPUTS
	};
	enum InputIds
	{
		INUP,
		INDN,
		RESET,
		RANDOM,
		CV,
		IN_1,	
		NUM_INPUTS = IN_1 + NUM_MPLEX_INPUTS
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

	Mplex() : Module()
	{		
		pWidget = NULL;
		num_inputs_f = NUM_MPLEX_INPUTS;

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Mplex::BTUP, 0.0, 1.0, 0.0);
		configParam(Mplex::BTDN, 0.0, 1.0, 0.0);
		configParam(Mplex::DIRECT_1, 0.0, 1.0, 1.0);

		load();
	}
	void setWidget(MplexWidget *pwdg) { pWidget = pwdg; }

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "num_inputs", json_integer((int)num_inputs_f));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		Module::dataFromJson(rootJ); 
		json_t *bpmJson = json_object_get(rootJ, "num_inputs");
		if(bpmJson)
			num_inputs_f = (float)json_integer_value(bpmJson);
		on_loaded();
	}

	int getRand(int rndMax) { return int(random::uniform() * rndMax); }
	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void onRandomize() override 
	{
		set_output(getRand(num_inputs_f));
	}
	float num_inputs_f;
	MplexWidget *pWidget;

private:
	void load();
	void on_loaded();
	void set_output(int n);
	void process_keys();

	int cur_sel;
	dsp::SchmittTrigger direct[NUM_MPLEX_INPUTS];
	dsp::SchmittTrigger upTrigger;
	dsp::SchmittTrigger dnTrigger;
	dsp::SchmittTrigger reset;
	dsp::SchmittTrigger random;
	dsp::SchmittTrigger outInc;
	dsp::SchmittTrigger outDec;
};
