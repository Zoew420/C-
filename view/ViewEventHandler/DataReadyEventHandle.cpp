#include "DataReadyEventHandle.h"

void DataReadyEventHandler::handle(const vector<ParticleInfo>& particles)
{
	view->Handler(const vector<ParticleInfo>& particles);
}
