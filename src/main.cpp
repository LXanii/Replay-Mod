#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

bool Recording = true;
float start;
float end;
bool holding;
std::vector<std::vector<float>> recordingMacro = {};
std::vector<std::vector<float>> playMacro;

class $modify(PlayLayer) {

    struct Fields {
        CCLabelBMFont* playingText;
        CCLabelBMFont* clickcountText;
        CCSprite* clickFrame1;
        CCSprite* clickFrame2;
        int clickCount;
    };

    void gameInput(bool hold, int clickType, bool plr1) {PlayLayer::handleButton(hold, clickType, plr1);}
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        PlayLayer::destroyPlayer(p0, p1);
        if (Recording) recordingMacro.clear();
    }
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        log::info("Recording State: {}", Recording);
        m_fields->playingText = CCLabelBMFont::create("Playback Mode", "bigFont.fnt");
        m_fields->clickcountText = CCLabelBMFont::create("0", "chatFont.fnt");
        m_fields->clickFrame1 = CCSprite::create("btnUp.png"_spr);
        m_fields->clickFrame2 = CCSprite::create("btnDown.png"_spr);
        m_fields->playingText->setPosition({285, 275});
        m_fields->playingText->setOpacity(75);
        m_fields->playingText->setScale(0.75);
        m_fields->playingText->setID("MacroPlayback");
        m_fields->clickFrame1->setPosition({540,45});
        m_fields->clickFrame1->setScale(0.8);
        m_fields->clickFrame2->setPosition({540,45});
        m_fields->clickFrame2->setScale(0.8);
        m_fields->clickcountText->setPosition({m_fields->clickFrame1->getPositionX(), m_fields->clickFrame1->getPositionY() + 10});
        if (Recording) {
            m_fields->playingText->setVisible(false);
            m_fields->clickcountText->setVisible(false);
            m_fields->clickFrame1->setVisible(false);
            m_fields->clickFrame2->setVisible(false);
        }
        recordingMacro.clear();
        Recording = true;
        addChild(m_fields->playingText, 100);
        addChild(m_fields->clickFrame1, 100);
        addChild(m_fields->clickFrame2, 100);
        addChild(m_fields->clickcountText, 100);
        return true;
    }
    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->clickCount = 0;
        if (!Recording) {
            m_fields->clickcountText->setVisible(true);
            m_fields->playingText->setVisible(true);
            m_fields->clickFrame1->setVisible(true);
            m_fields->clickFrame2->setVisible(true);
        }
        else if (this->getChildByID("MacroPlayback")) m_fields->playingText->setVisible(false);
    }
    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);
        if (!Recording) {
            m_fields->clickcountText->setString(fmt::format("{}", m_fields->clickCount).c_str());
            if (!holding) m_fields->clickFrame1->setVisible(true);
            else m_fields->clickFrame1->setVisible(false);
            if (holding) m_fields->clickFrame2->setVisible(true);
            else m_fields->clickFrame2->setVisible(false);
        }
        if (!Recording && PlayLayer::getCurrentPercent() != 100 && !playMacro.empty()) {
            if (PlayLayer::getCurrentPercent() >= playMacro[0][0] && !holding) {
                gameInput(true, 1, true);
                holding = true;
                m_fields->clickCount += 1;
            }
            if (PlayLayer::getCurrentPercent() >= playMacro[0][1]) {
                gameInput(false, 1, true);
                playMacro.erase(playMacro.begin());
                holding = false;
            }
        }
    }
};

class $modify(MacroPauseLayer, PauseLayer) {
	void customSetup() {
        PauseLayer::customSetup();
        auto centerButtons = this->getChildByID("right-button-menu");

        CCSprite* playMacroSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        playMacroSprite->setColor({160,0,0});
        playMacroSprite->setScale(0.3);
        CCMenuItemSpriteExtra* playMacroBtn = CCMenuItemSpriteExtra::create(playMacroSprite, this, menu_selector(MacroPauseLayer::macroPlayback));

        centerButtons->addChild(playMacroBtn);
        centerButtons->updateLayout(false);
    }
    void macroPlayback(CCObject*) {
        PlayLayer* pl = PlayLayer::get();
        Recording = false;
        playMacro = recordingMacro;
        pl->resetLevel();
    }
    void onExit() {
        PauseLayer::onExit();
        recordingMacro.clear();
        Recording = true;
    }
};

class $modify(EndMacroLayer, EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();
        auto mainLayer = this->getChildByID("main-layer");
        auto buttonMenu = mainLayer->getChildByID("button-menu");
        CCSprite* replaySprite = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
        replaySprite->setColor({210,0,0});
        replaySprite->setScale(0.7);
        CCMenuItemSpriteExtra* replayBtn = CCMenuItemSpriteExtra::create(replaySprite, this, menu_selector(EndMacroLayer::onReplayStart));
        replayBtn->setPosition({179, -126});
        buttonMenu->addChild(replayBtn);
    }
    void onReplayStart(CCObject*) {
        EndLevelLayer::onReplay(nullptr);
        Recording = false;
        playMacro = recordingMacro;
    }
    void onExit() {
        EndLevelLayer::onExit();
        recordingMacro.clear();
        Recording = true;
    }
};

class $modify(GJBaseGameLayer) {
    void handleButton(bool hold, int clickType, bool plr1) {
        GJBaseGameLayer::handleButton(hold, clickType, plr1);
        if (Recording) {
            PlayLayer* pl = PlayLayer::get();
            if (hold) start = pl->getCurrentPercent();
            else {
                end = pl->getCurrentPercent();
                std::vector<float> input = {start, end};
                recordingMacro.push_back(input);
            }
        }
    }
};