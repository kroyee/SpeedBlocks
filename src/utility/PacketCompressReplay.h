#ifndef PACKETCOMPRESSREPLAY_H
#define PACKETCOMPRESSREPLAY_H

#include "PacketCompress.h"

class Recording;
struct NP_Replay;
namespace sf {
class Time;
}
class GamePlay;
class ObsField;

class PacketCompressReplay : public PacketCompress {
   public:
    void compressReplay(Recording&, std::vector<uint8_t>&);
    void extractReplay(Recording&, const NP_Replay&);

    PacketCompressReplay& operator=(const GamePlay&);
    PacketCompressReplay& operator=(const ObsField&);

   private:
    void addTimeStamp(sf::Time&);
    void getTimeStamp(sf::Time&);

    // Tracker for replay time
    uint32_t lastTime;
};

#endif