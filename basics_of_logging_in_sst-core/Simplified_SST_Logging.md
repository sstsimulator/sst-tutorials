# SIMPLIFIED SST LOGGING

## INTRODUCTION

Logging is quite possibly one of the most important features built into an application.  It is useful not only for debugging but for analyzing what happened when problems occur after deployment.

To support logging, the Structural Simulation Toolkit (SST) includes a logging class that provides a standardized way to format and output logging messages.  Like most, it allows the developer to specify a formatted text string that will be printed before each log message, the logging level, and has the option to send log messages to the terminal or log file.

However, for those developers first coming to SST the interface can be slightly confusing.  This documentation is intended to serve as a brief introduction to the SST logging class, explain how its features map to those from other logging libraries, and provide ways to simplify the interface to something a developer may be more familiar with.

## A BRIEF INTRODUCTION TO LOGGING

As stated by Colin Eberhardt, "Logging is the process of recording application actions and state to a secondary interface." (Eberhardt, 2014)  Recording application actions usually takes the form of a printed message and the secondary interface can be a local file, network storage, or even the terminal.

According to the Syslog Protocol (Gerhards, 2009), logging messages can be categorized as:

Severity       | Description
---------------|----------------
Emergency      | System is unusable
Alert          | Action must be taken immediately
Critical       | Critical conditions
Error          | Error conditions
Warning        | Warning conditions
Notice         | Normal but significant condition
Informational  | Informational messages
Debug          | Debug-level messages

Available severity levels will differ according to the library being used.  Typical ones encountered are (Rafal, 2020):

Severity      | Description
--------------|-----------------
Fatal         | Something happened that prevents the application from operating.
Error         | Something happened that prevents one or more of the program functions from operating.
Warn          | Something unexpected happened but the program can continue.
Info          | Information useful during normal program operation.  Can be ignored.
Debug         | Information used to diagnose and trouble program operation.  Usually of use only to the programmer.
Trace         | The most verbose.  Can be used to trace the program execution path and method calls.

In some cases the programmer can also define custom severity levels that fit within this hierarchy.

Information included in a logging statement usually includes the file from which the logging statement is being emitted, the method name and line number, the time at which the logging statement is being emitted, severity, and a message describing the problem.  So a typical log message will look like:

```
2021-02-26 10:15:12; File=main.cc; Func=main; Line=42; WARNING; Cannot open configuration file
```

## LOGGING IN SST

Logging within SST is provided by the Output class.  The Output class allows the programmer to customize log messages and has different methods used to print.

### Customizing Log Messages

Log messages can be customized using the following properties.

**Prefix Data.**  Prefix data is a formatted string printed at the beginning of the log message, before the programmer's message.  The following information can be included in the prefix data.

* @f - Name of the file from which the log message is being printed.
* @l - File line number from which the log message is being printed.
* @p - Name of the function from which the log message is being printed.
* @r - MPI rank of the calling process.  Will be empty if there is only a single rank.
* @R - MPI rank of the calling process.  Will be 0 if there is only a single rank.
* @i - Calling process thread id.  Will be empty if there is only a single thread.
* @I - Calling process thread id.  Will be 0 if there is only a single thread.
* @x - Calling process rank information.  Will be empty if the number of MPI ranks and the number of threads are both 1.  Same as @r:@i.
* @X - Calling process rank information.  Will be 0.0 if the number of MPI ranks and the number of threads are both 1.  Same as @R:@I.
* @t - Simulation time.  This is the raw simulation cycle time retrieved from the SST Core.

**Verbose Level.**  The Output class verbose level corresponds to the Syslog Protocol severity level.  When an instance of the output class is instantiated an associated verbose level is assigned and only messages with a higher or equal verbose level (lower or equal numeric value) will be logged.  So, for example, if an instance of the Output class with a verbose level of 10 is instantiated, only messages with verbose levels of 0 - 10 will be logged.  

**Verbose Mask.**  The Output class verbose mask serves as an additional filter on log messages.  Only those log messages whose verbose mask matches the one specified when the Output class was instantiated will be printed.

**Target Output Location.**  The location where log messages are to be sent is specified when the Output class is instantiated.  Available choices are SST::Output::STDERR, SST::Output::STDOUT, SST::Output::NONE, or SST::Output::FILE.  If SST::Output::FILE is specified, the output file name can also be specified.

### Writing Log Messages

The SST Output class provides four different methods to write log messages.

**Output.**  This method works like a standard printf.  By default its messages do not contain the prefix data but can if desired.  

