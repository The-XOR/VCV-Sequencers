#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;

struct Uncertain;
struct UncertainWidget : SequencerWidget
{
	UncertainWidget(Uncertain *module);
};

struct Uncertain : Module
{
	enum ParamIds
	{
		FLUCT_AMT,
		QUANTIZED_AMT,
		STORED_AMT,
		CURVEAMP_AMT,
		NUM_PARAMS
	};

	enum InputIds
	{
		CLOCK_FLUCT,
		IN_FLUCT,
		CLOCK_QUANTIZED,
		IN_QUANTIZED,
		CLOCK_STORED,
		IN_STORED,
		IN_CURVEAMP,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_FLUCT,
		OUT_QUANTIZED_N1,
		OUT_QUANTIZED_2N,
		OUT_STORED_RND,
		OUT_STORED_BELL,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		NUM_LIGHTS 
	};

	Uncertain() : Module()
	{		
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Uncertain::FLUCT_AMT, LVL_MIN, LVL_MAX, LVL_OFF, "Fluctuate", "V");
		configParam(Uncertain::QUANTIZED_AMT, 0.0, 5.0, 0.0, "Quantized Amount", "V", 0, 1, 1);
		configParam(Uncertain::STORED_AMT, LVL_OFF + 2.5, LVL_MAX - 2.5, 5.0, "Stored Amount", "V");
		configParam(Uncertain::CURVEAMP_AMT, 0.0,2.0,1.0, "Sigma", "#");
	}

	void process(const ProcessArgs &args) override;
	void onReset() override { load(); }
	void dataFromJson(json_t *root) override { Module::dataFromJson(root); on_loaded(); }
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		return rootJ;
	};

private:
	void on_loaded();
	void load();
	void out_quantized(int clk);
	void out_stored(int clk);
	void out_fluct(int clk);
	int getInt(ParamIds p_id, InputIds i_id, float minValue, float maxValue) { return (int)getFloat(p_id, i_id, minValue, maxValue); }
	float rndFluctVoltage();
	float rndGaussianVoltage();

private:
	SchmittTrigger2 clock_fluct;
	SchmittTrigger2 clock_quantized;
	SchmittTrigger2 clock_stored;
	struct fluct_params
	{
		float vA;
		float vB;
		float deltaV;
		clock_t tStart;
		clock_t duration;

		void reset()
		{
			duration = tStart = 0;
			vA = deltaV = 0;
		}	
	} fluctParams;
};
