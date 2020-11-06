#include "../include/rr.hpp"

void RR::process(const ProcessArgs &args)
{
	if(inputs[CLOCK_IN].isConnected())
	{
		bool mode_sequential = isSwitchOn(this, MODE);
		float deltaTime = 1.0 / args.sampleRate;
		int clk = clockTrigger.process(inputs[CLOCK_IN].getVoltage()); // 1=rise, -1=fall
		bool desc = isSwitchOn(this,EDGE);
		if(clk == 1)
		{
			lastclockpulse = 0;
			if(!desc)
			{
				rr = next(mode_sequential);
				if(rr >= 0)
				{
					outputs[OUT1+rr].setVoltage(LVL_ON);
				}
			} else
			{
				if(rr >= 0)
				{
					descSignal.reset();
					pulseRunning = false;
					outputs[OUT1+rr].setVoltage(LVL_OFF);
				}
			}
		} else if(clk == -1)
		{
			if(desc)
			{
				rr = next(mode_sequential);
				if(rr >= 0)
				{
					descSignal.trigger(lastclockpulse);
					outputs[OUT1+rr].setVoltage(LVL_ON);
					pulseRunning = true;
				}
			} else
			{
				if(rr >= 0)
				{
					outputs[OUT1+rr].setVoltage(LVL_OFF);
				}
			}
		} else
		{
			if(pulseRunning && !descSignal.process(deltaTime))
			{
				pulseRunning = false;
				outputs[OUT1+rr].setVoltage(LVL_OFF);
			} 
			lastclockpulse += deltaTime;
		}
	}
}

int RR::next(bool mode_sequential){return mode_sequential ? next_seq() : next();}

int RR::next_seq()
{
	if(n>=0)
		lights[LD1+n].value = LED_OFF;

	int last_connected = 0;
	for(int k=0; k < 8; k++)
	{
		if(outputs[OUT1+k].isConnected())
			last_connected=k;
	}
	n = (n+1)%(1+last_connected);
	if(outputs[OUT1+n].isConnected())
	{
		lights[LD1+n].value = LED_ON;
		return n;
	}
	descSignal.reset();
	pulseRunning = false;
	return -1;
}

int RR::next()
{
	n=rr;
	if(n>=0)
		lights[LD1+n].value = LED_OFF;

	for(int k=0; k < 8; k++)
	{
		n = (n+1)%8;
		if(outputs[OUT1+n].isConnected())
		{
			lights[LD1+n].value = LED_ON;
			return n;
		}
	}
	descSignal.reset();
	pulseRunning = false;
	return -1;
}

RRWidget::RRWidget(RR *module)
{
	CREATE_PANEL(module, this, 4, "res/modules/rr.svg");
	addInput(createInput<PJ301RPort>(Vec(mm2px(2.328), yncscape(113.610, 8.255)), module, RR::CLOCK_IN));
	addParam(createParam<TL1105HSw>(Vec(mm2px(6.857), yncscape(106.556, 4.477)), module, RR::EDGE));
	addParam(createParam<TL1105Sw>(Vec(mm2px(13.195), yncscape(114.063,6.607)), module, RR::MODE));

	float y_inc=12.763;
	for(int k=0; k < 8; k++)
	{
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(7.816), yncscape(94.932-k*y_inc, 8.255)), module, RR::OUT1+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(4.136), yncscape(100.617-k*y_inc, 2.176)), module, RR::LD1+k));
	}
}

