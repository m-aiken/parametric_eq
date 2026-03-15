#include "colourpalette.h"
#include "globals.h"
#include "PluginEditor.h"
#include "PluginProcessor.h"


//==============================================================================
GraphicEqEditor::GraphicEqEditor(GraphicEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    setLookAndFeel(&lnf);

    analyzerControls  = std::make_unique<AnalyzerControls>(processorRef.apvts);
    themeButton       = std::make_unique<ThemeButton>();
    analyzerContainer = std::make_unique<AnalyzerContainer>(processorRef.apvts,
                                                            processorRef.getSampleRate(),
                                                            processorRef.preEqMonoFifoL,
                                                            processorRef.preEqMonoFifoR,
                                                            processorRef.postEqMonoFifoL,
                                                            processorRef.postEqMonoFifoR);

    eqControls = std::make_unique<EqControlsContainer>(processorRef.apvts);

    addAndMakeVisible(*analyzerControls);
    addAndMakeVisible(*themeButton);
    addAndMakeVisible(*analyzerContainer);
    addAndMakeVisible(*eqControls);

    setSize(800, 600);

    activeNode = analyzerContainer->getActiveNodeIndex();

    themeButton->onClick = [this]() {
        ColourPalette::toggleDarkMode();
        repaint();
    };

    startTimerHz(20);
}

GraphicEqEditor::~GraphicEqEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

//==============================================================================
void GraphicEqEditor::paint(juce::Graphics& g)
{
    if (analyzerContainer == nullptr) {
        return;
    }

    g.fillAll(ColourPalette::getColourV2(ColourPalette::MainBackground));
    g.setFont(Globals::getFont());
    g.setColour(ColourPalette::getColourV2(ColourPalette::Text));

    auto analyzerBounds = analyzerContainer->getBounds();

    juce::Rectangle<int> leftDbScaleBounds(0, analyzerBounds.getY(), analyzerBounds.getX(), analyzerBounds.getHeight());
    juce::Rectangle<int> rightDbScaleBounds(analyzerBounds.getRight(), analyzerBounds.getY(), analyzerBounds.getX(), analyzerBounds.getHeight());

    drawDbLabels(g, leftDbScaleBounds);
    drawDbLabels(g, rightDbScaleBounds);

    auto                 textHeight = g.getCurrentFont().getHeight();
    juce::Rectangle<int> frequencyScaleBounds(analyzerBounds.getX(), analyzerBounds.getY() - textHeight, analyzerBounds.getWidth(), textHeight);
    drawFrequencyLabels(g, frequencyScaleBounds);
}

void GraphicEqEditor::resized()
{
    if (analyzerControls == nullptr || themeButton == nullptr || analyzerContainer == nullptr || eqControls == nullptr) {
        return;
    }

    auto bounds           = getLocalBounds();
    auto mainWindowWidth  = bounds.getWidth();
    auto mainWindowHeight = bounds.getHeight();
    auto padding          = Globals::getFont().getHeight();

    juce::Rectangle<int> analyzerBounds(padding * 3,
                                        padding * 6,
                                        mainWindowWidth - (padding * 6),
                                        static_cast<int>(std::floor((mainWindowHeight * 0.6) - (padding * 2))));

    auto analyzerwidth = analyzerBounds.getWidth();

    analyzerControls->setBounds(bounds.getCentreX() - (analyzerwidth * 0.25),
                                padding,
                                analyzerwidth * 0.5,
                                padding * 3);

    auto themeButtonDiameter = 24;
    themeButton->setBounds(bounds.getRight() - themeButtonDiameter - padding, padding, themeButtonDiameter, themeButtonDiameter);

    analyzerContainer->setBounds(analyzerBounds);

    eqControls->setBounds(bounds.getCentreX() - (analyzerwidth * 0.5),
                          analyzerContainer->getBottom() + padding,
                          analyzerwidth,
                          bounds.getBottom() - analyzerContainer->getBottom() - (padding * 2));
}

void GraphicEqEditor::drawDbLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds)
{
    auto textHeight = g.getCurrentFont().getHeight();

    auto maxDb = Globals::getMaxDecibels();
    auto minDb = Globals::getNegativeInf();

    auto dbScaleY      = labelBounds.getY();
    auto dbScaleHeight = labelBounds.getHeight();
    auto dbScaleWidth  = labelBounds.getWidth();
    auto dbScaleX      = labelBounds.getX();

    for (auto db = minDb; db <= maxDb; db += 12) {
        auto yCoord   = juce::jmap<int>(db, minDb, maxDb, dbScaleHeight, 0);
        auto textY    = yCoord + dbScaleY - (textHeight * 0.5);
        auto dbString = juce::String(db);

        g.drawFittedText((db > 0 ? '+' + dbString : dbString), // text
                         dbScaleX,                             // x
                         textY,                                // y
                         dbScaleWidth,                         // width
                         textHeight,                           // height
                         juce::Justification::centred,         // justification
                         1);                                   // max num lines
    }
}

void GraphicEqEditor::drawFrequencyLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds)
{
    std::vector<float> freqs { 20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f };

    const float minFreq = Globals::getMinFrequency();
    const float maxFreq = Globals::getMaxFrequency();

    auto boundsX     = labelBounds.getX();
    auto boundsY     = labelBounds.getY();
    auto boundsWidth = labelBounds.getWidth();

    auto textHeight = g.getCurrentFont().getHeight();
    auto textWidth  = textHeight * 2.5;

    for (auto i = 1; i < freqs.size() - 1; ++i) {
        auto normalizedX = juce::mapFromLog10<float>(freqs[i], minFreq, maxFreq);
        auto labelX      = boundsX + boundsWidth * normalizedX;

        juce::String text = freqs[i] >= 1000.f ? juce::String(freqs[i] / 1000.f) + "kHz" : juce::String(freqs[i]) + "Hz";

        g.drawFittedText(text,                                     // text
                         labelX - (textWidth * 0.5),               // x
                         boundsY - (textHeight * 0.5),             // y
                         textWidth,                                // width
                         textHeight,                               // height
                         juce::Justification::horizontallyCentred, // justification
                         1);                                       // max num lines
    }
}

void GraphicEqEditor::timerCallback()
{
    if (analyzerContainer == nullptr || eqControls == nullptr) {
        return;
    }

    if (activeNode != analyzerContainer->getActiveNodeIndex()) {
        activeNode = analyzerContainer->getActiveNodeIndex();
        eqControls->setBandHasNodeSelection(activeNode);
    }
}
