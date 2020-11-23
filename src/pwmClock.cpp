#include "../include/pwmClock.hpp"
#include <GLFW/glfw3.h>



void PwmClock::on_loaded()
{
	current_status = false;
	bpm = 120;
	sampleRate= 0;
	_reset();
}

void PwmClock::_reset()
{
	float numTicks[OUT_SOCKETS] =
	{
		//nor         dot (3/2)        trip (2/3)
		/*1/1*/ 4.f, 1.5f * 4.f, 0.66666666f * 4.f,
		/*1/2*/ 2.f, 1.5f * 2.f, 0.66666666f * 2.f,
		/*1/4*/ 1.f, 1.5f * 1.f, 0.66666666f * 1.f,
		/*1/8*/ 0.5f, 1.5f * 0.5f, 0.66666666f * 0.5f,
		/*1/16*/ 0.25f, 1.5f * 0.25f, 0.66666666f * 0.25f,
		/*1/32*/ 0.125f, 1.5f * 0.125f, 0.66666666f * 0.125f,
		/*1/64*/ 0.0625f, 1.5f * 0.0625f, 0.66666666f * 0.0625f
	};
	optimize_manualStep = false;
	for(int k=0; k < OUT_SOCKETS;k++)
		_timer[k].Reset(numTicks[k]);
	sampleRate= 0;
}

void PwmClock::updateBpm(float sr)
{
	process_keys();
	float new_bpm = clamp(roundf(getModulableParam(this, BPM, EXT_BPM, BPM_MINVALUE, BPM_MAXVALUE)), BPM_MINVALUE, BPM_MAXVALUE);

	if (bpm != new_bpm || sr != sampleRate)
	{
		bpm = new_bpm;
		sampleRate = sr;
		for (int k = 0; k < OUT_SOCKETS; k++)
			_timer[k].Calc(sampleRate, bpm);
	}
}

void PwmClock::process_keys()
{
	if(btnup.process(params[BPM_INC].value))
	{
		if(bpm < BPM_MAXVALUE)
		{
			bpm = round(bpm+1);
			pWidget->SetBpm(bpm);
		}
	} else if(btndwn.process(params[BPM_DEC].value))
	{
		if(bpm > BPM_MINVALUE)
		{
			bpm = round(bpm-1);
			pWidget->SetBpm(bpm);
		}
	}
}

void PwmClock::process_active(const ProcessArgs &args)
{
	onStopPulse.reset();
	onManualStep.reset();
	if(resetTrigger.process(inputs[RESET].getVoltage()))
	{
		_reset();
	} else 
	{
		float pwm = getPwm();
		float swing = getSwing();

		for(int k = 0; k < OUT_SOCKETS; k++)
		{
			if(outputs[OUT_1 + k].isConnected())
			{
				outputs[OUT_1 + k].setVoltage(_timer[k].Process(pwm, args.sampleTime, swing) ? LVL_ON : LVL_OFF);
			}
		}
	}
}

void PwmClock::process_inactive(const ProcessArgs &args)
{
	float deltaTime = 1.0 / args.sampleRate;

	if(current_status && !onStopPulse.process(deltaTime))
		onStopPulse.trigger(PULSE_TIME);

	if(!onManualStep.process(deltaTime))
	{
		if(optimize_manualStep)
		{
			optimize_manualStep = false;
			for(int k = 0; k < OUT_SOCKETS; k++)
				outputs[OUT_1 + k].setVoltage( LVL_OFF);

			lights[ACTIVE].value = LED_OFF;
		}
		if((manualTrigger.process(params[PULSE].value) || pulseTrigger.process(inputs[PULSE_IN].getVoltage())))
		{
			onManualStep.trigger(PULSE_TIME);
			optimize_manualStep = true;
			for(int k = 0; k < OUT_SOCKETS; k++)
			{
				outputs[OUT_1 + k].setVoltage( LVL_ON);
			}
			lights[ACTIVE].value = LED_ON;
		}
	}
	outputs[ONSTOP].setVoltage( onStopPulse.process(deltaTime) ? LVL_ON : LVL_OFF);
}

bool PwmClock::isGeneratorActive()
{
	bool active = false;
	if(inputs[REMOTE_IN].isConnected()) // priorita; prioritaria
	{
		active = inputs[REMOTE_IN].getVoltage() > 0.5f;
		if(active && !isSwitchOn(this, OFFON))
		{
			pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 1.0;
		} else if(!active && isSwitchOn(this, OFFON))
		{
			pWidget->params[OFFON]->dirtyValue = params[OFFON].value = 0.0;
		}
	} else
	{
		active = isSwitchOn(this, OFFON);
	}
	return active;
}

