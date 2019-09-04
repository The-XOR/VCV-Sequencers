#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
#define NORDSTEPS	64
#define NORDCARS	4

struct Nordschleife;
struct NordschleifeWidget : SequencerWidget
{
	NordschleifeWidget(Nordschleife *module);
	void createStep(Nordschleife *module, int index);
	void createCar(Nordschleife *module, int index);
	void createDataEntry(Nordschleife *module);
};

struct Nordschleife : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		STEPSELECT_1 = VOLTAGE_1 + NORDSTEPS,
		CAR_SELECT = STEPSELECT_1 + NORDSTEPS,
		DATAENTRY_OK = CAR_SELECT + NORDCARS,
		DATAENTRY_UP,
		DATAENTRY_DOWN,
		DATAENTRY_LEFT,
		DATAENTRY_RIGHT,
		RANGE,
		NUM_PARAMS = RANGE + cvStrip::CVSTRIP_PARAMS
	};
	enum InputIds
	{
		CAR_RESET,
		CAR_CLOCK = CAR_RESET + NORDCARS,
		RANDOMIZONE = CAR_CLOCK + NORDCARS,
		RANGE_IN,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};
	enum OutputIds
	{
		OUT_A,
		OUT_B = OUT_A + NORDSTEPS,
		CAR_CV = OUT_B + NORDSTEPS,
		CAR_GATE = CAR_CV + NORDCARS,
		CAR_LAP = CAR_GATE + NORDCARS,
		NUM_OUTPUTS = CAR_LAP + NORDCARS
	};
	enum LightIds
	{
		LOTUS_LED,
		BRABHAM_LED = LOTUS_LED + NORDSTEPS,
		FERRARI_LED = BRABHAM_LED + NORDSTEPS,
		HESKETH_LED = FERRARI_LED + NORDSTEPS,
		NUM_LIGHTS = HESKETH_LED + NORDSTEPS

	};

	Nordschleife() : Module()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;

	cvStrip cvs;
	int selectedCar = 0;
	int selectedStep = 0;

	private:
	void car_select();
	void step_select();
	inline void setCar(int n)
	{
		selectedCar = n;
		for(int k = 0; k < NORDCARS; k++)
			params[CAR_SELECT + k].setValue(k == selectedCar);
	}
	inline void setStep(int n)
	{
		selectedStep = n;
		for(int k = 0; k < NORDSTEPS; k++)
			params[STEPSELECT_1 + k].setValue(k == selectedStep);
	}
	private:
	dsp::SchmittTrigger carSelectTrigger[NORDCARS];
	dsp::SchmittTrigger stepSelectTrigger[NORDSTEPS];
};
