#ifndef LOGINBOX_H
#define LOGINBOX_H

#include <thread>
#include "Connecting.h"
#include "GuiBase.h"
#include "PatchCheck.h"

class Resources;

class LoginBox : public GuiBase {
   private:
    bool edited = false;
    std::string name, pass;

   public:
    os::Edit username;
    os::Edit password;
    os::Edit guestname;
    std::thread t;
    PatchCheck patcher;
    Connecting connectingScreen;
    LoginBox(sf::Rect<int> _pos, Resources& _res, os::Panel& parent);
    void launchLogin(uint8_t guest);
    void login(std::string name, std::string pass, uint8_t guest);
    virtual void show();
    void sendLogin(const std::string& hashorname, uint8_t guest);
    void regPressed();
    void forgotPressed();
    void checkStatus();
};

#endif