#include "../include/scheletone.hpp"

void scheletone::process(const ProcessArgs &args)
{

}


scheletoneWidget::scheletoneWidget(scheletone *module)
{
	CREATE_PANEL(module, this, 8, "res/modules/scheletone.svg");


	addParam(createParam<HiddenButton>(Vec(mm2px(17.78), yncscape(12.806, 5.08)), module, scheletone::));
	addInput(createInput<PJ301HPort>(Vec(mm2px(0), yncscape(0, 8.255)), module, scheletone::));
}

