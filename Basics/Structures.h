
#pragma once


namespace foleys
{

struct Size
{
    int width = 0;
    int height = 0;
};

struct Timecode
{
    /** The time code count. This is not necessarily incrementing in single steps, since
     for finer resolution the time base can be set different from the frame rate. */
    juce::int64 count = -1;

    /** The time base for audio is usually set to the sample rate and for video e.g. 1000
     to count in milli seconds. */
    double timebase = 1;

    /** Check if a timecode is set. A timecode is invalid, if no time base was set. */
    bool isValid() const { return timebase > 0 && count >= 0; }

    bool operator==(const Timecode& other)
    {
        return count == other.count && timebase == other.timebase;
    }

    bool operator!=(const Timecode& other)
    {
        return count != other.count || timebase != other.timebase;
    }
};

static inline juce::String timecodeToString (Timecode tc)
{
    auto seconds = tc.count * double (tc.timebase);
    juce::int64 intSeconds = seconds;

    auto milliSeconds = int (1000.0 * (seconds - intSeconds));
    auto days = int (intSeconds / (3600 * 24));
    intSeconds -= days * 3600 * 24;
    auto hours = int (intSeconds / 3600);
    intSeconds -= hours * 3600;
    auto minutes = int (intSeconds / 60);
    intSeconds -= minutes * 60;

    return days != 0 ? juce::String (days) + "d " : ""
    + juce::String (hours).paddedLeft ('0', 2) + ":"
    + juce::String (minutes).paddedLeft ('0', 2) + ":"
    + juce::String (intSeconds).paddedLeft ('0', 2) + "."
    + juce::String (milliSeconds).paddedLeft ('0', 3);
}

}