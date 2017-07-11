#include "BPMCount.h"

void BPMCount::addPiece(const sf::Time& _time) {
	bpmCount.push_front(_time);
}

sf::Uint16 BPMCount::calcBpm(const sf::Time& _time) {
	while (_time > bpmMeasureTiming) {
		bpmMeasureTiming+=sf::milliseconds(100);
		while (bpmCount.size()) {
			if (_time>bpmCount.back()+sf::seconds(5))
				bpmCount.pop_back();
			else
				break;
		}
		oldbpm[oldbpmCount] = bpmCount.size()*12;
		float total=0;
		for(int i=0; i<10; i++)
			total+=oldbpm[i];
		bpm=total/10;
		oldbpmCount++;
		if (oldbpmCount==10)
			oldbpmCount=0;
	}
	return bpm;
}

void BPMCount::clear() {
	bpmMeasureTiming=sf::seconds(0);
	oldbpmCount=0;
	bpm=0;
	for (int i=0; i<10; i++)
		oldbpm[i]=0;
	bpmCount.clear();
}