#ifndef SIGNAL_H
#define SIGNAL_H

#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>

#ifdef DEBUG
#include <iostream>
#endif

template<typename... T>
class Signal;

template<typename... Args>
class Signal<void, Args...> {
	std::vector<std::function<void(Args...)>> function;
public:
	template<typename... Args2>
	void operator()(Args2&&... args2) {

		#ifdef DEBUG
		if (function.empty())
			std::cout << "WARNING: Using empty signal!!" << std::endl;
		#endif

		for (auto& func : function)
			func(std::forward<Args2>(args2)...);
	}

	template<typename Func>
	void connect(Func const &func) {
		function.emplace_back(func);
	}

	template<typename Class, typename Instance>
	void connect(void(Class::*func)(Args...), Instance instance) {
		function.emplace_back([instance, func](Args&&... args){
			(instance->*func)(std::forward<Args>(args)...);
		});
	}

	void disconnect() {
		function.clear();
	}

	static Signal& get(std::string signal_name) {
		static std::unordered_map<std::string, Signal> signal_map;

		if (signal_map.find(signal_name) == signal_map.end())
			signal_map[signal_name] = Signal();

		return signal_map[signal_name];
	}
};

template<typename ReturnType, typename... Args>
class Signal<ReturnType, Args...> {
	std::function<ReturnType(Args...)> function;
public:
	template<typename... Args2>
	ReturnType operator()(Args2&&... args2) {

		#ifdef DEBUG
		if (!function)
			std::cout << "WARNING: Using empty signal!!" << std::endl;
		#endif

		return function(std::forward<Args2>(args2)...);
	}

	template<typename Func>
	void connect(Func const &func) {
		function = func;
	}

	template<typename Class, typename Instance>
	void connect(ReturnType(Class::*func)(Args...), Instance instance) {
		function = [instance, func](Args&&... args) -> ReturnType {
			return (instance->*func)(std::forward<Args>(args)...);
		};
	}

	static Signal& get(std::string signal_name) {
		static std::unordered_map<std::string, Signal> signal_map;
		return signal_map[signal_name];
	}
};

namespace sf { class String; class Packet; class Time; }
enum class GameStates : unsigned int;
class ObsField;

struct GameplayData {
	uint16_t linesSent=0;
	uint16_t linesRecieved=0;
	uint16_t bpm=0;
	uint16_t garbageCleared=0;
	uint16_t linesCleared=0;
	uint16_t pieceCount=0;
	uint16_t linesBlocked=0;
	uint16_t maxCombo=0;
	uint16_t roundDuration=0;
	void clear() {
		linesSent=0;
		linesRecieved=0;
		bpm=0;
		garbageCleared=0;
		linesCleared=0;
		pieceCount=0;
		linesBlocked=0;
		maxCombo=0;
		roundDuration=0;
	}
};

// Signal connection functions

template<typename T>
struct SignalSignature {};

template<typename ReturnType, typename Class, typename... Args>
struct SignalSignature<ReturnType(Class::*)(Args...)> {
	using Type = Signal<ReturnType, Args...>;
};

template<typename ReturnType, typename Class, typename... Args>
struct SignalSignature<ReturnType(Class::*)(Args...) const> {
	using Type = Signal<ReturnType, Args...>;
};

template<typename Func>
void connectSignal(std::string signal_name, Func func) {
	using Type = typename SignalSignature<decltype(&Func::operator())>::Type;
	Type::get(signal_name).connect(std::forward<Func&&>(func));
}

template<typename Func, typename Instance>
void connectSignal(std::string signal_name, Func func, Instance instance) {
	using Type = typename SignalSignature<Func>::Type;
	Type::get(signal_name).connect(std::forward<Func&&>(func), std::forward<Instance&&>(instance));
}

void SendSignal(int x, int y=-1, int z=-1);

/* GUI Elements (Hide/Show ect)				Sounds (PlaySound)
-------------------------------------------------------
0	mainMenu							0	pieceDrop
1	loginBox							1	lineClear
2	gameOptions							2	garbAdd
3	connectingScreen					3	lineBlock
4	onlineplayUI						4	menuSel
5	areYouSure							5	menuBack
6	performanceOutput					6	combo5
7	bugReport							7	combo8
8	challengesGameUI					8	combo11
9	replayUI							9	combo13
10	gameStandings						10	combo15
11	chatScreen							11	combo17
12	slideMenu							12	combo19
13	scoreScreen							13	combo21
14	serverUI							14	startBeep1
15	alertsUI							15	startBeep2
										16	alert
*/

// -------------- Packet delegation

struct SignalHolder {
	SignalHolder(std::size_t _size=-1) : size(_size) {}
	virtual ~SignalHolder() {}
	std::size_t size;
};

template<typename... T>
struct SignalArg : public SignalHolder {
	Signal<T...> function;
	SignalArg() : SignalHolder(sizeof...(T)-1) {}
};

template<typename T>
struct SignalArgs {};

template<typename ReturnType, typename Class, typename... Args>
struct SignalArgs<ReturnType(Class::*)(Args...)> {
	using type = SignalArg<void, Args...>;
};

template<typename ReturnType, typename Class, typename... Args>
struct SignalArgs<ReturnType(Class::*)(Args...) const> {
	using type = SignalArg<void, Args...>;
};

class Net {
	static std::vector<Signal<void, sf::Packet&>> packetReceivers;
	static std::vector<std::unique_ptr<SignalHolder>> signalReceivers;
public:
	template<typename Func>
	static void takePacket(std::size_t id, const Func &func) {
		if (id >= packetReceivers.size())
			packetReceivers.resize(id+1);
		packetReceivers[id].connect(func);
	}

	template<typename Func, typename Instance>
	static void takePacket(std::size_t id, const Func &func, const Instance &instance) {
		if (id >= packetReceivers.size())
			packetReceivers.resize(id+1);
		packetReceivers[id].connect(func, instance);
	}

	template<typename Func>
	static void takeSignal(std::size_t id, const Func &func) {
		if (id >= signalReceivers.size())
			signalReceivers.resize(id+1);
		typedef typename SignalArgs<decltype(&Func::operator())>::type type;
		signalReceivers[id] = std::unique_ptr<SignalHolder>(new type);
		SignalHolder &ref = *signalReceivers[id];
		static_cast<type&>(ref).function.connect(func);
	}

	template<typename Func, typename Instance>
	static void takeSignal(std::size_t id, const Func func, const Instance &instance) {
		if (id >= signalReceivers.size())
			signalReceivers.resize(id+1);
		typedef typename SignalArgs<Func>::type type;
		signalReceivers[id] = std::unique_ptr<SignalHolder>(new type);
		SignalHolder &ref = *signalReceivers[id];
		static_cast<type&>(ref).function.connect(func, instance);
	}

	static bool passOnPacket(std::size_t id, sf::Packet& packet);

	template<typename... Args>
	static bool passOnSignal(std::size_t id, Args... args) {
		if (id < signalReceivers.size()) {
			SignalHolder &ref = *signalReceivers[id];
			if (ref.size != sizeof...(Args))
				return false;
			static_cast<SignalArg<void, Args...>&>(ref).function(std::forward<Args&&>(args)...);
			return true;
		}
		return false;
	}
};

#endif
