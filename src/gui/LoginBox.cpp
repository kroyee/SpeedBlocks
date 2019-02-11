#include "LoginBox.h"
#include "GameSignals.h"
#include "Options.h"
#include "Packets.hpp"
#include "Resources.h"
#include "machineid.h"
#include "network.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& SetName = Signal<void, const std::string&>::get("SetName");
static auto& Disconnect = Signal<void, int>::get("Disconnect");
static auto& SetRoomListTime = Signal<void>::get("SetRoomListTime");

LoginBox::LoginBox(sf::Rect<int> _pos, Resources& _res, os::Panel& parent) : GuiBase(_pos, _res, parent), connectingScreen(sf::Rect<int>(0, 0, 960, 600), resources) {
    os::Label().text_size(42).text("Play online").pos(20, 0).add_to(panel);

    os::Label().text("Password").pos(23, 123).add_to(panel);

    username.pos(120, 80)
        .size(180, 30)
        .text(Options::get<std::string>("username"))
        .add_to(panel)
        .title("Username")
        .connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);

    if (Options::get<bool>("rememberme")) {
        std::string boguspass;
        for (int i = 0; i < Options::get<short>("pass"); i++) boguspass += "*";
        password.text(boguspass);
    }
    password.pos(120, 120)
        .size(180, 30)
        .pass()
        .connect("TextChanged", [&]() { edited = true; })
        .connect("Focused",
                 [&]() {
                     if (!edited) password.text("");
                 })
        .connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0)
        .connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0)
        .add_to(panel);

    os::CheckBox()
        .text("Remember me")
        .pos(120, 160)
        .check(Options::get<bool>("rememberme"))
        .connect("Checked", [&]() { Options::get<bool>("rememberme") = true; })
        .connect("Unchecked", [&]() { Options::get<bool>("rememberme") = false; })
        .add_to(panel);

    os::Button().pos(160, 200).size(100, 40).text("Login").connect("pressed", &LoginBox::launchLogin, this, 0).add_to(panel);

    os::Button().pos(20, 220).size(100, 30).text("Register").connect("pressed", &LoginBox::regPressed, this).add_to(panel);

    os::Button().pos(20, 260).size(160, 30).text("Forgot password").connect("pressed", &LoginBox::forgotPressed, this).add_to(panel);

    os::Label().text("Play as guest").pos(145, 330).add_to(panel);

    os::Label().text("Name").pos(53, 363).add_to(panel);

    guestname.pos(120, 360).size(180, 30).connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 1).add_to(panel);

    os::Button().pos(135, 410).size(150, 40).text("Login as Guest").connect("pressed", &LoginBox::launchLogin, this, 1).add_to(panel);

    DEBUG_ONLY([&]() {
        os::Button().text("localhost").pos(135, 460).add_to(panel).connect("pressed", [&]() {
            resources.net->serverAdd = "localhost";
            launchLogin(1);
        });
    });

    connectSignal("IsLoginThreadJoinable", [&]() { return t.joinable(); });
    connectSignal("TellPatcherToQuit", [&]() { patcher.quit = true; });
}

void LoginBox::launchLogin(uint8_t guest) {
    if (guest && !guestname->getText().getSize()) return;
    Hide(0);
    connectingScreen.show();
    connectingScreen.label.text("Connecting to server...");
    if (t.joinable()) t.join();
    if (guest)
        t = std::thread(&LoginBox::login, this, guestname->getText(), password->getText(), guest);
    else
        t = std::thread(&LoginBox::login, this, username->getText(), password->getText(), guest);
}

