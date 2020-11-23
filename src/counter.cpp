#include "../include/counter.hpp"

void Counter::on_loaded()
{
	load();
}

void Counter::load()
{
	countDown = 0;
	reset();
}

void Counter::reset()
{
	curCounter = 0;
	outPulse.reset();
	toggle_status = false;
	lights[TOGGLESTAT].value = LED_OFF;
	outputs[OUT_TGL].setVoltage( LVL_OFF);
}

void Counter::process_keys()
{
	if(pWidget != NULL)
	{
		if(btnup.process(params[COUNTER_INC].value))
		{
			int n = roundf(counter_f);
			if(n < COUNTER_MAXVALUE)
			{
				n += 1;
				pWidget->SetCounter(n);
			}
		} else if(btndwn.process(params[COUNTER_DEC].value))
		{
			int n = roundf(counter_f);
			if(n > COUNTER_MINVALUE)
			{
				n -= 1;
				pWidget->SetCounter(n);
			}
		}
	}
}

void Counter::process(const ProcessArgs &args)
{
	bool oneshot_mode = isSwitchOn(this, ONESHOT);

	process_keys();
	int n = (int)roundf(getModulableParam(this, COUNTER, IN_COUNTER, COUNTER_MINVALUE, COUNTER_MAXVALUE));
	counter_f = n;
	
	countDown = n - curCounter;
	float deltaTime = 1.0 / args.sampleRate;

	if(resetTrigger.process(inputs[RESET].getVoltage()))
	{
		reset();

	} else 
	{
		if(!oneshot_mode || !toggle_status)
		{
			if(counterTigger.process(inputs[IN_1].getVoltage()))
			{
				++curCounter;
				if(curCounter >= n)
					trig_out();
			}
		}
	}

	if(outPulse.process(deltaTime))
	{
		lights[ACTIVE].value = LED_ON;
		outputs[OUT_1].setVoltage( LVL_ON);
	} else
	{
		lights[ACTIVE].value = LED_OFF;
		outputs[OUT_1].setVoltage( LVL_OFF);
	}
}

void Counter::trig_out()
{
	curCounter = 0;
	outPulse.trigger(PULSE_TIME);
	toggle_status = !toggle_status;
	lights[TOGGLESTAT].value = toggle_status ? LED_ON : LED_OFF;
	outputs[OUT_TGL].setVoltage(   toggle_status ? LVL_ON : LVL_OFF);
}

CounterWidget::CounterWidget(Counter *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 8, "res/modules/Counter.svg");

	addParam(createParam<UPSWITCH>(Vec(mm2px(2.281), yncscape(104.588, 4.115)), module, Counter::COUNTER_INC));
	addParam(createParam<DNSWITCH>(Vec(mm2px(2.281), yncscape(99.788, 4.115)), module, Counter::COUNTER_DEC));
	addParam(createParam<TL1105Sw>(Vec(mm2px(7.934), yncscape(84.759, 6.607)), module, Counter::ONESHOT));

	SigDisplayWidget *display = new SigDisplayWidget(3, 0);
	display->box.size = Vec(30 + 16, 22);
	display->box.pos = Vec(mm2px(7.934), yncscape(100.07, px2mm(display->box.size.y)));

	if(module != NULL)
		display->value = &module->counter_f;
	addChild(display);

	SigDisplayWidget *displayCtr = new SigDisplayWidget(3, 0);
	displayCtr->box.size = Vec(30 + 16, 22);
	displayCtr->box.pos = Vec(mm2px(12.418), yncscape(40.849, px2mm(display->box.size.y)));
	if(module != NULL)
		displayCtr->value = &module->countDown;
	addChild(displayCtr);

	ParamWidget *pw = createParam<Davies1900hFixWhiteKnob>(Vec(mm2px(28.0435), yncscape(99.483, 9.525)), module, Counter::COUNTER);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addInput(createInput<PJ301BPort>(Vec(mm2px(3.238), yncscape(12.664, 8.255)), module, Counter::IN_1));
	addInput(createInput<PJ301YPort>(Vec(mm2px(3.238), yncscape(28.287, 8.255)), module, Counter::RESET));
	addInput(createInput<PJ301BPort>(Vec(mm2px(29.070), yncscape(83.935, 8.255)), module, Counter::IN_COUNTER));

	addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(25.242), yncscape(15.703, 2.176)), module, Counter::ACTIVE));
	addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(25.242), yncscape(31.327, 2.176)), module, Counter::TOGGLESTAT));

	addOutput(createOutput<PJ301BLUPort>(Vec(mm2px(29.793), yncscape(12.664, 8.255)), module, Counter::OUT_1));
	addOutput(createOutput<PJ301WPort>(Vec(mm2px(29.793), yncscape(28.287, 8.255)), module, Counter::OUT_TGL));
}

void CounterWidget::SetCounter(int n)
{
	int index = getParamIndex(Counter::COUNTER);
	if(index >= 0)
	{
		// VCV interface update is ahem.... migliorabile....
		Davies1900hFixWhiteKnob *pKnob = (Davies1900hFixWhiteKnob *)params[index];
		bool smooth = pKnob->smooth;
		pKnob->smooth = false;
		params[index]->paramQuantity->setValue((float)n);
		pKnob->smooth = smooth;
	}
}
