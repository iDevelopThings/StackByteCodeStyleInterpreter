#pragma once
#include <chrono>
#include <iostream>

#define PREPROCESSOR_JOIN(x, y) PREPROCESSOR_JOIN_INNER(x, y)
#define PREPROCESSOR_JOIN_INNER(x, y) x##y
#define ANONYMOUS_VARIABLE(Name) PREPROCESSOR_JOIN(Name, __COUNTER__)


struct TimeStat
{
    friend class FScopedTimeStat;

    std::string Name;
    std::string FunctionName;

    std::chrono::high_resolution_clock::time_point StartedAt;
    std::chrono::high_resolution_clock::time_point EndedAt;

    TimeStat() = default;

    TimeStat(const std::string& InName) {
        Name = InName;
        StartedAt = std::chrono::high_resolution_clock::now();
    }

    TimeStat(const std::string& InName, const std::string& FnName) {
        Name = InName;
        FunctionName = FnName;
        StartedAt = std::chrono::high_resolution_clock::now();
    }

    static std::string FormatStatTimeDuration(std::chrono::nanoseconds Duration) {
        const auto Nanoseconds = Duration.count();
        const auto Microseconds = std::chrono::duration_cast<std::chrono::microseconds>(Duration).count();
        const auto Milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(Duration).count();
        const auto Seconds = std::chrono::duration_cast<std::chrono::seconds>(Duration).count();

        if (Nanoseconds < 1000.0) {
            return std::format("{} ns", Nanoseconds);
        }

        if (Microseconds < 1000.0) {
            return std::format("{} us", Microseconds);
        }

        if (Milliseconds < 1000.0) {
            return std::format("{:.1f} ms", static_cast<float>(Milliseconds));
        }

        return std::format("{:.2f} s", static_cast<float>(Seconds));
    }

private:
    void Ended() {
        if (Name.empty() && FunctionName.empty()) {
            return;
        }

        EndedAt = std::chrono::high_resolution_clock::now();
        const auto Duration = std::chrono::duration_cast<std::chrono::nanoseconds>(EndedAt - StartedAt);

        const std::string Time = FormatStatTimeDuration(Duration);

        std::string ContextString = "[TIMED STAT]";
        if (!FunctionName.empty()) {
            ContextString += std::string("[" + FunctionName + "]");
        }
        if (!Name.empty()) {
            ContextString += std::string("[" + Name + "]");
        }

        std::cout << ContextString << " -> " << Time << std::endl;
    }
};

class FScopedTimeStat
{
    TimeStat TimeStatValue;

public:
    FScopedTimeStat(const std::string& InName) : TimeStatValue(TimeStat(InName)) {}

    FScopedTimeStat(const std::string& InName, const std::string& InFnName) : TimeStatValue(TimeStat(InName, InFnName)) {}

    ~FScopedTimeStat() {
        TimeStatValue.Ended();
    }
};

#define TIMED_FUNCTION() FScopedTimeStat ANONYMOUS_VARIABLE(TimeStat_)("", __FUNCTION__)
#define TIMED_SCOPE(Name) FScopedTimeStat ANONYMOUS_VARIABLE(TimeStat_)(Name, __FUNCTION__)
#define SCOPED_TIME(Name) FScopedTimeStat ANONYMOUS_VARIABLE(TimeStat_)(Name, __FUNCTION__)
