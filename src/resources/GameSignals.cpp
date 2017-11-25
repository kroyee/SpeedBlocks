#include "GameSignals.h"

Signal<void, int> Signals::Show;
Signal<void, int> Signals::Hide;
Signal<void, int> Signals::Enable;
Signal<void, int> Signals::Disable;
Signal<bool, int> Signals::IsVisible;
Signal<void, const sf::Time&> Signals::EnableBackground;
Signal<void> Signals::DisableBackground;
Signal<void> Signals::ShowAlert;
Signal<void> Signals::HideAlert;
Signal<void> Signals::LeaveRoom;
Signal<void, int> Signals::Disconnect;
Signal<void, GameStates> Signals::SetGameState;
Signal<bool> Signals::IsLoginThreadJoinable;
Signal<void> Signals::TellPatcherToQuit;
Signal<bool> Signals::ApplyPatch;
Signal<void, const sf::String&> Signals::QuickMsg;
Signal<void> Signals::LightTheme;
Signal<void> Signals::DarkTheme;
Signal<void, int> Signals::SetFieldsBackColor;
Signal<void, int, int> Signals::SetFieldsSize;
Signal<void> Signals::MakeBackgroundLines;
Signal<int, int> Signals::AreThereFields;
Signal<void, const sf::String&> Signals::SetConnectingText;
Signal<void> Signals::SetRoomListTime;
Signal<void, const sf::String&> Signals::SetAreYouSure;
Signal<void, const sf::String&> Signals::AddAlert;
Signal<void> Signals::HideStartChallengeButton;
Signal<void, int> Signals::JoinRoom;
Signal<void, int> Signals::ShowOptions;
Signal<void, GameplayData&> Signals::UpdateGamedata;
Signal<void, GameplayData&> Signals::UpdateChallengesUI;
Signal<void, sf::Time> Signals::UpdateReplayUI;
Signal<bool> Signals::Survivor;
Signal<bool> Signals::Cheese30L;
Signal<void, GameplayData&, uint16_t, const sf::String&, uint16_t> Signals::AddLocalScore;
Signal<void, int> Signals::SetRoundlenghtForScore;
Signal<void> Signals::FieldFinishedDrawing;

//GameFieldDrawer
Signal<void> Signals::ShowGameFields;
Signal<void> Signals::HideGameFields;
Signal<bool> Signals::GameFieldsIsVisible;
Signal<obsField&, int, const sf::String&> Signals::AddField;
Signal<void, int> Signals::RemoveField;
Signal<void> Signals::RemoveAllFields;

//Network
Signal<void, int, int, int> Signals::SendSignal;
Signal<void, sf::Packet&> Signals::SendPacket;
Signal<void, sf::Packet&> Signals::SendPacketUDP;
Signal<void, int, int> Signals::SendPing;

//Sound
Signal<void, int> Signals::PlaySound;
Signal<void, int> Signals::SetEffectVolume;
Signal<void, int> Signals::SetMusicVolume;
Signal<void, int> Signals::SetAlertsVolume;
Signal<void> Signals::EnableSound;
Signal<void> Signals::DisableSound;

//Game
Signal<void> Signals::StartCountDown;
Signal<void, int> Signals::GameOver;
Signal<void> Signals::Ready;
Signal<void> Signals::Away;
Signal<void, bool> Signals::SetAway;
Signal<void> Signals::SetDrawMe;
Signal<void, int> Signals::SetGameBackColor;
Signal<void, const sf::String&> Signals::SetName;
Signal<const sf::String&> Signals::GetName;
Signal<void> Signals::UpdateGamePieces;
Signal<GameplayData&> Signals::GetGameData;
Signal<sf::Time> Signals::GetGameTime;
Signal<void> Signals::SendGameState;
Signal<void> Signals::PushGarbage;
Signal<void, int, int> Signals::SeedRander;
Signal<void> Signals::GameClear;
Signal<void> Signals::GameDraw;
Signal<void, int> Signals::GameSetup;
Signal<void, int> Signals::GameAddDelay;
Signal<void, int> Signals::AddGarbage;
Signal<void> Signals::MakeDrawCopy;
Signal<void> Signals::GameDrawSprite;

//AI
Signal<void, int, int> Signals::DistributeLinesLocally;
Signal<void, uint8_t> Signals::AmountAI;
Signal<void, uint16_t> Signals::SpeedAI;

//Recording
Signal<const sf::Time&> Signals::GetRecDuration;
Signal<sf::Time> Signals::GetRecTime;
Signal<void, int> Signals::RecJumpTo;
Signal<const sf::String&> Signals::GetRecName;
Signal<void> Signals::RecUpdateScreen;
Signal<void, int> Signals::SendRecording;

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