#include "SecurityEnvironment.hpp"

SecurityEnvironment::SecurityEnvironment(AuthentificationSensor& auth_sensor, PasswordSensor& password_sensor)
  : auth_sensor(auth_sensor), password_sensor(password_sensor) {
}

SecurityEnvironment::SecurityEnvironment(AuthentificationSensor&& auth_sensor, PasswordSensor&& password_sensor)
  : auth_sensor(std::move(auth_sensor)), password_sensor(std::move(password_sensor)) {
}

void SecurityEnvironment::handleTick() {
  auth_sensor.handleTick();
  password_sensor.handleTick();

  if (auth_sensor.getLastReadTime() + password_timeout <= millis()) {
    if (current_user_id != empty_user_id) {
      current_user_id = empty_user_id;
      password_sensor.stopPasswordCollection();
      onTimeExpirationSinceAuth();
    }
    return;
  }

  if (auth_sensor.getLastReadUserID() != current_user_id) {
    current_user_id = auth_sensor.getLastReadUserID();
    std::string current_user_id_as_string = auth_sensor.getLastReadUserIDAsString();
    password_sensor.stopPasswordCollection();
    if (isKnownUser(current_user_id)) {
      password_sensor.startPasswordCollection();
      next_3_seconds_event = millis() + 3000;
      onNewKnownUser(current_user_id_as_string);
    } else {
      onNewUnknownUser(current_user_id_as_string);
    }
  }

  if (current_user_id_passed != isUserPassed2FA()) {
    if (isUserPassed2FA()) {
      current_user_id_passed = true;
      onUserPassed2FA();
    } else {
      current_user_id_passed = false;
    }
  }

  if (next_3_seconds_event < millis()) {
    onEvery3SecondsSinceAuth(auth_sensor.getLastReadTime() + password_timeout - next_3_seconds_event);
    next_3_seconds_event += 3000;
  }

  onEveryTick(auth_sensor.getLastReadUserIDAsString(), password_sensor.getPasswordAsString());
}

bool SecurityEnvironment::isUserAuthentificated() const {
  return isKnownUser(current_user_id);
}

bool SecurityEnvironment::isUserPassed2FA() const {
  return isUserAuthentificated() && password_sensor.getPassword() == correct_password;
}

bool SecurityEnvironment::isKnownUser(std::array<byte, 4> user_id) const {
  for (const std::array<byte, 4>& known_user_id : known_user_ids) {
    if (known_user_id == user_id) {
      return true;
    }
  }
  return false;
}

void SecurityEnvironment::registerOnTimeExpirationSinceAuthCallback(std::function<void()> callback) {
  onTimeExpirationSinceAuth = callback;
}

void SecurityEnvironment::registerOnNewKnownUser(std::function<void(std::string)> callback) {
  onNewKnownUser = callback;
}

void SecurityEnvironment::registerOnNewUnknownUser(std::function<void(std::string)> callback) {
  onNewUnknownUser = callback;
}

void SecurityEnvironment::registerOnUserPassed2FA(std::function<void()> callback) {
  onUserPassed2FA = callback;
}

void SecurityEnvironment::registerOnEveryTick(std::function<void(std::string, std::string)> callback) {
  onEveryTick = callback;
}

void SecurityEnvironment::registerOnEvery3SecondsSinceAuth(std::function<void(long)> callback) {
  onEvery3SecondsSinceAuth = callback;
}
