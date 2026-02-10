#include "../../include/utils/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace HWIDRandomizer {

    Logger& Logger::GetInstance() {
        static Logger instance;
        return instance;
    }

    Logger::~Logger() {
        Close();
    }

    void Logger::Initialize(const std::string& logFilePath) {
        std::lock_guard<std::mutex> lock(mutex);

        if (initialized) {
            logFile.close();
        }

        logFile.open(logFilePath, std::ios::out | std::ios::app);
        if (logFile.is_open()) {
            initialized = true;
            logFile << "\n========== New Session: " << GetTimestamp() << " ==========\n" << std::endl;
            logFile.flush();
        }
    }

    void Logger::Log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);

        std::string timestamp = GetTimestamp();
        std::string levelStr = GetLevelString(level);
        std::string formattedMessage = "[" + timestamp + "] [" + levelStr + "] " + message;

        // Write to file
        if (initialized && logFile.is_open()) {
            logFile << formattedMessage << std::endl;
            logFile.flush();
        }

        // Add to GUI message buffer
        guiMessages.push_back(formattedMessage);
        if (guiMessages.size() > MAX_GUI_MESSAGES) {
            guiMessages.erase(guiMessages.begin());
        }

        // Also print to console for debugging
        std::cout << formattedMessage << std::endl;
    }

    void Logger::LogInfo(const std::string& message) {
        Log(LogLevel::INFO, message);
    }

    void Logger::LogWarning(const std::string& message) {
        Log(LogLevel::WARNING, message);
    }

    void Logger::LogError(const std::string& message) {
        Log(LogLevel::ERROR, message);
    }

    void Logger::LogSuccess(const std::string& message) {
        Log(LogLevel::SUCCESS, message);
    }

    void Logger::ClearGuiMessages() {
        std::lock_guard<std::mutex> lock(mutex);
        guiMessages.clear();
    }

    void Logger::Close() {
        std::lock_guard<std::mutex> lock(mutex);
        if (logFile.is_open()) {
            logFile << "\n========== Session End: " << GetTimestamp() << " ==========\n" << std::endl;
            logFile.close();
        }
        initialized = false;
    }

    std::string Logger::GetTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        std::tm tm_buf;
        localtime_s(&tm_buf, &time_t_now);
        ss << std::put_time(&tm_buf, "%H:%M:%S");
        return ss.str();
    }

    std::string Logger::GetLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::SUCCESS: return "SUCCESS";
            default:                return "UNKNOWN";
        }
    }

} // namespace HWIDRandomizer
