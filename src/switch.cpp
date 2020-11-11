#include "../include/common.hpp"
#include "../include/switch.hpp"

void XSwitch::process(const ProcessArgs &args)
{
	if(pWidget == NULL)
		return;

	float last_value = 0;
	bool reset_state = resetInput.process(inputs[RESET].getVoltage());
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		if(inputs[IN_1 + k].isConnected())
			last_value = inputs[IN_1 + k].getVoltage();

		if(getSwitch(k, reset_state))
		{
			lights[LED_1 + k].value = LED_ON;
			outputs[OUT_1 + k].setVoltage( last_value);
		} else
		{
			lights[LED_1 + k].value = LED_OFF;
			if(!isSwitchOn(this, OUTMODE))
				outputs[OUT_1 + k].setVoltage( LVL_OFF);
		}
	}
}

SwitchWidget::SwitchWidget(XSwitch *module) : SequencerWidget()
{
	if(module != NULL)
		module->setWidget(this);

	CREATE_PANEL(module, this, 10, "res/modules/switch.svg");

	float in_x = mm2px(2.500);
	float mod_x = mm2px(17.306);
	float sw_x = mm2px(25.027);
	float led_x = mm2px(28.173);
	float out_x = mm2px(40.045);
	float y = 101.567;
	float y1 = 98.387;
	float yled = 114.949;
	float ysw = 105.667;
	float yinv = 99.785;
	float delta_y = 79.394 - 101.567;
	
	addInput(createInput<portYSmall>(Vec(mm2px(41.221), yncscape(116.530, 5.885)), module, XSwitch::RESET));
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, XSwitch::IN_1 + k));
		addInput(createInput<portSmall>(Vec(mod_x+5, yncscape(y1, 8.255)), module, XSwitch::MOD_1 + k));
		addInput(createInput<portBLUSmall>(Vec(mod_x-15, yncscape(y1, 8.255)), module, XSwitch::TRIG_IN + k));
		addParam(createParam<LevettaR>(Vec(sw_x, yncscape(ysw, 7.336)), module, XSwitch::SW_1+k));
		addParam(createParam<TL1105Sw>(Vec(mm2px(33.711), yncscape(yinv, 5.460)), module, XSwitch::INV_1+k));
		addChild(createLight<SmallLight<RedLight>>(Vec(led_x, yncscape(yled, 2.176)), module, XSwitch::LED_1 + k ));
		addOutput(createOutput<PJ301GPort>(Vec(out_x, yncscape(y, 8.255)), module, XSwitch::OUT_1+k));
		y += delta_y;
		y1 += delta_y;
		ysw += delta_y;
		yled += delta_y;
		yinv += delta_y;
	}
	addParam(createParam<TL1105HBSw>(Vec(mm2px(9.823), yncscape(116.927, 4.477)), module, XSwitch::OUTMODE));
}

void SwitchWidget::SetSwitch(int n, bool status)
{
	int index = getParamIndex(n);
	if(index >= 0)
	{
		//params[index]->dirtyValue = 
		params[index]->paramQuantity->setValue(status ? 1.0 : 0.0);
	}
}
	