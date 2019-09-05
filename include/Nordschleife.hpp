#include "common.hpp"

using namespace rack;
extern Plugin *pluginInstance;
#define NORDSTEPS	64
#define NORDCARS	4

enum NordschleifeFields
{
	shlfStep,
	shlfDirection,

	NORDFIELDS
};

struct NordschleifeField
{
	std::string label;
	int minValue;
	int maxValue;
	int *currentValue;

	void set(const char *lbl, int mi, int ma, int *p)
	{
		label = lbl;
		minValue=mi;
		maxValue=ma;
		currentValue=p;
	}
};


struct Nordschleife;
struct nordDisplay;
struct NordschleifeWidget : SequencerWidget
{
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH = 0x01,
		RANDOMIZE_LAQUALUNQUE = 0x10,
		QUANTIZE_PITCH
	};

	struct RandomizeSubItemItem : MenuItem
	{
		RandomizeSubItemItem(Module *m, const char *title, int action);

		int randomizeDest;
		Nordschleife *md;
		void onAction(const event::Action &e) override;
	};

	struct RandomizeItem : ui::MenuItem
	{
		public:
		RandomizeItem(Module *m)
		{
			md = m;
			text = "Force the hand of chance";
			rightText = RIGHT_ARROW;
		};
		Menu *createChildMenu() override
		{
			Menu *sub_menu = new Menu;
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Pitch", RANDOMIZE_PITCH));
			sub_menu->addChild(new RandomizeSubItemItem(md, "Ov Power", RANDOMIZE_LAQUALUNQUE));
			return sub_menu;
		}
		private:
		Module *md;
	};

	void onHoverKey(const event::HoverKey &e) override;

	NordschleifeWidget(Nordschleife *module);
	void createStep(Nordschleife *module, int index);
	void createCar(Nordschleife *module, int index);
	void createDataEntry(Nordschleife *module);
	void onMenu(int action);
	Menu *addContextMenu(Menu *menu) override;

};

struct Nordschleife : Module
{
	enum ParamIds
	{
		VOLTAGE_1,
		STEPSELECT_1 = VOLTAGE_1 + NORDSTEPS,
		CAR_SELECT = STEPSELECT_1 + NORDSTEPS,
		DATAENTRY_MODE = CAR_SELECT + NORDCARS,
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
		display = NULL;
		pWidget = NULL;
		theRandomizer = 0;
		declareFields();
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;

	cvStrip cvs;
	int theRandomizer;
	nsFields[NORDFIELDS];

	TransparentWidget *createDisplay(Vec pos);

	void QuantizePitch();
	void setWidget(NordschleifeWidget *pwdg) { pWidget = pwdg; }
	inline void setCar(int n)
	{
		selectedCar = n;
		for(int k = 0; k < NORDCARS; k++)
			params[CAR_SELECT + k].setValue(k == selectedCar);
	}
	inline void toggleDataEntryMode()
	{
		params[DATAENTRY_MODE].setValue(params[DATAENTRY_MODE].getValue() == 0);
	}
	inline bool moveByStep() { return params[DATAENTRY_MODE].getValue() == 0; }
	inline void setKey(int code) { key = code; }

	protected:
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
		INFO("to json");
		json_t *rootJ = json_object();
		json_t *rndJson = json_integer(theRandomizer);
		json_object_set_new(rootJ, "theRandomizer", rndJson);
		return rootJ;
	}

	private:
	void car_select();
	void step_select();

	inline void setStep(int n)
	{
		selectedStep = n;
		for(int k = 0; k < NORDSTEPS; k++)
			params[STEPSELECT_1 + k].setValue(k == selectedStep);
	}

	void randrandrand();
	void randrandrand(int action);
	void on_loaded();
	void load();
	void data_entry();
	void declareFields();
	void reset()
	{
		lazyCheck = 0;
	}

	inline bool consumeKey(int code)
	{
		if(key == code)
		{
			key = 0;
			return true;
		}
		return false;
	}

	dsp::SchmittTrigger carSelectTrigger[NORDCARS];
	dsp::SchmittTrigger stepSelectTrigger[NORDSTEPS];
	dsp::SchmittTrigger rndTrigger;
	dsp::SchmittTrigger leftKey;
	dsp::SchmittTrigger rightKey;
	dsp::SchmittTrigger upKey;
	dsp::SchmittTrigger downKey;
	int lazyCheck = 0;
	int key = 0;
	int selectedCar = 0;
	int selectedStep = 0;
	NordschleifeWidget *pWidget;
	nordDisplay *display;
};
