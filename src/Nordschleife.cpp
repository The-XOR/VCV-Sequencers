#include "../include/Nordschleife.hpp"

void Nordschleife::process(const ProcessArgs &args)
{

}

NordschleifeWidget::NordschleifeWidget(Nordschleife *module)
{
	CREATE_PANEL(module, this, 40, "res/modules/Nordschleife.svg");

	//addParam(createParam<HiddenButton>(Vec(mm2px(17.78), yncscape(12.806, 5.08)), module, Nordschleife::MOTTBTN));
	//addInput(createInput<PJ301HPort>(Vec(mm2px(0), yncscape(0, 8.255)), module, Nordschleife::MOTTETTO));
}

