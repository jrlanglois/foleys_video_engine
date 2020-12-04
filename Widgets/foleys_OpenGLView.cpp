/*
 ==============================================================================

 Copyright (c) 2020, Foleys Finest Audio - Daniel Walz
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

#if FOLEYS_USE_OPENGL

namespace foleys
{

OpenGLView::OpenGLView()
{

#if FOLEYS_SHOW_SPLASHSCREEN
    addAndMakeVisible (splashscreen);
#endif
}

OpenGLView::~OpenGLView()
{
    openGLContext.detach();
    shutdownOpenGL();
}

void OpenGLView::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void OpenGLView::render()
{
    juce::ScopedLock l (clipLock);
    if (clip.get() == nullptr)
        return;

    auto frame = clip->getCurrentFrame();
    
}

void OpenGLView::setClip (std::shared_ptr<AVClip> clipToUse)
{
    juce::ScopedLock l (clipLock);
    clip = clipToUse;
}

void OpenGLView::initialise()
{
    
}

void OpenGLView::shutdown()
{
}

void OpenGLView::timecodeChanged (int64_t count, double seconds)
{
    juce::ignoreUnused (count);
    juce::ignoreUnused (seconds);

}

} // namespace foleys

#endif // FOLEYS_USE_OPENGL
