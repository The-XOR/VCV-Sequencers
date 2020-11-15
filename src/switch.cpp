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

	float in_x = mm2px(1.569);
	float mod_x = mm2px(26.969);
	float trig_x = mm2px(35.171);
	float sw_x = mm2px(8.523);
	float led_x = mm2px(16.958);
	float out_x = mm2px(43.373);
	float y = 95.964;
	float yled = y+5.701;
	float ysw = y+0.797;
	float yinv = y+0.661;
	float delta_y = 76.649 - y;
	
	addInput(createInput<portYSmall>(Vec(mm2px(4.136), yncscape(6.967, 5.885)), module, XSwitch::RESET));
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		addInput(createInput<portGRSmall>(Vec(in_x, yncscape(y, 5.885)), module, XSwitch::IN_1 + k));
		addParam(createParam<LevettaR>(Vec(sw_x, yncscape(ysw, 7.336)), module, XSwitch::SW_1+k));
		addChild(createLight<SmallLight<RedLight>>(Vec(led_x, yncscape(yled, 2.176)), module, XSwitch::LED_1 + k ));
		addParam(createParam<TL1105Sw>(Vec(mm2px(20.380), yncscape(yinv, 5.460)), module, XSwitch::INV_1+k));
		addInput(createInput<portSmall>(Vec(mod_x, yncscape(y, 5.885)), module, XSwitch::MOD_1 + k));
		addInput(createInput<portBLUSmall>(Vec(trig_x, yncscape(y, 5.885)), module, XSwitch::TRIG_IN + k));
		addOutput(createOutput<portGSmall>(Vec(out_x, yncscape(y, 5.885)), module, XSwitch::OUT_1+k));
		y += delta_y;
		ysw += delta_y;
		yled += delta_y;
		yinv += delta_y;
	}
	addParam(createParam<TL1105HBSw>(Vec(mm2px(22.097), yncscape(112.772, 4.477f)), module, XSwitch::OUTMODE));
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
	