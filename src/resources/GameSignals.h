#ifndef SIGNAL_H
#define SIGNAL_H

#include <vector>
#include <functional>
#include <memory>

template<typename... T>
class Signal;

template<typename... Args>
class Signal<void, Args...> {
	std::vector<std::function<void(Args...)>> function;
public:
	template<typename... Args2>
	void operator()(Args2&&... args2) {
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
};

template<typename ReturnType, typename... Args>
class Signal<ReturnType, Args...> {
	std::function<ReturnType(Args...)> function;
public:
	template<typename... Args2>
	ReturnType operator()(Args2&&... args2) {
			return function(std::forward<Args2>(args2)...);
	}

	template<typename Func>
	void connect(Func const &func) {
		function = func;
	}

	template<typename Class, typename Instance>
	void connect(ReturnType(Class::*func)(Args...), Instance instance) {
		function = [instance, func](Args&&... args){
			return (instance->*func)(std::forward<Args>(args)...);
		};
	}
};

namespace sf { class String; class Packet; class Time; }
enum class GameStates : unsigned int;

struct GameplayData {
	uint16_t linesSent=0;
	uint16_t linesRecieved=0;
	uint16_t linesPerMinute=0;
	uint16_t bpm=0;
	uint16_t garbageCleared=0;
	uint16_t linesCleared=0;
	uint16_t pieceCount=0;
	uint16_t linesBlocked=0;
	uint16_t maxCombo=0;
	void clear() {
		linesSent=0;
		linesRecieved=0;
		linesPerMinute=0;
		bpm=0;
		garbageCleared=0;
		linesCleared=0;
		pieceCount=0;
		linesBlocked=0;
		maxCombo=0;
	}
};

struct ReplayData {

};

struct Signals {
									//GUI
	static Signal<void, int>				Show;
	static Signal<void, int>				Hide;
	static Signal<void, int>				Enable;
	static Signal<void, int>				Disable;
	static Signal<bool, int>				IsVisible;
	static Signal<void>						ShowGameFields;
	static Signal<void>						HideGameFields;
	static Signal<bool>						GameFieldsIsVisible;
	static Signal<void, const sf::Time&>	EnableBackground;
	static Signal<void>						DisableBackground;
	static Signal<void>						ShowAlert;
	static Signal<void>						HideAlert;
	static Signal<void>						LeaveRoom;
	static Signal<void, int>				Disconnect;
	static Signal<void, GameStates>			SetGameState;
	static Signal<bool>						IsLoginThreadJoinable;
	static Signal<void>						TellPatcherToQuit;
	static Signal<bool>						ApplyPatch;
	static Signal<void, const sf::String&>	QuickMsg;
	static Signal<void>						LightTheme;
	static Signal<void>						DarkTheme;
	static Signal<void, int>				SetFieldsBackColor;
	static Signal<void, int, int>			SetFieldsSize;
	static Signal<void>						MakeBackgroundLines;
	static Signal<int, int>					AreThereFields;
	static Signal<void, const sf::String&>	SetConnectingText;
	static Signal<void>						SetRoomListTime;
	static Signal<void, const sf::String&>	SetAreYouSure;
	static Signal<void, const sf::String&>	AddAlert;
	static Signal<void>						HideStartChallengeButton;
	static Signal<void, int>				JoinRoom;
	static Signal<void, int>				ShowOptions;
	static Signal<void, GameplayData&>		UpdateGamedata;
	static Signal<void, GameplayData&>		UpdateChallengesUI;
	static Signal<void, sf::Time>			UpdateReplayUI;
	static Signal<bool>						Survivor;

									//Network
	static Signal<void, int, int, int>		SendSignal;
	static Signal<void, sf::Packet&>		SendPacket;
	static Signal<void, sf::Packet&>		SendPacketUDP;
	static Signal<void, int, int>			SendPing;

									//Sound
	static Signal<void, int>				PlaySound;
	static Signal<void, int>				SetEffectVolume;
	static Signal<void, int>				SetMusicVolume;
	static Signal<void, int>				SetAlertsVolume;
	static Signal<void>						EnableSound;
	static Signal<void>						DisableSound;

									//Game
	static Signal<void>						StartCountDown;
	static Signal<void, int>				GameOver;
	static Signal<void>						Ready;
	static Signal<void>						Away;
	static Signal<void, bool>				SetAway;
	static Signal<void>						SetDrawMe;
	static Signal<void, int>				SetGameBackColor;
	static Signal<void, const sf::String&>	SetName;
	static Signal<const sf::String&>		GetName;
	static Signal<void>						UpdateGamePieces;
	static Signal<GameplayData&>			GetGameData;
	static Signal<sf::Time>					GetGameTime;
	static Signal<void>						SendGameState;
	static Signal<void>						PushGarbage;
	static Signal<void, int, int>			SeedRander;
	static Signal<void>						GameClear;
	static Signal<void>						GameDraw;
	static Signal<void, int>				GameSetup;

									//Recording
	static Signal<const sf::Time&>			GetRecDuration;
	static Signal<sf::Time>					GetRecTime;
	static Signal<void, int>				RecJumpTo;
	static Signal<const sf::String&>		GetRecName;
	static Signal<void>						RecUpdateScreen;
	static Signal<void, int>				SendRecording;

	static void SendSig(int x, int y=-1, int z=-1) {
		SendSignal(x,y,z);
	}
};

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