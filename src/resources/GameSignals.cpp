#include "GameSignals.h"

// Packet delegation

std::vector<Signal<void, sf::Packet&>> Net::packetReceivers;
std::vector<std::unique_ptr<SignalHolder>> Net::signalReceivers;

bool Net::passOnPacket(std::size_t id, sf::Packet& packet) {
	if (id < packetReceivers.size()) {
		packetReceivers[id](packet);
		return true;
	}
	return false;
}

static auto& SendNetSignal = Signal<void, int, int, int>::get("SendSignal");

void SendSignal(int x, int y, int z) {
	SendNetSignal(x,y,z);
}