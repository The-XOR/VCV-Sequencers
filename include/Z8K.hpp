#pragma once
#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "z8kSequencer.hpp"

#define Z8KPATHS 100

struct Z8KWidget : SequencerWidget
{
public:
	Z8KWidget(Z8K *module);
	void onMenu(int action);
private:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH,
		QUANTIZE_PITCH
	};

	Menu *addContextMenu(Menu *menu) override;
};

struct Z8K : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		M_RESET = VOLTAGE_1 + 16,
		DIRSW_1,
		DIRSW_A = DIRSW_1+4,
		DIRSW_VERT = DIRSW_A+4,
		DIRSW_HORZ,
		DIRSW_PATH,
		PTN_INC,
		PTN_DEC,
		RANGE,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};

	enum InputIds
	{
		RESET_1,
		RESET_A = RESET_1 + 4,
		RESET_VERT = RESET_A + 4,
		RESET_HORIZ,
		RESET_PATH,

		DIR_1,
		DIR_A = DIR_1 + 4,
		DIR_VERT = DIR_A + 4,
		DIR_HORIZ,
		DIR_PATH,

		CLOCK_1,
		CLOCK_A = CLOCK_1 + 4,
		CLOCK_VERT = CLOCK_A + 4,
		CLOCK_HORIZ,
		CLOCK_PATH,

		PATH_SELECT,

		RANDOMIZE,
		MASTERRESET,

		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};

	enum OutputIds
	{
		CV_1,
		CV_A = CV_1 + 4,
		CV_VERT = CV_A + 4,
		CV_HORIZ,
		CV_PATH,
		ACTIVE_STEP,
		EXP_PORT = ACTIVE_STEP + 16,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		LED_ROW,
		LED_COL = LED_ROW + 16,
		LED_VERT = LED_COL + 16,
		LED_HORIZ = LED_VERT + 16,
		LED_PATH = LED_HORIZ + 16,
		NUM_LIGHTS = LED_PATH + 16
	};

	enum SequencerIds
	{
		SEQ_1,
		SEQ_A = SEQ_1 + 4,
		SEQ_VERT = SEQ_A + 4,
		SEQ_HORIZ,
		SEQ_PATH,
		NUM_Z8SEQUENCERS
	};

	Z8K() : Module()
	{
		basePtn=curPtn = 0;
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(int r = 0; r < 4; r++)
		{
			for(int c = 0; c < 4; c++)
			{
				int n = c + r * 4;
				configParam(Z8K::VOLTAGE_1 + n, 0.0, 1.0, 0.0, "Voltage", "V");
			}
		}
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);
		on_loaded();
	}

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void setWidget(Z8KWidget *pwdg) { pWidget = pwdg; }
	void QuantizePitch();

	void dataFromJson(json_t *root) override 
	{ 
		Module::dataFromJson(root); on_loaded(); 
		json_t *rndJson = json_object_get(root, "basePtn");
		if(rndJson)
			basePtn = json_integer_value(rndJson);

	}
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *rndJson = json_integer(basePtn);
		json_object_set_new(rootJ, "basePtn", rndJson);
		return rootJ;
	}
	cvStrip cvs;
	inline int patternNumber() const {return curPtn; }
	inline std::vector<int> getPath() const {
		std::vector<int> rv;
		for (int k = 0; k < 16; k++)
			rv.push_back(paths[curPtn][k]);
		return rv;
	}

private:
	void on_loaded();
	void process_keys();
	void load();
	void reset();	
	z8kSequencer seq[NUM_Z8SEQUENCERS];
	dsp::SchmittTrigger randomizeTrigger;
	Z8KWidget *pWidget;
	dsp::SchmittTrigger masterReset;
	dsp::SchmittTrigger masterResetIn;
	dsp::SchmittTrigger btninc;
	dsp::SchmittTrigger btndec;
	int curPtn;
	int basePtn;
	static int paths[Z8KPATHS][16];
};
