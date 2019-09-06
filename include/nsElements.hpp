#pragma once
#define STEP_RESET		-200

struct Nordschleife;
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

struct NordschleifeCar
{
	enum CarDirection {carForward, carBackward, carAlternate, carBrownian, carRandom };
	enum CarCollision { carIgnore, carInvert, car90left, car90right };
	CarDirection direction;
	CarCollision collision;
	int stepFrom;
	int stepTo;
	int path;

	void Init(Nordschleife *p, int id, int lightid);
	void init();

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
		r = json_object_get(root, ("carpath_" + myIDstr).c_str());
		if(r) path = json_integer_value(r);
	}

	json_t *dataToJson(json_t *rootJ)
	{
		json_object_set_new(rootJ, ("cardir_" + myIDstr).c_str(), json_integer(direction));
		json_object_set_new(rootJ, ("carcoll_" + myIDstr).c_str(), json_integer(collision));
		json_object_set_new(rootJ, ("carfrom_" + myIDstr).c_str(), json_integer(stepFrom));
		json_object_set_new(rootJ, ("carto_" + myIDstr).c_str(), json_integer(stepTo));
		json_object_set_new(rootJ, ("carpath_" + myIDstr).c_str(), json_integer(path));
		return rootJ;
	}

	std::string name;
	inline int getLap() const {return lapCounter / NORDSTEPS;}

	// ------------------------ race control ---------------------------
	int process(float deltaTime);
	void onCollision();
	inline bool crashWith(int stp) { return step_n != STEP_RESET && stp == step_n; }

private:
	int move_next();
	int get_next_step();
	inline void ledOff();
	inline void ledOn();
	void pulseTrig();
	void reset();

private:
	Nordschleife *pNord = NULL;
	bool moving_bwd;
	int myID;
	int curStepCounter;
	int step_n = 0;
	std::string myIDstr;
	SchmittTrigger2 clockTrigger;
	dsp::SchmittTrigger resetTrig;
	dsp::PulseGenerator lapPulse;
	int lightid;
	int lapCounter;
	int angle = 0;
	int repetitions;
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
	int beginPulse(Nordschleife *pNord, int carID);
	bool endPulse(Nordschleife *pNord, int carID, float deltaTime, bool end);
	void Init(int id)
	{
		myID = id;
	}
	int repCount;

private:
	int myID;
	dsp::PulseGenerator stepPulseA;
	dsp::PulseGenerator stepPulseB;

};
