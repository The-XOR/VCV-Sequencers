#pragma once
#include "common.hpp"

#define CCLK_MINVALUE 0
#define CCLK_MAXVALUE 64

struct cclk;
struct cclkWidget : SequencerWidget
{
	cclkWidget(cclk *module);
};

struct cclk : Module
{
	enum ParamIds
	{
		COUNTER_INC,
		COUNTER_DEC,
		COUNTER_INC2,
		COUNTER_DEC2,
		COUNTR_IN_ATT,
		RESET_IN_ATT,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		IN_1,
		COUNTR_IN,
		RESET_IN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		OUT_2,
		OUT_3,
		OUT_4,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		NUM_LIGHTS
	};

	cclk() : Module()
	{
		pWidget = NULL;
		autoreset_f = 0;
		counter_f=4;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(cclk::COUNTER_INC, 0.0, 1.0, 0.0);
		configParam(cclk::COUNTER_DEC, 0.0, 1.0, 0.0);
		configParam(cclk::COUNTER_INC2, 0.0, 1.0, 0.0);
		configParam(cclk::COUNTR_IN_ATT,CCLK_MINVALUE, CCLK_MAXVALUE, 0.0);
		configParam(cclk::RESET_IN_ATT,CCLK_MINVALUE, CCLK_MAXVALUE, 0.0);

		on_loaded();
	}

	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *bpmJson = json_integer((int)counter_f);
		json_object_set_new(rootJ, "counter", bpmJson);
		bpmJson = json_integer((int)autoreset_f);
		json_object_set_new(rootJ, "autoreset", bpmJson);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "counter");
		if(bpmJson)
			counter_f = (float)json_integer_value(bpmJson);
		bpmJson = json_object_get(rootJ, "autoreset");
		if (bpmJson)
			autoreset_f = (float)json_integer_value(bpmJson);
		on_loaded();
	}

	void onReset() override
	{
		load();
	}

	void onRandomize() override {}
	void setWidget(cclkWidget *pwdg) { pWidget = pwdg; }
	float counter_f;
	float autoreset_f;

private:
	dsp::SchmittTrigger btnup;
	dsp::SchmittTrigger btndwn;
	dsp::SchmittTrigger btnup2;
	dsp::SchmittTrigger btndwn2;
	cclkWidget *pWidget;
	int counter;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger counterTigger;

	void process_keys();

	void on_loaded();
	void load();
	void reset();
};