void LoginBox::login(std::string name, std::string pass, uint8_t guest) {
    patcher.status = 1;
    if (resources.net->connect() == sf::Socket::Done) {
        std::string hash;
        if (!guest) {
            patcher.status = 2;
            if (!edited && Options::get<bool>("rememberme"))
                hash = resources.net->sendCurlPost("https://speedblocks.se/secure_auth.php",
                                                   "name=" + name + "&remember=" + Options::get<std::string>("hash") + "&machineid=" + machineid::machineHash(), 1);
            else
                hash =
                    resources.net->sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&pass=" + pass + "&machineid=" + machineid::machineHash(), 1);
            if (Options::get<bool>("rememberme")) {
                if (hash.size() >= 40) Options::get<std::string>("hash") = hash.substr(20);
            } else
                Options::get<std::string>("hash") = "null";
            patcher.status = 3;
            hash = hash.substr(0, 20);
            Options::get<std::string>("username") = name;
            Options::get<short>("pass") = pass.size();
            sendLogin(hash, guest);
        } else {
            patcher.status = 3;
            sendLogin(name, guest);
            SetName(name);
            resources.name = name;
        }
        resources.playonline = true;
        SetRoomListTime();
        Show(11);
    } else {
        Disconnect(2);
        patcher.status = -1;
    }
}

void LoginBox::checkStatus() {
    if (patcher.status == 2) {
        connectingScreen.label.text("Doing secure auth with auth-server...").pos(130, 290);
        connectingScreen.changelog.hide();
        connectingScreen.cancel.hide();
        connectingScreen.apply.hide();
        connectingScreen.apply.disable();
        connectingScreen.downloadProgress.hide().set(0);
    } else if (patcher.status == 3)
        connectingScreen.label.text("Waiting for auth-response from game-server...");
    else if (patcher.status == -1) {
        t.join();
        QuickMsg("Could not connect to server");
        connectingScreen.hide();
        Show(0);
    } else if (patcher.status == 4) {
        t.join();
        QuickMsg("No new version found");
        connectingScreen.hide();
        Show(0);
    } else if (patcher.status == 5) {
        connectingScreen.label.text("New patch found, " + std::to_string(patcher.files_downloaded) + " of " + std::to_string(patcher.files_total) + " files downloaded");
        connectingScreen.label.pos(130, 40);
        connectingScreen.setDownloadProgress(patcher.filesize, patcher.downloaded);
        connectingScreen.changelog.text(patcher.changelog);
        connectingScreen.changelog.show();
        connectingScreen.cancel.show();
        connectingScreen.downloadProgress.show();
    } else if (patcher.status == 6) {
        t.join();
        connectingScreen.label.text("Patch is ready!");
        connectingScreen.apply.show();
        connectingScreen.apply.enable();
    } else if (patcher.status == -2) {
        t.join();
        QuickMsg("Error talking to patch server");
        connectingScreen.hide();
        Show(0);
    } else if (patcher.status == -3) {
        t.join();
        QuickMsg("Error downloading file");
        connectingScreen.hide();
        Show(0);
    } else if (patcher.status == -4) {
        t.join();
        QuickMsg("The md5-sum for downloaded file did not match, aborting");
        connectingScreen.hide();
        Show(0);
    } else if (patcher.status == -5) {
        t.join();
        QuickMsg("Error saving file");
        connectingScreen.hide();
        Show(0);
    } else if (patcher.status == -6) {
        t.join();
        connectingScreen.hide();
        Show(0);
    }
    patcher.status = 0;
}

void LoginBox::show() {
    panel.show();
    username.focus();
}

void LoginBox::sendLogin(const std::string& hashorname, uint8_t guest) { TCP.write_as<NP_LoginRequest>(resources.clientVersion, guest, hashorname); }

void LoginBox::regPressed() {
#ifdef _WIN32
    system("start https://speedblocks.se/forum/ucp.php?mode=register");
#elif __APPLE__
    system("open https://speedblocks.se/forum/ucp.php?mode=register");
#else
    system("xdg-open https://speedblocks.se/forum/ucp.php?mode=register");
#endif
}

void LoginBox::forgotPressed() {
#ifdef _WIN32
    system("start https://speedblocks.se/forum/ucp.php?mode=sendpassword");
#elif __APPLE__
    system("open https://speedblocks.se/forum/ucp.php?mode=sendpassword");
#else
    system("xdg-open https://speedblocks.se/forum/ucp.php?mode=sendpassword");
#endif
}
