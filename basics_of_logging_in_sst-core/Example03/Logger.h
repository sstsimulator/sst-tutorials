#ifndef LOGGER_H
#define LOGGER_H

#include <sst/core/output.h>
#include <stdio.h>

namespace LoggerExample
{
    class Logger
    {
    public:
        enum LogLevel {
            Fatal = 0,
            Error = 100,
            Warn  = 200,
            Info  = 300,
            Debug = 400,
            Trace = 500
        };

        enum LogLocation {
            NONE,       // No output
            STDOUT,     // Print to stdout
            STDERR,     // Print to STDERR
            FILE        // Print to a file
        };

        SST::Output output_;

        // Constructors.
        //
        Logger()
        {
            output_ = SST::Output("", LogLevel::Trace, 0x00, static_cast<SST::Output::output_location_t>(LogLocation::NONE));
        }

        Logger(const std::string prefix, LogLevel logLevel, LogLocation location, const std::string& logFileName = "")
        {
            output_ = SST::Output(prefix, logLevel, 0x00, static_cast<SST::Output::output_location_t>(location), logFileName);
        }

        // Methods used to log the various log levels.
        // The first three parameters (line, file, and func) are provided by the SST CALL_INFO macro.
        //
        template<typename... Args> 
        void logFatal(
            uint32_t line, const char* file, const char* func,
            const char* format, Args... arg)
        {
            std::string fmt = "[FATAL] - " + std::string(format);
            output_.fatal(line, file, func, 0x00, fmt.c_str(), arg...);
        }

        template<typename... Args> 
        void logError(
            uint32_t line, const char* file, const char* func,
            const char* format, Args... arg)
        {
            std::string fmt = "[ERROR] - " + std::string(format);
            output_.verbose(line, file, func, LogLevel::Error, 0x00, fmt.c_str(), arg...);
        }

        template<typename... Args>
        void logWarn(
            uint32_t line, const char* file, const char* func,
            const char* format, Args... arg)
        {
            std::string fmt = "[WARN] - " + std::string(format);
            output_.verbose(line, file, func, LogLevel::Warn, 0x00, fmt.c_str(), arg...);
        }

        template<typename... Args>    
        void logInfo(
            uint32_t line, const char* file, const char* func,
            const char* format, Args... arg)
        {
            std::string fmt = "[INFO] - " + std::string(format);
            output_.verbose(line, file, func, LogLevel::Info, 0x00, fmt.c_str(), arg...);
        }

        template<typename... Args> 
        void logDebug(
            uint32_t line, const char* file, const char* func,
            const char* format, Args... arg)
        {
            std::string fmt = "[DEBUG] - " + std::string(format);
            output_.verbose(line, file, func, LogLevel::Debug, 0x00, fmt.c_str(), arg...);
        }

        template<typename... Args>
        void logTrace(
            uint32_t line, const char* file, const char* func, 
            const char* format, Args... arg)
        {
            std::string fmt = "[TRACE] - " + std::string(format);
            output_.verbose(line, file, func, LogLevel::Trace, 0x00, fmt.c_str(), arg...);
        }
    };
}

#endif