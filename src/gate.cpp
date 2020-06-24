#include "../include/common.hpp"
#include "../include/gate.hpp"

void gate::process(const ProcessArgs &args)
{
	for(int k = 0; k < NUM_GATES; k++)
	{
		bool inb_active = inputs[CNTRL_1+k].getNormalVoltage(0.f) > 0.5f;
		outputs[OUT_1+k].setVoltage(inb_active ? inputs[IN_B1+k].getVoltage() : inputs[IN_A1+k].getVoltage());
		lights[LED_A1+k].setBrightness(inb_active ? 0 : 10);
		lights[LED_B1+k].setBrightness(inb_active ? 10 : 0);
	}
}

gateWidget::gateWidget(gate *module) : ModuleWidget()
{
	CREATE_PANEL(module, this, 10, "res/modules/gate.svg");

	float in_x = mm2px(2.500);
	float ctrl_x = mm2px(21.272);
	float led_x = mm2px(12.482);
	float out_x = mm2px(40.045);
	float y = 109.505;

	float in_y = 96.805-y;
	float ctrl_y = 103.155-y;
	float out_y = 103.155-y;
	float leda_y = 112.545-y;
	float ledb_y = 99.845-y;
	float delta_y = -30;
	
	for(int k = 0; k < NUM_GATES; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, gate::IN_A1 + k));
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y+in_y, 8.255)), module, gate::IN_B1 + k));
		addInput(createInput<PJ301BPort>(Vec(ctrl_x, yncscape(y+ctrl_y, 8.255)), module, gate::CNTRL_1 + k));

		addChild(createLight<SmallLight<RedLight>>(Vec(led_x, yncscape(y+leda_y, 2.176)), module, gate::LED_A1 + k ));
		addChild(createLight<SmallLight<RedLight>>(Vec(led_x, yncscape(y+ledb_y, 2.176)), module, gate::LED_B1 + k ));
		addOutput(createOutput<PJ301GPort>(Vec(out_x, yncscape(y+out_y, 8.255)), module, gate::OUT_1+k));

		y += delta_y;
	}
}

