#include "../include/kexp.hpp"

void kExp::process(const ProcessArgs &args)
{
	float expander_out;
	uint8_t *p = (uint8_t *)&expander_out;
	if(IsExpansion(this, &expander_out, EXPPORT_KLEE, EXP_IN, EXP_LED))
	{
		if(inputs[CLOCK_IN].isConnected())
		{
			int clk = clockTrigger.process(inputs[CLOCK_IN].getVoltage()); // 1=rise, -1=fall
			if(clk == 1)
				and_mask = 0xff;
			else if(clk == -1)
				and_mask = 0;
		} else
			and_mask = 0xff;
		
		bool seq_b = isSwitchOn(this, SEQ_SELECT);
		uint8_t astat = and_mask & *(p+1);
		uint8_t bstat = and_mask & *(p+2);
		
		float v_out = 0;
		for(int k = 0; k < 8; k++)
		{
			if(astat & 0x01)
			{
				outputs[BUS_OUTA+k].setVoltage(LVL_ON);
				lights[LDA+k].value = LED_ON;
				if(!seq_b)
					v_out += params[KNOB_1+k].value;
			} else
			{
				outputs[BUS_OUTA+k].setVoltage(LVL_OFF);
				lights[LDA+k].value = LED_OFF;
			}

			if(bstat & 0x01)
			{
				outputs[BUS_OUTB+k].setVoltage(LVL_ON);
				lights[LDB+k].value = LED_ON;
				if(seq_b)
					v_out += params[KNOB_1+k].value;	
			} else
			{
				outputs[BUS_OUTB+k].setVoltage(LVL_OFF);
				lights[LDB+k].value = LED_OFF;
			}			
			bstat >>= 1;
			astat >>=1;
		}

		outputs[OUT].setVoltage(v_out);
	}

	for(int k = 0; k < 4; k++)
	{
		bool a = inputs[ANDIN_1 + k].getNormalVoltage(0.0) > 0.5;
		bool b = inputs[ANDIN_2 + k].getNormalVoltage(0.0) > 0.5;
		outputs[ANDOUT + k].setVoltage(a && b ? LVL_ON : LVL_OFF);
		outputs[OROUT + k].setVoltage(a || b ? LVL_ON : LVL_OFF);
	}
}

kExpWidget::kExpWidget(kExp *module)
{
	CREATE_PANEL(module, this, 11, "res/modules/kexp.svg");

	addParam(createParam<TL1105Sw2>(Vec(mm2px(37.863), yncscape(112.059, 6.607)), module, kExp::SEQ_SELECT));
	addInput(createInput<PJ301EXP>(Vec(mm2px(7.816), yncscape(111.234, 8.255)), module, kExp::EXP_IN));
	addInput(createInput<PJ301RPort>(Vec(mm2px(23.793), yncscape(111.234, 8.255)), module, kExp::CLOCK_IN));
	addOutput(createOutput<PJ301GPort>(Vec(mm2px(44.246), yncscape(111.234, 8.255)), module, kExp::OUT));
	addChild(createLight<TinyLight<WhiteLight>>(Vec(mm2px(5.769), yncscape(118.410, 1.088)), module, kExp::EXP_LED));

	float y_inc=12.763;
	for(int k=0; k < 8; k++)
	{
		addParam(createParam<Davies1900hFixRedKnobSmall>(Vec(mm2px(44.734), yncscape(95.060-k*y_inc, 8.0)), module, kExp::KNOB_1+k));
		addOutput(createOutput<portWSmall>(Vec(mm2px(4.768), yncscape(96.117-k*y_inc, 5.885)), module, kExp::BUS_OUTA+k));
		addChild(createLight<SmallLight<RedLight>>(Vec(mm2px(2.592), yncscape(100.617-k*y_inc, 2.176)), module, kExp::LDA+k));
		addOutput(createOutput<portWSmall>(Vec(mm2px(12.539), yncscape(96.117-k*y_inc, 5.885)), module, kExp::BUS_OUTB+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(mm2px(18.424), yncscape(100.617-k*y_inc, 2.176)), module, kExp::LDB+k));
	}

	for(int k=0; k < 4; k++)
	{
		addInput(createInput<portWSmall>(Vec(mm2px(24.459), yncscape(96.117-2*k*y_inc, 5.885)), module, kExp::ANDIN_1+k));
		addInput(createInput<portWSmall>(Vec(mm2px(24.459), yncscape(83.353-2*k*y_inc, 5.885)), module, kExp::ANDIN_2+k));
		addOutput(createOutput<portWSmall>(Vec(mm2px(32.863), yncscape(94.530 - 2 * k * y_inc, 5.885)), module, kExp::OROUT + k));
		addOutput(createOutput<portWSmall>(Vec(mm2px(32.863), yncscape(84.941 - 2 * k * y_inc, 5.885)), module, kExp::ANDOUT + k));
	}
}

