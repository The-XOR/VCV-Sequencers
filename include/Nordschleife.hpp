#include "common.hpp"
#include <functional>

using namespace rack;
extern Plugin *pluginInstance;
#define NORDSTEPS	64
#define NORDCARS	4

enum NordschleifeFields
{
	shlfDirection,
	shlfPath,
	shlfCollision,
	shlfFrom,
	shlfTo,

	shlfStep,
	shlfMode,
	shlfProbab,
	shlfRepeats,
	shlfOutA,
	shlfOutB,

	NORDFIELDS
};

struct NordschleifeField
{
	std::string label;
	int minValue;
	int maxValue;
	std::function<void(int)> setter;
	std::function<int(void)> getter;
	std::vector<std::string> values;
	int pos_x;
	int pos_y;
	int display_offset;
	bool bigField;

	void set(int x, int y, const char *lbl, std::vector<std::string> strngs, std::function<int(void)> gtr, std::function<void(int)> str, bool big = false)
	{
		set(x,y, lbl, 0, (int)strngs.size()-1, gtr, str, 0, big);
		values = strngs;
	}

	void set(int x, int y, const char *lbl, int mi, int ma, std::function<int(void)> gtr, std::function<void(int)> str, int dispoff = 0, bool big = false)
	{
		pos_x=x;
		pos_y=y;
		label = lbl;
		minValue=mi;
		maxValue=ma;
		getter=gtr;
		setter=str;
		display_offset = dispoff;
		bigField = big;
	}

	void inc()
	{
		int value = getter();
		if(++value > maxValue)
			value = minValue;
		setter(value);
	}

	void dec()
	{
		int value = getter();
		if(--value < minValue)
			value = maxValue;
		setter(value);
	}

	std::string getText()
	{
		if(values.empty())
		{
			char n[20];
			sprintf(n, "%i", display_offset+getter());
			return n;
		}
		
		return values[getter()];
	}
};

struct Nordschleife;
struct NordschleifeCar
{
	enum CarDirection {carForward, carBackward, carAlternate, carBrownian, carRandom };
	enum CarCollision { carIgnore, carInvert, car90lef, car90right };
	std::string name;
	CarDirection direction;
	CarCollision collision;
	int stepFrom;
	int stepTo;
	int path;

	void Init(Nordschleife *p, int id, int lightid);

	void dataFromJson(json_t *root)
	{
		json_t *r = json_object_get(root, ("cardir_"+myIDstr).c_str());
		if(r) direction = (CarDirection)json_integer_value(r);
		r = json_object_get(root, ("carcoll_" + myIDstr).c_str());
		if(r) collision = (CarCollision)json_integer_value(r);
		r = json_object_get(root, ("carfrom_" + myIDstr).c_str());
		if(r) stepFrom = json_integer_value(r);
		r = json_object_get(root, ("carto_" + myIDstr).c_str());
		if(r) stepTo = json_integer_value(r);
	}

	json_t *dataToJson(json_t *rootJ)
	{
		json_object_set_new(rootJ, ("cardir_" + myIDstr).c_str(), json_integer(direction));
		json_object_set_new(rootJ, ("carcoll_" + myIDstr).c_str(), json_integer(collision));
		json_object_set_new(rootJ, ("carfrom_" + myIDstr).c_str(), json_integer(stepFrom));
		json_object_set_new(rootJ, ("carto_" + myIDstr).c_str(), json_integer(stepTo));
		return rootJ;
	}

	inline int getLap() const {return lapCounter / NORDSTEPS;}

	// ------------------------ race control ---------------------------
	void process();

private:
	void reset();	
	void beginPulse(bool silent);
	void endPulse();
	int move_next();
	void ledOff();
	void ledOn();

private:
	Nordschleife *pNord = NULL;
	bool moving_bwd;
	int myID;
	int curStepCounter;
	int step_n =0;
	std::string myIDstr;
	SchmittTrigger2 clockTrigger;
	dsp::SchmittTrigger resetTrig;
	int lightid;
	int lapCounter;
};

struct NordschleifeStep
{
	enum StepMode { Off, On, Skip, Legato, Reset };
	StepMode mode = On;
	int outA = 0;
	int outB = 1;
	int probability = 100;
	int repeats = 1;

	void dataFromJson(json_t *root, std::string myID)
	{
		json_t *r = json_object_get(root, ("stepmode_"+myID).c_str());
		if(r) mode = (StepMode)json_integer_value(r);
		r = json_object_get(root, ("stepouta_"+myID).c_str());
		if(r) outA = (StepMode)json_integer_value(r);
		r = json_object_get(root, ("stepoutb_"+myID).c_str());
		if(r) outB = (StepMode)json_integer_value(r);
		r = json_object_get(root, ("stepprob_"+myID).c_str());
		if(r) probability = (StepMode)json_integer_value(r);
		r = json_object_get(root, ("stepreps_"+myID).c_str());
		if(r) repeats = (StepMode)json_integer_value(r);
	}

	json_t *dataToJson(json_t *rootJ, std::string myID)
	{
		json_object_set_new(rootJ, ("stepmode_" + myID).c_str(), json_integer(mode));
		json_object_set_new(rootJ, ("step_outa" + myID).c_str(), json_integer(outA));
		json_object_set_new(rootJ, ("step_outb" + myID).c_str(), json_integer(outB));
		json_object_set_new(rootJ, ("stepprob_" + myID).c_str(), json_integer(probability));
		json_object_set_new(rootJ, ("stepreps_" + myID).c_str(), json_integer(repeats));
		return rootJ;
	}
};

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
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		cvs.configure(this, NUM_PARAMS - cvStrip::CVSTRIP_PARAMS);

		int l[NORDCARS] = {LOTUS_LED, BRABHAM_LED, FERRARI_LED, HESKETH_LED};
		for(int k = 0; k < NORDCARS; k++)
			cars[k].Init(this, k, l[k]);
	}

	void process(const ProcessArgs &args) override;

	cvStrip cvs;
	int theRandomizer;
	NordschleifeField nsFields[NORDFIELDS];
	NordschleifeCar cars[NORDCARS];
	NordschleifeStep steps[NORDSTEPS];
	int selectedCar = 0;
	int selectedStep = 0;
	static int paths[12][64];
	static std::vector<std::string> pathNames;
	static std::vector<std::string> carNames;

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
		for(int k = 0; k < NORDCARS; k++)
			cars[k].dataFromJson(root);
		
		for(int k = 0; k < NORDSTEPS; k++)
			steps[k].dataFromJson(root, std::to_string(k));

		on_loaded();
	}
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *rndJson = json_integer(theRandomizer);
		json_object_set_new(rootJ, "theRandomizer", rndJson);
		for(int k = 0; k < NORDCARS; k++)
			cars[k].dataToJson(rootJ);
		
		for(int k = 0; k < NORDSTEPS; k++)
			steps[k].dataToJson(rootJ, std::to_string(k));
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
		setCar(0);
		setStep(0);
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
	NordschleifeWidget *pWidget;
	nordDisplay *display;
};
