#pragma once
#include "Spiralone.hpp"

struct Spiralone : Module
{
	enum ParamIds
	{
		M_RESET,
		VOLTAGE_1,
		MODE_1 = VOLTAGE_1 + TOTAL_STEPS,
		LENGHT_1 = MODE_1 + NUM_SEQUENCERS,
		STRIDE_1 = LENGHT_1 + NUM_SEQUENCERS,
		XPOSE_1 = STRIDE_1 + NUM_SEQUENCERS,
		RANGE = XPOSE_1 + NUM_SEQUENCERS,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};

	enum InputIds
	{
		RANDOMIZONE,
		RESET_1,
		INLENGHT_1 = RESET_1 + NUM_SEQUENCERS,
		INSTRIDE_1 = INLENGHT_1 + NUM_SEQUENCERS,
		INXPOSE_1 = INSTRIDE_1 + NUM_SEQUENCERS,
		CLOCK_1 = INXPOSE_1 + NUM_SEQUENCERS,
		RANGE_IN = CLOCK_1 + NUM_SEQUENCERS,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};

	enum OutputIds
	{
		CV_1,
		GATE_1 = CV_1 + NUM_SEQUENCERS,
		NUM_OUTPUTS = GATE_1 + NUM_SEQUENCERS
	};

	enum LightIds
	{
		LED_SEQUENCE_1,
		NUM_LIGHTS = (LED_SEQUENCE_1 + TOTAL_STEPS) * NUM_SEQUENCERS
	};

	Spiralone() : Module()
	{
		theRandomizer = 0;
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int k = 0; k < TOTAL_STEPS; k++)
			configParam(Spiralone::VOLTAGE_1 + k, 0.0, 1.0, 0.0, "Voltage", "V");
		configParam(Spiralone::M_RESET, 0.0, 1.0, 0.0);
		for(int seq = 0; seq < NUM_SEQUENCERS; seq++)
		{
			sequencer[seq].seq = seq;
			configParam(Spiralone::MODE_1 + seq, 0.0, 2.0, 0.0);
			configParam(Spiralone::LENGHT_1 + seq, 1.0, TOTAL_STEPS, TOTAL_STEPS, "Steps", "#");
			configParam(Spiralone::STRIDE_1 + seq, 1.0, 8.0, 1.0, "Stride", "#");
			configParam(Spiralone::XPOSE_1 + seq, LVL_MIN, LVL_MAX, 0.0, "Transpose", "V");
		}
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);

		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(this, Scene5, 1);
		#endif
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 5);
		#endif

		on_loaded();
	}

	#ifdef DIGITAL_EXT
	~Spiralone()
	{
		#if defined(LAUNCHPAD)
		delete drv;
		#endif
		#if defined(OSCTEST_MODULE)
		delete oscDrv;
		#endif
	}
	#endif

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void setWidget(SpiraloneWidget *pwdg) { pWidget = pwdg; }
	void QuantizePitch();

	void dataFromJson(json_t *root) override
	{
		Module::dataFromJson(root);
		json_t *rndJson = json_object_get(root, "theRandomizer");
		if(rndJson)
			theRandomizer = json_integer_value(rndJson);
		on_loaded();
	}
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *rndJson = json_integer(theRandomizer);
		json_object_set_new(rootJ, "theRandomizer", rndJson);
		return rootJ;
	}
	int theRandomizer;

	#ifdef DIGITAL_EXT
	float connected;
	#endif
	#ifdef LAUNCHPAD
	LaunchpadBindingDriver *drv = NULL;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv = NULL;
	#endif
	spiraloneSequencer sequencer[NUM_SEQUENCERS];
	cvStrip cvs;

private:
	void on_loaded();
	void load();
	void randrandrand();
	void randrandrand(int action);

	SpiraloneWidget *pWidget;
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger rndTrigger;
};
