#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <mutex>

namespace HWIDRandomizer {

    enum class LogLevel {
        INFO,
        WARNING,
        ERROR_LOG,  // Changed from ERROR to avoid Windows.h conflict
        SUCCESS
    };

    class Logger {
    public:
        static Logger& GetInstance();

        void Initialize(const std::string& logFilePath);
        void Log(LogLevel level, const std::string& message);
        void LogInfo(const std::string& message);
        void LogWarning(const std::string& message);
        void LogError(const std::string& message);
        void LogSuccess(const std::string& message);

        const std::vector<std::string>& GetGuiMessages() const { return guiMessages; }
        void ClearGuiMessages();

        void Close();

    private:
        Logger() = default;
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::string GetTimestamp() const;
        std::string GetLevelString(LogLevel level) const;

        std::ofstream logFile;
        std::vector<std::string> guiMessages; // Last 100 messages for GUI display
        std::mutex mutex;
        bool initialized = false;

        static constexpr size_t MAX_GUI_MESSAGES = 100;
    };

} // namespace HWIDRandomizer
