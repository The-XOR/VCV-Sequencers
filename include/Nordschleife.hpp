#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
#define NORDSTEPS	48
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
		RANGE_IN = CAR_CLOCK + NORDCARS,
		NUM_INPUTS = RANGE_IN + cvStrip::CVSTRIP_INPUTS
	};
	enum OutputIds
	{
		LOTUS_OUT,
		BRABHAM_OUT = LOTUS_OUT + NORDSTEPS,
		FERRARI_OUT = BRABHAM_OUT + NORDSTEPS,
		HESKETH_OUT = FERRARI_OUT + NORDSTEPS,
		CAR_CV = HESKETH_OUT + NORDSTEPS,
		CAR_GATE = CAR_CV + NORDCARS,
		NUM_OUTPUTS = CAR_GATE + NORDCARS
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
private:

};
