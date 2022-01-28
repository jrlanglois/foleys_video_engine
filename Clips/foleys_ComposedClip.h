/*
 ==============================================================================

 Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
 All rights reserved.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.

 ==============================================================================
 */

#pragma once

namespace foleys
{

/**
 @class ComposedClip

 This clip does the actual work of composing images into a video. It is assembled
 by a number of ComposedClip::ClipDescriptor instances.
 ComposedClip does audio mixing as well as video compositing. While the video
 is done on a background thread ahead of time, the audio is pulled in realtime
 to allow low latency processing.

 When you created a shared_ptr of an ComposedClip, call manageLifeTime() on the
 VideoEngine, that will add it to the auto release pool and register possible
 background jobs with the TimeSliceThreads.
 */
class ComposedClip  : public AVClip,
                      private juce::AsyncUpdater,
                      private juce::ValueTree::Listener
{
public:
    ComposedClip (VideoEngine& videoEngine);
    virtual ~ComposedClip() = default;

    /** Used to identify the clip type to the user */
    juce::String getClipType() const override { return NEEDS_TRANS ("Edit"); }

    /** Forces the video to re-render, e.g. if a parameter was changed */
    void invalidateVideo();

    juce::String getDescription() const override;

    std::pair<int64_t, juce::Image> getFrame (double pts) const override;
    bool isFrameAvailable (double pts) const override;

    juce::Image getCurrentFrame() const override;

    Size getVideoSize() const override;
    double getCurrentTimeInSeconds() const override;

    juce::Image getStillImage (double seconds, Size size) override;

    double getLengthInSeconds() const override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    void getNextAudioBlock (const juce::AudioSourceChannelInfo&) override;
    void setNextReadPosition (juce::int64 samples) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping (bool shouldLoop) override;

    bool hasVideo() const override;
    bool hasAudio() const override;

    std::shared_ptr<AVClip> createCopy (StreamTypes types) override;

    double getSampleRate() const override;

    /** When rendering non realtime (bounce), use this to wait for background
        threads to read ahead */
    bool waitForSamplesReady (int samples, int timeout=1000) override;

    int getDefaultBufferSize() const;

    juce::TimeSliceClient* getBackgroundJob() override;

    /** Read all plugins getStateInformation() and save it into the statusTree as BLOB */
    void readPluginStatesIntoValueTree();

    /** The ValueTree describes the media and positions of the individual clips.
        You can use this to listen to changes or to serialise a clip / project */
    juce::ValueTree& getStatusTree();

    std::shared_ptr<ClipDescriptor> addClip (std::shared_ptr<AVClip> clip, double start, double length = -1, double offset = 0);
    void removeClip (std::shared_ptr<ClipDescriptor> descriptor);

    std::vector<std::shared_ptr<ClipDescriptor>> getClips() const;
    std::shared_ptr<ClipDescriptor> getClip (int index);

    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                   const juce::Identifier& property) override;

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override;

    void valueTreeChildOrderChanged (juce::ValueTree& /*parentTreeWhoseChildrenHaveMoved*/,
                                     int /*oldIndex*/, int /*newIndex*/) override {}

    void valueTreeParentChanged (juce::ValueTree& /*treeWhoseParentHasChanged*/) override {}

    juce::UndoManager* getUndoManager();

    /** This lock is used, when the vector of clips is changed, or when a clip is altered
        in a way, that it cannot render correctly, e.g. when adding or removing an audio plugin */
    juce::CriticalSection& getCallbackLock() { return clipDescriptorLock; }

    /** Create a unique description by appending or incrementing a number */
    juce::String makeUniqueDescription (const juce::String& description) const;

private:

    void handleAsyncUpdate() override;

    double convertToSamples (int64_t pos) const;

    std::vector<std::shared_ptr<ClipDescriptor>> getActiveClips (std::function<bool(ClipDescriptor&)> selector) const;

    /** @internal */
    class ComposingThread : public juce::TimeSliceClient
    {
    public:
        ComposingThread (ComposedClip& owner);
        int useTimeSlice() override;

        void setSuspended (bool s);
        bool isSuspended() const;

    private:
        ComposedClip& owner;
        bool suspended = true;
        bool inRenderBlock = false;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComposingThread)
    };

    juce::CriticalSection clipDescriptorLock;

    VideoFifo videoFifo;
    ComposingThread videoRenderJob;

    juce::ValueTree state;
    bool manualStateChange = false;

    AudioStreamSettings audioSettings;

    std::unique_ptr<VideoMixer> videoMixer;
    std::unique_ptr<AudioMixer> audioMixer;

    std::vector<std::shared_ptr<ClipDescriptor>> clips;
    std::atomic<int64_t> position = {};

    int64_t lastShownFrame;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComposedClip)
};

} // foleys
