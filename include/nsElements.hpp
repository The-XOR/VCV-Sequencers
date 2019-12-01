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
	shlfOutA,
	shlfOutB,
	shlfMode,
	shlfProbab,
	shlfRepeats,
	shlfStepOffset,
	shlfTrigger,
	shlfAux,
	shlfDelay,

	NORDFIELDS
};

struct NordschleifeCar
{
	enum CarDirection {carForward, carBackward, carAlternate, carBrownian, carRandom   ,NUM_CAR_DIRECTIONS};
	enum CarCollision { carIgnore, carInvert, car90left, car90right, nextPath, prevPath, randomPath, carRnd     ,NUM_CAR_COLLISIONS};
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

	void stepTrig();

	// ------------------------ race control ---------------------------
	bool process(float deltaTime);
	void onCollision();	
	void reset();

private:
	int move_next(int carID);
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
	dsp::PulseGenerator stepTrigger;
	dsp::PulseGenerator ledStepTrigger;
	int totalCounter;
	int lapCounter;
	int pitStopCounter;
	bool pitstop;
};

struct NordschleifeStep
{

	enum StepMode { Off, On, Skip, Legato, Slide, Reset    ,NUM_STEP_MODE};
	bool outA[NORDCARS];
	bool outB[NORDCARS];
	StepMode mode[NORDCARS];
	int probability[NORDCARS];
	int repeats[NORDCARS];
	bool trigger[NORDCARS];
	float offset[NORDCARS];
	float aux[NORDCARS];
	int delay[NORDCARS];

	void dataFromJson(json_t *root, std::string myID)
	{
		for(int k = 0; k < NORDCARS; k++)
		{
			json_t *r = json_object_get(root, mkjson("stepouta", k).c_str());
			if(r) outA[k] = (StepMode)json_integer_value(r);
		
			r = json_object_get(root, mkjson("stepoutb", k).c_str());
			if(r) outB[k] = (StepMode)json_integer_value(r);

			r = json_object_get(root, mkjson("stepmode", k).c_str());
			if(r) mode[k] = (StepMode)json_integer_value(r);

			r = json_object_get(root, mkjson("stepprob", k).c_str());
			if(r) probability[k] = (StepMode)json_integer_value(r);

			r = json_object_get(root, mkjson("stepreps", k).c_str());
			if(r) repeats[k] = (StepMode)json_integer_value(r);

			r = json_object_get(root, mkjson("stepoffs", k).c_str());
			if(r) offset[k] = (StepMode)json_real_value(r);

			r = json_object_get(root, mkjson("steptrig", k).c_str());
			if(r) trigger[k] = json_integer_value(r) > 0;

			r = json_object_get(root, mkjson("stepaux", k).c_str());
			if(r) aux[k] = json_real_value(r);

			r = json_object_get(root, mkjson("stepdelay", k).c_str());
			if(r) delay[k] = (StepMode)json_integer_value(r);
		}
	}

	json_t *dataToJson(json_t *rootJ, std::string myID)
	{
		for(int k = 0; k < NORDCARS; k++)
		{
			json_object_set_new(rootJ, mkjson("step_outa", k).c_str(), json_integer(outA[k]));
			json_object_set_new(rootJ, mkjson("step_outb", k).c_str(), json_integer(outB[k]));
			json_object_set_new(rootJ, mkjson("stepmode" , k).c_str(), json_integer(mode[k]));
			json_object_set_new(rootJ, mkjson("stepprob" , k).c_str(), json_integer(probability[k]));
			json_object_set_new(rootJ, mkjson("stepreps" , k).c_str(), json_integer(repeats[k]));
			json_object_set_new(rootJ, mkjson("stepoffs" , k).c_str(), json_real(offset[k]));
			json_object_set_new(rootJ, mkjson("steptrig" , k).c_str(), json_integer(trigger[k] ? 1 : 0));
			json_object_set_new(rootJ, mkjson("stepaux"  , k).c_str(), json_real(aux[k]));
			json_object_set_new(rootJ, mkjson("stepdelay", k).c_str(), json_integer(delay[k]));
		}
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
		for(int k = 0; k < NORDCARS; k++)
		{
			outA[k] = outB[k] = false;
			mode[k] = On;
			offset[k] = 0;
			probability[k] = 100;
			repeats[k] = 1;
			trigger[k] = false;
			aux[k] = 0;
			delay[k] = 0;
		}
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
	float cvDelay[NORDCARS];

private:
	StepMode endPulse(Nordschleife *pNord, int carID);
	void process(Nordschleife *pNord, int carID, float elapsedTime);
	void mute(Nordschleife *pNord, int carID);
	std::string mkjson(std::string prefix, int k) {return prefix + "_"+std::to_string(myID)+"_"+std::to_string(k);}
};
