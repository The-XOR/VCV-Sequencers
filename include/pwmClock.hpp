#pragma once
#include "common.hpp"
#define BPM_MINVALUE (10.0)
#define BPM_MAXVALUE (300.0)
#define PWM_MINVALUE (0.05)
#define PWM_MAXVALUE (0.95)
#define SWING_MINVALUE (0.0)
#define SWING_MAXVALUE (0.5)

#define OUT_SOCKETS (21)
struct PwmClock;
struct PwmClockWidget : SequencerWidget
{
	PwmClockWidget(PwmClock *module);
	void SetBpm(float bpm);

};

struct SA_TIMER	//sample accurate version
{
	float Reset(float now)
	{
		prevTime = curTime = now;
		return Begin();
	}

	void RestartStopWatch() { stopwatch = 0; }
	float Begin()
	{
		RestartStopWatch();
		return totalPulseTime = 0;
	}
	inline float Elapsed() { return totalPulseTime; }
	float StopWatch() { return stopwatch; }

	float Step(float now)
	{
		curTime += now;
		float deltaTime = curTime - prevTime;
		prevTime = curTime;
		totalPulseTime += deltaTime;
		stopwatch += deltaTime;
		return deltaTime;
	}

private:
	float curTime;
	float prevTime;
	float totalPulseTime;
	float stopwatch;
};

struct PwmClock : Module
{
	enum ParamIds
	{
		BPM_INC, BPM_DEC,
		PWM, BPM, 
		SWING,
		OFFON,
		PULSE,
		NUM_PARAMS
	};
	enum InputIds
	{
		RESET,
		EXT_BPM,
		PWM_IN,
		SWING_IN,
		REMOTE_IN,
		PULSE_IN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		OUT_1,
		ONSTOP = OUT_1 + OUT_SOCKETS,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		ACTIVE,
		NUM_LIGHTS
	};

	PwmClock() : Module()
	{
		pWidget = NULL;
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PwmClock::BPM_INC, 0.0, 1.0, 0.0);
		configParam(PwmClock::BPM_DEC, 0.0, 1.0, 0.0);
		configParam(PwmClock::BPM, BPM_MINVALUE, BPM_MAXVALUE, 120.0, "BPM","Beats per minute");
		configParam(PwmClock::OFFON, 0.0, 1.0, 0.0);
		configParam(PwmClock::SWING, SWING_MINVALUE, SWING_MAXVALUE, SWING_MINVALUE, "Swing", "%", 0, 100);
		configParam(PwmClock::PWM, PWM_MINVALUE, PWM_MAXVALUE, 0.5, "Duty cycle", "%", 0, 100);

		on_loaded();
	}
	void process(const ProcessArgs &args) override;

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		json_t *bpmJson = json_real(bpm);
		json_object_set_new(rootJ, "bpm", bpmJson);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
		json_t *bpmJson = json_object_get(rootJ, "bpm");
		if(bpmJson)
			bpm = json_number_value(bpmJson);
		on_loaded();
	}

	void onReset() override
	{
		bpm = 120;

		load();
	}
	void onRandomize() override {}
	void setWidget(PwmClockWidget *pwdg) { pWidget = pwdg; }
	float bpm;
	float swing;

private:
	dsp::SchmittTrigger btnup;
	dsp::SchmittTrigger btndwn;
	PwmClockWidget *pWidget;
	bool current_status;
	SchmittTrigger2 resetTrigger;
	SchmittTrigger2 pulseTrigger;
	SchmittTrigger2 onTrigger;
	SchmittTrigger2 offTrigger;
	dsp::SchmittTrigger manualTrigger;
	dsp::PulseGenerator onStopPulse;
	dsp::PulseGenerator onManualStep;
	bool optimize_manualStep;
	float sampleRate;

	void process_keys();
	void updateBpm(float sr);
	void process_active(const ProcessArgs &args);
	void process_inactive(const ProcessArgs &args);
	
	inline float getDuration(int n) 	{return odd_beat[n] ? swingAmt[n] : duration[n]; }
	float duration[OUT_SOCKETS];
	float swingAmt[OUT_SOCKETS];
	
	bool odd_beat[OUT_SOCKETS];
	void on_loaded();
	void _reset();
	inline float getPwm() { return getModulableParam(this, PWM, PWM_IN, PWM_MINVALUE, PWM_MAXVALUE); }

	inline float getSwing() { return getModulableParam(this, SWING, SWING_IN, SWING_MINVALUE, SWING_MAXVALUE); }
	bool isGeneratorActive();
	SA_TIMER sa_timer[OUT_SOCKETS];
};
