#include "../include/kExp.hpp"

void kExp::process(const ProcessArgs &args)
{
	float expander_out;
	uint8_t *p = (uint8_t *)&expander_out;
	if(IsExpansion(this, &expander_out, EXPPORT_KLEE, EXP_IN, EXP_LED))
	{
		if(inputs[CLOCK_IN].isConnected())
		{
			int clk = clockTrigger.process(inputs[CLOCK_IN].value); // 1=rise, -1=fall
			if(clk == 1)
				and_mask = 0xff;
			else if(clk == -1)
				and_mask = 0;
		} else
			and_mask = 0xff;
		
		int addr = isSwitchOn(this, SEQ_SELECT) ? 2 : 1;
		uint8_t bstat = and_mask & *(p+addr);
		
		float v_out = 0;
		for(int k = 0; k < 8; k++)
		{
			if(bstat & 0x01)
			{
				outputs[BUS_OUT+k].setVoltage(LVL_ON);
				lights[LD1+k].value = LED_ON;
				v_out += params[KNOB_1+k].value;
			} else
			{
				outputs[BUS_OUT+k].setVoltage(LVL_OFF);
				lights[LD1+k].value = LED_OFF;
			}
			bstat >>=1;
		}

		outputs[OUT].setVoltage(v_out);
	}

	for(int k = 0; k < 4; k++)
	{
		bool b = inputs[ANDIN_1+k].getNormalVoltage(0.0) != LVL_OFF && inputs[ANDIN_2+k].getNormalVoltage(0.0) != LVL_OFF;
		outputs[ANDOUT+k].setVoltage(b ? LVL_ON : LVL_OFF);
		lights[LDAND+k].value = b ? LED_ON : LED_OFF;
	}
}

kExpWidget::kExpWidget(kExp *module)
{
	CREATE_PANEL(module, this, 11, "res/modules/kexp.svg");

	addParam(createParam<TL1105Sw>(Vec(mm2px(20.93), yncscape(110.471, 6.607)), module, kExp::SEQ_SELECT));
	addInput(createInput<PJ301EXP>(Vec(mm2px(7.816), yncscape(111.234, 8.255)), module, kExp::EXP_IN));
	addInput(createInput<PJ301RPort>(Vec(mm2px(29.561), yncscape(111.234, 8.255)), module, kExp::CLOCK_IN));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(44.246), yncscape(111.234, 8.255)), module, kExp::OUT));
	addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(5.769), yncscape(118.410, 1.088)), module, kExp::EXP_LED));

	float y_inc=12.763;
	for(int k=0; k < 8; k++)
	{
		addParam(createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(44.734), yncscape(95.060-k*y_inc, 8.0)), module, kExp::KNOB_1+k));
		addOutput(createOutput<PJ301WPort>(Vec(mm2px(7.816), yncscape(94.932-k*y_inc, 8.255)), module, kExp::BUS_OUT+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(4.136), yncscape(100.617-k*y_inc, 2.176)), module, kExp::LD1+k));
	}

	for(int k=0; k < 4; k++)
	{
		addInput(createInput<portWSmall>(Vec(mm2px(20.226), yncscape(96.117-2*k*y_inc, 5.885)), module, kExp::ANDIN_1+k));
		addInput(createInput<portWSmall>(Vec(mm2px(20.226), yncscape(83.353-2*k*y_inc, 5.885)), module, kExp::ANDIN_2+k));
		addOutput(createOutput<portWSmall>(Vec(mm2px(30.746), yncscape(89.735-2*k*y_inc, 5.885)), module, kExp::ANDOUT+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(32.601), yncscape(97.972-2*k*y_inc, 2.176)), module, kExp::LDAND+k));
	}
}

