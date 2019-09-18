#include "../include/rr.hpp"

void RR::process(const ProcessArgs &args)
{
	if(inputs[CLOCK_IN].isConnected())
	{
		int clk = clockTrigger.process(inputs[CLOCK_IN].value); // 1=rise, -1=fall
		if(isSwitchOn(this,EDGE))
			clk = -clk;

		if(clk == 1)
		{
			rr = next();
			if(rr >= 0)
			{
				outputs[OUT1+rr].setVoltage(LVL_ON);
				lights[LD1+rr].value = LED_ON;
			}
		} else if(clk == -1)
		{
			if(rr >= 0)
			{
				outputs[OUT1+rr].setVoltage(LVL_OFF);
				lights[LD1+rr].value = LED_OFF;
			}
		}
	}
}

int RR::next()
{
	int n=rr;
	for(int k=0; k < 8; k++)
	{
		n = (n+1)%8;
		if(outputs[OUT1+n].isConnected())
			return n;
	}
	return -1;
}

RRWidget::RRWidget(RR *module)
{
	CREATE_PANEL(module, this, 4, "res/modules/rr.svg");
	addInput(createInput<PJ301RPort>(Vec(mm2px(6.032), yncscape(113.610, 8.255)), module, RR::CLOCK_IN));
	addParam(createParam<TL1105HSw>(Vec(mm2px(6.857), yncscape(106.556, 4.477)), module, RR::EDGE));

	float y_inc=12.763;
	for(int k=0; k < 8; k++)
	{
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(7.816), yncscape(94.932-k*y_inc, 8.255)), module, RR::OUT1+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(4.136), yncscape(100.617-k*y_inc, 2.176)), module, RR::LD1+k));
	}
}