void PwmClock::process(const ProcessArgs &args)
{
	if(pWidget == NULL)
		return;

	bool active = isGeneratorActive();

	updateBpm(args.sampleRate);

	if(active)
	{
		process_active(args);
	} else
	{
		process_inactive(args);
	}

	if(active != current_status)
	{
		current_status = active;
		_reset();
		if(active)
		{
			lights[ACTIVE].value = LED_ON;
		} else
		{
			lights[ACTIVE].value = LED_OFF;
			for(int k = 0; k < OUT_SOCKETS; k++)
				outputs[OUT_1 + k].setVoltage( LVL_OFF);
		}
	}
}

PwmClockWidget::PwmClockWidget(PwmClock *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 7, "res/modules/PwmClock.svg");

	
	SigDisplayWidget *display = new SigDisplayWidget(3);
	display->box.pos = Vec(mm2px(18.838), mm2px(5.));
	display->box.size = Vec(30 + 14, 20);
	if(module != NULL)
		display->value = &module->bpm;
	addChild(display);

	

	ParamWidget *pw = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(7.753), yncscape(115.403, 9.525)), module, PwmClock::BPM);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addInput(createInput<portSmall>(Vec(mm2px(25.464), yncscape(108.276, 5.885)), module, PwmClock::EXT_BPM));

	addParam(createParam<UPSWITCH>(Vec(mm2px(1.562), yncscape(120.534, 4.115)), module, PwmClock::BPM_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(1.562), yncscape(115.733, 4.115)), module, PwmClock::BPM_DEC));

	addChild(createParam<CKD6>(Vec(mm2px(8.028), yncscape(101., 7.407)), module, PwmClock::PULSE));
	addInput(createInput<portSmall>(Vec(mm2px(25.464), yncscape(100.229, 5.885)), module, PwmClock::PULSE_IN));

	addParam(createParam<LevettaR>(Vec(mm2px(8.304), yncscape(89.447, 9.488)), module, PwmClock::OFFON));
	addChild(createLight<LargeLight<RedLight>>(Vec(mm2px(1.195), yncscape(90.208, 5.179)), module, PwmClock::ACTIVE));
	addInput(createInput<portSmall>(Vec(mm2px(25.464), yncscape(89.855, 5.885)), module, PwmClock::REMOTE_IN));
	
	addParam(createParam<daviesVerySmallRed>(Vec(mm2px(25.407), yncscape(79.424, 6)), module, PwmClock::PWM));
	addInput(createInput<portSmall>(Vec(mm2px(9.589), yncscape(79.481, 5.885)), module, PwmClock::PWM_IN));
	addParam(createParam<daviesVerySmallRed>(Vec(mm2px(25.407), yncscape(70.638, 6)), module, PwmClock::SWING));
	addInput(createInput<portSmall>(Vec(mm2px(9.589), yncscape(70.695, 5.885)), module, PwmClock::SWING_IN));
	addInput(createInput<portYSmall>(Vec(mm2px(25.464), yncscape(61.909, 5.885)), module, PwmClock::RESET));
	float col_x[3] = {9.589, 17.527, 25.464};
	float pos_y = 47.164;
	float dy =39.844 -pos_y;
	int col = 0;
	for(int k = 0; k < OUT_SOCKETS; k++)
	{
		addOutput(createOutput<portOSmall>(Vec(mm2px(col_x[col]), yncscape(pos_y, 5.885)), module, PwmClock::OUT_1 + k));
		if(++col >= 3)
		{
			col = 0;
			pos_y +=dy;
		}
	}
	addOutput(createOutput<portBLUSmall>(Vec(mm2px(9.589), yncscape(61.909, 5.885)), module, PwmClock::ONSTOP));
}

void PwmClockWidget::SetBpm(float bpm)
{
	int index = getParamIndex(PwmClock::BPM);
	if(index >= 0)
	{
		// VCV interface update is ahem.... migliorabile....
		Davies1900hFixWhiteKnob *pKnob = (Davies1900hFixWhiteKnob *)params[index];
		bool smooth = pKnob->smooth;
		pKnob->smooth = false;
		params[index]->paramQuantity->setValue(bpm);
		pKnob->smooth = smooth;
	}
}