**Verbose.**  Messages logged using this method contain the prefix method by default.  Whether or not the messages are logged is controlled by the verbose level and verbose mask.

**Debug.**  This method operates similarly to the Verbose method but will only be compiled if the --enable-debug-flag is set in the SST configuration file.  

**Fatal.**  Messages logged using this method are always sent to SST::Output::STDERR and terminate the simulation.  

Of the four methods, Verbose is the one that will be used most often.

### SST Logging Example

Based upon the above information, a typical logger instantiation will look like:

```
Output logger("@t; File=@f; Func=@p; Line=@l", 300, 0x00, SST::Output::STDOUT);
```
This logger will include the simulation time, file name, function name, and line number in the prefix.  Any log message with a verbose level of less than 300 and a verbose mask of 0x00 will print to STDOUT.

Calls to write to the log file will look like:

```
logger.verbose(CALL_INFO, 200, 0x00, "Log message %d.\n", 1);
logger.verbose(CALL_INFO, 400, 0x00, "Log message %d.\n", 2);
```
In these cases, the first log message with a verbose level less than 300 will be printed.  However, the second will not be printed since its verbose level is greater than 300.

## SIMPLIFYING THE LOGGER PROCESS

The SST Output class provides all the capabilities necessary for logging.  However, there are some ways the interface can be made simpler and more intuitive.

### Define Log Levels ###

The SST Output class defines its verbose level as a 32-bit unsigned integer.  This allows for over 4 billion logging levels.  However, loggers typically only define a few logging levels as listed above, possibly with room for a few user-defined custom logging levels.  In addition, loggers typically define their logging levels by name for easy reference.

To accomplish this in SST, map the numeric log levels to names using `#define` or enumerations.

```
#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 100
#define LOG_LEVEL_WARN  200
#define LOG_LEVEL_INFO  300
#define LOG_LEVEL_DEBUG 400
#define LOG_LEVEL_TRACE 500

enum LogLevel {
    Fatal = 0,
    Error = 100,
    Warn  = 200,
    Info  = 300,
    Debug = 400,
    Trace = 500
}
```

Once the log levels are defined instantiation and calling the logger will look like:

```
Output logger("@t; File=@f; Func=@p; Line=@l", INFO, 0x00, SST::Output::STDOUT);

logger.verbose(CALL_INFO,  WARN, 0x00, "Log message #%d.\n", 1);
logger.verbose(CALL_INFO, DEBUG, 0x00, "Log message #%d.\n", 2);
```
or

```
Output logger("@t; File=@f; Func=@p; Line=@l", LogLevel.Info, 0x00, SST::Output::STDOUT);

logger.verbose(CALL_INFO,  LogLevel::Warn, 0x00, "Log message #%d.\n", 1);
logger.verbose(CALL_INFO, LogLevel::Debug, 0x00, "Log message #%d.\n", 2);
```
### Create Logging Methods ###

The command to write to the logger is rather verbose.  It can be simplified by defining a logging method for each of the logging levels.  Another advantage is you can add the logging level to the format string, which makes it easier to search for messages associated with specific logging levels in the logging output.

```
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
```
Writing messages to the logger then looks like:
```
Output logger("@t; File=@f; Func=@p; Line=@l", INFO, 0x00, SST::Output::STDOUT);

logWarn(logger, CALL_INFO, "Log message #%d.\n", 1);
logDebug(logger, CALL_INFO, "Log message #%d.\n", 2);
```

### Put It All In A Class ###

Taking it one step further, this could all be combined into a C++ class that wraps the SST Output class.

```
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

```
Writing messages to the logger then looks like
```
Logger logger("@t; File=@f; Func=@p; Line=@l", Logger::LogLevel::INFO, Logger.LogLocation::STDOUT);

logger.Warn(CALL_INFO, "Log message #%d.\n", 1);
logger.Debug(CALL_INFO, "Log message #%d.\n", 2);
```
which is much simpler than the original.


References

1.  Eberhardt, C. (2014, March 25). The Art of Logging. Retrieved March 5, 2021, from https://www.codeproject.com/Articles/42354/The-Art-of-Logging.

2.  Gerhards, R. (2009, March). RFC 5424 - The Syslog Protocol. Retrieved March 5, 2021, from https://tools.ietf.org/html/rfc5424

3.  Rafal, K. (2020, October 8). Logging Levels: What They Are & How to Choose Them - Sematext. Sematext. Retrieved March 5, 2021, from https://sematext.com/blog/logging-levels/
