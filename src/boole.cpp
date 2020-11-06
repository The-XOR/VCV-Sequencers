#include "../include/common.hpp"
#include "../include/boole.hpp"

void Boole::process(const ProcessArgs &args)
{
	bool hiz = isSwitchOn(this, HIZ);
	bool compare = isSwitchOn(this, COMPAREMODE);
	for(int k = 0; k < NUM_BOOL_OP; k++)
	{
		bool o = process(k, hiz, compare);
		if(isSwitchOn(this, INVERT_1 + k))
			o = !o;
		lights[LED_OUT + k].value = o ? LED_ON : LED_OFF;
		outputs[OUT_1 + k].setVoltage( o ? LVL_ON : LVL_OFF);
	}
}

float Boole::getVoltage(int index, bool hiz)
{
	if (hiz && !inputs[index].isConnected())
	{
		if((hizCounter++ == 4000))
		{
			hizvalue = random::normal() * 2.3;
		} else if(hizCounter == 6000)
		{
			hizCounter = 0;
			hizvalue = 0;
		}
		return hizvalue;
	} 
	
	return inputs[index].getNormalVoltage(0.0);
}

bool Boole::logicLevel(float v1, float v2, bool compare)
{	
	return compare ? fabs(v1 - v2) < std::numeric_limits<float>::epsilon() : v1 > v2;
}

bool Boole::process(int num_op, bool hiz, bool compare)
{
	bool x = logicLevel(getVoltage(IN_X + num_op, hiz), params[THRESH_X + num_op].value, compare);
	lights[LED_X + num_op].value = x ? LED_ON : LED_OFF;
	if(num_op == 0)	// not?
		return !x;

	int mode = (int)roundf(params[Boole::OPMODE_1 + num_op-1].value);
	bool y = logicLevel(getVoltage(IN_Y + num_op-1, hiz), params[THRESH_Y + num_op-1].value, compare);
	lights[LED_Y + num_op - 1].value = y ? LED_ON : LED_OFF;
		
	switch(mode)
	{	
		case 0: return x && y;	//and
		case 1: return x || y;	//or
		case 2: return x ^ y;	//the xor
		case 3: return !x || y;	// implication
	}

	return false;
}

BooleWidget::BooleWidget(Boole *module) : ModuleWidget()
{
	CREATE_PANEL(module, this, 14, "res/modules/boole.svg");

	float in_x = mm2px(5.170);
	float in_led_x = mm2px(15.778);
	float out_led_x = mm2px(53.878);
	float pot_x = mm2px(20.561);
	float out_x = mm2px(58.091);
	float y = 112.349;
	float yout = 112.349;
	float ypot = 112.477;
	float yled = 115.389;
	float yinv = 97.892;
	float yled_out = 115.389;
	float delta_y =- 14.771;
	float sub_dy = -11.92;
	float out_dy = -26.691;
	
	for(int k = 0; k < NUM_BOOL_OP; k++)
	{
		// X
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, Boole::IN_X + k));
		addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(pot_x, yncscape(ypot, 8.0)), module, Boole::THRESH_X + k));
		addChild(createLight<SmallLight<RedLight>>(Vec(in_led_x, yncscape(yled, 2.176)), module, Boole::LED_X +k));

		// Y
		if(k > 0)
		{
			addParam(createParam<BoolSwitch>(Vec(mm2px(35.111), yncscape(ypot+4, 7.093)), module, Boole::OPMODE_1 + k - 1));
			y += sub_dy;
			ypot += sub_dy;
			yled += sub_dy;
			addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, Boole::IN_Y + k-1));
			addParam(createParam<Davies1900hFixWhiteKnobSmall>(Vec(pot_x, yncscape(ypot, 8.0)), module, Boole::THRESH_Y + k - 1));
			addChild(createLight<SmallLight<RedLight>>(Vec(in_led_x, yncscape(yled, 2.176)), module, Boole::LED_Y + k-1 ));
		}
		
		// OUT
		addOutput(createOutput<PJ301WPort>(Vec(out_x, yncscape(yout, 8.255)), module, Boole::OUT_1+k));
		addChild(createLight<SmallLight<WhiteLight>>(Vec(out_led_x, yncscape(yled_out, 2.176)), module, Boole::LED_OUT+k));

		if(k == 0)
		{
			addParam(createParam<TL1105Sw>(Vec(mm2px(52.727), yncscape(118.714, 6.607)), module, Boole::INVERT_1 + k));
			yled_out -= 20.731;
			yout -= 20.731;
		} else
		{
			addParam(createParam<TL1105Sw>(Vec(mm2px(52.727), yncscape(yinv,6.607)), module, Boole::INVERT_1 + k));
			yled_out += out_dy;
			yout += out_dy;
			yinv += out_dy;
		}

		y += delta_y;
		ypot += delta_y;
		yled += delta_y;
	}

	addParam(createParam<TL1105HSw>(Vec(mm2px(10.228), yncscape(122.202, 4.477)), module, Boole::COMPAREMODE));
	addParam(createParam<TL1105HSw>(Vec(mm2px(39.832), yncscape(2.319, 4.477)), module, Boole::HIZ));
}

