#ifndef MINI_OS_SECURITY_HPP
#define MINI_OS_SECURITY_HPP

#include <string>
#include <unordered_map>

namespace MiniOS {

    enum class Role { ADMIN, NORMAL, GUEST };

    struct User {
        std::string username;
        std::string passwordHash;
        Role role;
    };

    class SecurityManager {
    private:
        std::unordered_map<std::string, User> users;
        std::string currentUser = "guest";

        std::string simpleHash(const std::string& pass) {
            uint64_t hash = 5381;
            for (char c : pass) hash = ((hash << 5) + hash) + c;
            return std::to_string(hash);
        }

    public:
        SecurityManager() {
            users["admin"] = {"admin", simpleHash("admin123"), Role::ADMIN};
            users["user"] = {"user", simpleHash("user123"), Role::NORMAL};
            users["guest"] = {"guest", simpleHash(""), Role::GUEST};
        }

        bool login(const std::string& username, const std::string& password) {
            if (users.find(username) != users.end()) {
                if (users[username].passwordHash == simpleHash(password)) {
                    currentUser = username;
                    return true;
                }
            }
            return false;
        }

        std::string getCurrentUser() const { return currentUser; }
    };
}

#endif