#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>


#define LOG_TRACE(msg) Logger::getInstance().log(Logger::TRACE, msg, __FILE__, __LINE__)
#define LOG_DEBUG(msg) Logger::getInstance().log(Logger::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Logger::getInstance().log(Logger::INFO, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::getInstance().log(Logger::WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::getInstance().log(Logger::ERROR, msg, __FILE__, __LINE__)

class Logger {
public:
    enum Severity {
        TRACE, DEBUG, INFO, WARNING, ERROR
    };

    // Получение экземпляра Logger
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Установка уровня логирования
    void setLevel(Severity severity) {
        logLevel = severity;
    }

    // Установка имени файла лога
    void setLogFileName(const std::string& name) {
        logFileName = name;
    }

    // Установка места вывода
    void setOutput(bool toConsole, bool toFile) {
        outputToConsole = toConsole;
        outputFile = toFile ? std::make_unique<std::ofstream>(logFileName) : nullptr;
    }

    // Логирование сообщения
    void log(Severity severity, const std::string& message, const char* file, int line) {
        if (severity < logLevel) return;

        std::lock_guard<std::mutex> guard(mutex);
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::tm now_tm;
        localtime_s(&now_tm, &now_c);

        std::ostringstream logStream;
        logStream << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
        logStream << " | " << getSeverityString(severity) << " | " << file << ":" << line << " -> " << message << std::endl;

        if (outputToConsole) {
            std::cout << logStream.str();
        }
        if (outputFile) {
            *outputFile << logStream.str();
        }
    }

private:
    Logger() : logLevel(INFO), logFileName("project.log"), outputToConsole(true) {}

    Severity logLevel;
    std::string logFileName;
    bool outputToConsole;
    std::unique_ptr<std::ofstream> outputFile;
    std::mutex mutex;

    // Получение строки уровня логирования
    std::string getSeverityString(Severity severity) {
        switch (severity) {
        case TRACE: return "TRACE";
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
        }
    }

    // Запрет копирования и присваивания
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

// Пример использования
int main() {
    // Установка уровня логирования
    Logger::getInstance().setLevel(Logger::DEBUG);

    // Установка имени файла лога
    Logger::getInstance().setLogFileName("my_project.log");

    // Установка мест вывода
    Logger::getInstance().setOutput(true, true);

    // Логирование
    LOG_DEBUG("Debug Message");
    LOG_INFO("Information message");
    LOG_WARNING("Warning message");
    LOG_ERROR("Error message");
    return 0;
}
