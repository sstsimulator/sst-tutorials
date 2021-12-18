#ifndef LOGGER_H
#define LOGGER_H

#include <sst/core/output.h>
#include <stdio.h>

namespace Logger
{
    // Define the log levels.  Each level displays its own message and the
    // levels above it.
    //
    enum LogLevel {
        Fatal = 0,
        Error = 100,
        Warn  = 200,
        Info  = 300,
        Debug = 400,
        Trace = 500
    };

    template<typename... Args> 
    void logFatal(SST::Output logger, 
        uint32_t line, const char* file, const char* func,
        const char* format, Args... arg)
    {
        std::string fmt = "[FATAL] - " + std::string(format);
        logger.fatal(line, file, func, 0x00, fmt.c_str(), arg...);
    }

    template<typename... Args> 
    void logError(SST::Output logger, 
        uint32_t line, const char* file, const char* func,
        const char* format, Args... arg)
    {
        std::string fmt = "[ERROR] - " + std::string(format);
        logger.verbose(line, file, func, LogLevel::Error, 0x00, fmt.c_str(), arg...);
    }

    template<typename... Args>
    void logWarn(SST::Output logger, 
        uint32_t line, const char* file, const char* func,
        const char* format, Args... arg)
    {
        std::string fmt = "[WARN] - " + std::string(format);
        logger.verbose(line, file, func, LogLevel::Warn, 0x00, fmt.c_str(), arg...);
    }

    template<typename... Args>    
    void logInfo(SST::Output logger, 
        uint32_t line, const char* file, const char* func,
        const char* format, Args... arg)
    {
        std::string fmt = "[INFO] - " + std::string(format);        
        logger.verbose(line, file, func, LogLevel::Info, 0x00, fmt.c_str(), arg...);
    }

    template<typename... Args> 
    void logDebug(SST::Output logger, 
        uint32_t line, const char* file, const char* func,
        const char* format, Args... arg)
    {
        std::string fmt = "[DEBUG] - " + std::string(format);
        logger.verbose(line, file, func, LogLevel::Debug, 0x00, fmt.c_str(), arg...);
    }

    template<typename... Args>
    void logTrace(SST::Output logger, 
        uint32_t line, const char* file, const char* func, 
        const char* format, Args... arg)
    {
        std::string fmt = "[TRACE] - " + std::string(format);
        logger.verbose(line, file, func, LogLevel::Trace, 0x00, fmt.c_str(), arg...);
    }
}

#endif