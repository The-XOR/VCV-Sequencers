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
	shlfStartGrid,
	shlfAngle,
	shlfStrtgEvery,
	shlfStrtgFor,
	shlfOffset,

	shlfStep,
	shlfMode,
	shlfProbab,
	shlfRepeats,
	shlfStepOffset,
	shlfOutA,
	shlfOutB,

//shlfAux,
//shlfDelay,
//shlfGate,
//shlfTrigger,

	NORDFIELDS
};

struct NordschleifeCar
{
	enum CarDirection {carForward, carBackward, carAlternate, carBrownian, carRandom };
	enum CarCollision { carIgnore, carInvert, car90left, car90right, nextPath, prevPath, randomPath, carRnd };
	CarDirection direction;
	CarCollision collision;
	int stepFrom;
	int stepTo;
	int path;
	int startGrid;
	int strategyEvery;
	int strategyFor;
	int angle;
	int offset;

	void Init(Nordschleife *p, int id);
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
		r = json_object_get(root, ("carstartg_" + myIDstr).c_str());
		if(r) startGrid = json_integer_value(r);
		r = json_object_get(root, ("carpath_" + myIDstr).c_str());
		if(r) path = json_integer_value(r);
		r = json_object_get(root, ("carstrevery_" + myIDstr).c_str());
		if(r) strategyEvery = json_integer_value(r);
		r = json_object_get(root, ("carstrfor_" + myIDstr).c_str());
		if(r) strategyFor = json_integer_value(r);
		r = json_object_get(root, ("carangle_" + myIDstr).c_str());
		if(r) angle = json_integer_value(r);
		r = json_object_get(root, ("caroffset_" + myIDstr).c_str());
		if(r) offset = json_integer_value(r);
	}

	json_t *dataToJson(json_t *rootJ)
	{
		json_object_set_new(rootJ, ("cardir_" + myIDstr).c_str(), json_integer(direction));
		json_object_set_new(rootJ, ("carcoll_" + myIDstr).c_str(), json_integer(collision));
		json_object_set_new(rootJ, ("carfrom_" + myIDstr).c_str(), json_integer(stepFrom));
		json_object_set_new(rootJ, ("carto_" + myIDstr).c_str(), json_integer(stepTo));
		json_object_set_new(rootJ, ("carstartg_" + myIDstr).c_str(), json_integer(startGrid));
		json_object_set_new(rootJ, ("carpath_" + myIDstr).c_str(), json_integer(path));
		json_object_set_new(rootJ, ("carstrevery_" + myIDstr).c_str(), json_integer(strategyEvery));
		json_object_set_new(rootJ, ("carstrfor_" + myIDstr).c_str(), json_integer(strategyFor));
		json_object_set_new(rootJ, ("carangle_" + myIDstr).c_str(), json_integer(angle));
		json_object_set_new(rootJ, ("caroffset_" + myIDstr).c_str(), json_integer(offset));
		return rootJ;
	}

	std::string name;
	static int CarLed[NORDCARS];
	inline std::string getLap() const 
	{
		if(pitstop)
			return "Lap #"+std::to_string(lapCounter) + " *** PIT ***";
		else
			return "Lap #" + std::to_string(lapCounter);
	}

	// ------------------------ race control ---------------------------
	bool process(float deltaTime);
	void onCollision();	
	void reset();

private:
	int move_next();
	int get_next_step();
	void pulseTrig();
	bool inPit();
	void onCollision(CarCollision n);

private:
	float stopWatch;
	float lastPulseDuration;
	Nordschleife *pNord = NULL;
	bool moving_bwd;
	int myID;
	int curStepCounter;
	int playingStep;
	std::string myIDstr;
	SchmittTrigger2 clockTrigger;
	dsp::SchmittTrigger resetTrig;
	dsp::PulseGenerator lapPulse;
	dsp::PulseGenerator ledLapPulse;
	int totalCounter;
	int lapCounter;
	int pitStopCounter;
	bool pitstop;
};

struct NordschleifeStep
{
	enum StepMode { Off, On, Skip, Legato, Slide, Reset };
	StepMode mode;
	int outA;
	int outB;
	int probability;
	int repeats;
	int offset;

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
		r = json_object_get(root, ("stepoffs_" + myID).c_str());
		if(r) offset = (StepMode)json_integer_value(r);
	}
	json_t *dataToJson(json_t *rootJ, std::string myID)
	{
		json_object_set_new(rootJ, ("stepmode_" + myID).c_str(), json_integer(mode));
		json_object_set_new(rootJ, ("step_outa" + myID).c_str(), json_integer(outA));
		json_object_set_new(rootJ, ("step_outb" + myID).c_str(), json_integer(outB));
		json_object_set_new(rootJ, ("stepprob_" + myID).c_str(), json_integer(probability));
		json_object_set_new(rootJ, ("stepreps_" + myID).c_str(), json_integer(repeats));
		json_object_set_new(rootJ, ("stepoffs_" + myID).c_str(), json_integer(offset));
		return rootJ;
	}

	inline static void Mute(Nordschleife *pNord, int carID);
	inline static StepMode EndPulse(Nordschleife *pNord, int carID);
	inline static void  Process(Nordschleife *pNord, int carID, float deltaTime);
	void beginPulse(Nordschleife *pNord, int carID, float lastPulseDuration, int nextStep);
	static bool Collision(int carID)
	{
		if(NordschleifeStep::selectedByCar[carID] != STEP_RESET)
		{
			for(int k = 0; k < NORDCARS; k++)
			{
				if(carID != k && NordschleifeStep::selectedByCar[k] == NordschleifeStep::selectedByCar[carID])
					return true;
			}
		}

		return false;
	}

	void Init(int id)
	{
		myID = id;
		init();
	}
	void reset()
	{
		for(int k = 0; k < NORDCARS; k++)
		{
			repCount[k] = 0;
			playing[k] = false;
			NordschleifeStep::selectedByCar[k] = STEP_RESET;
		}
	}

	void init()
	{
		mode = On;
		outA = 0;
		outB = 1;
		offset = 0;
		probability = 100;
		repeats = 1;
		reset();
	}

private:
	int myID;
	dsp::PulseGenerator stepPulseB;
	int repCount[NORDCARS];
	static int selectedByCar[NORDCARS];
	bool playing[NORDCARS];
	float pulseDuration[NORDCARS];
	bool repeat_gateStatus[NORDCARS];
	float stopWatch[NORDCARS];
	float startVoltage[NORDCARS];
	float elapsedTime[NORDCARS];
	float slideToVoltage[NORDCARS];

private:
	StepMode endPulse(Nordschleife *pNord, int carID);
	void process(Nordschleife *pNord, int carID, float elapsedTime);
	void mute(Nordschleife *pNord, int carID);
};
