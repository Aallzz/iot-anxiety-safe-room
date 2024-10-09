#pragma once

#include "AuthentificationSensor.hpp"
#include "PasswordSensor.hpp"
#include <functional>
#include <vector>
#include <array>
#include <string>

class SecurityEnvironment {
public:
    SecurityEnvironment(AuthentificationSensor& auth_sensor, PasswordSensor& password_sensor);
    SecurityEnvironment(AuthentificationSensor&& auth_sensor, PasswordSensor&& password_sensor);

    void handleTick();

    bool isUserAuthentificated() const;
    bool isUserPassed2FA() const;

    void registerOnTimeExpirationSinceAuthCallback(std::function<void()> callback);
    void registerOnNewKnownUser(std::function<void(std::string)> callback);
    void registerOnNewUnknownUser(std::function<void(std::string)> callback);
    void registerOnUserPassed2FA(std::function<void()> callback);
    void registerOnEveryTick(std::function<void(std::string, std::string)> callback);
    void registerOnEvery3SecondsSinceAuth(std::function<void(long)> callback);

private:
    bool isKnownUser(std::array<byte, 4> user_id) const;

    std::function<void(long)> onEvery3SecondsSinceAuth;
    std::function<void()> onTimeExpirationSinceAuth;
    std::function<void(std::string)> onNewKnownUser;
    std::function<void(std::string)> onNewUnknownUser;
    std::function<void()> onUserPassed2FA;
    std::function<void(std::string, std::string)> onEveryTick;

    std::vector<std::array<byte, 4>> known_user_ids = { { 0xA3, 0xE8, 0xC9, 0x0D } };
    std::vector<int> correct_password = { 0, 1, 0 };
    long password_timeout = 10000;
    std::array<byte, 4> empty_user_id = { 0x00, 0x00, 0x00, 0x00 };

    long next_3_seconds_event;
    std::array<byte, 4> current_user_id;
    bool current_user_id_passed;

    AuthentificationSensor auth_sensor;
    PasswordSensor password_sensor;
};
