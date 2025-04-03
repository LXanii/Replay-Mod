#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

bool Recording = true;
int deathCount;
float start;
float end;
bool holding1;
bool holding2;
std::vector<std::vector<float>> recordingMacro1 = {};
std::vector<std::vector<float>> playMacro1;
std::vector<std::vector<float>> recordingMacro2 = {};
std::vector<std::vector<float>> playMacro2;
//[{0983248, [[2,2], [3,3]], true}, {0983248, [[2,2], [3,3]], false}]

class $modify(PlayLayer) {

    struct Fields {
        CCLabelBMFont* playingText;
        CCLabelBMFont* clickcountText1;
        CCLabelBMFont* clickcountText2;
        CCSprite* clickFrame11;
        CCSprite* clickFrame12;
        CCSprite* clickFrame21;
        CCSprite* clickFrame22;
        int clickCount1;
        int clickCount2;
    };

    void gameInput(bool hold, int clickType, bool plr1) {PlayLayer::handleButton(hold, clickType, plr1);}
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        PlayLayer::destroyPlayer(p0, p1);
        deathCount += 1;
    }
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        m_fields->playingText = CCLabelBMFont::create("Playback Mode", "bigFont.fnt");
        m_fields->clickcountText1 = CCLabelBMFont::create("0", "chatFont.fnt");
        m_fields->clickcountText2 = CCLabelBMFont::create("0", "chatFont.fnt");
        m_fields->clickFrame11 = CCSprite::create("btnUp1.png"_spr);
        m_fields->clickFrame12 = CCSprite::create("btnDown.png"_spr);
        m_fields->clickFrame21 = CCSprite::create("btnUp2.png"_spr);
        m_fields->clickFrame22 = CCSprite::create("btnDown.png"_spr);
        m_fields->playingText->setPosition({285, 275});
        m_fields->playingText->setOpacity(75);
        m_fields->playingText->setScale(0.75);
        m_fields->playingText->setID("MacroPlayback");
        m_fields->clickFrame11->setPosition({490,45});
        m_fields->clickFrame11->setScale(0.8);
        m_fields->clickFrame12->setPosition({490,45});
        m_fields->clickFrame12->setScale(0.8);
        m_fields->clickFrame21->setPosition({520,45});
        m_fields->clickFrame21->setScale(0.8);
        m_fields->clickFrame22->setPosition({520,45});
        m_fields->clickFrame22->setScale(0.8);
        m_fields->clickcountText1->setPosition({m_fields->clickFrame11->getPositionX(), m_fields->clickFrame11->getPositionY() + 10});
        m_fields->clickcountText2->setPosition({m_fields->clickFrame21->getPositionX(), m_fields->clickFrame21->getPositionY() + 10});

        recordingMacro1.clear();
        recordingMacro2.clear();
        Recording = true;

        if (Recording) {
            m_fields->playingText->setVisible(false);
            m_fields->clickcountText1->setVisible(false);
            m_fields->clickFrame11->setVisible(false);
            m_fields->clickFrame12->setVisible(false);
            m_fields->clickcountText2->setVisible(false);
            m_fields->clickFrame21->setVisible(false);
            m_fields->clickFrame22->setVisible(false);
        }

        addChild(m_fields->playingText, 100);
        addChild(m_fields->clickFrame11, 100);
        addChild(m_fields->clickFrame12, 100);
        addChild(m_fields->clickcountText1, 100);
        addChild(m_fields->clickFrame21, 100);
        addChild(m_fields->clickFrame22, 100);
        addChild(m_fields->clickcountText2, 100);
        return true;
    }
    void updateAttempts() {
        PlayLayer::updateAttempts();
        if (Recording) {recordingMacro1.clear(); recordingMacro2.clear();}
    };
    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->clickCount1 = 0;
        m_fields->clickCount2 = 0;
        deathCount = -13;
        if (!Recording && this->getChildByID("MacroPlayback")) {
            m_fields->clickcountText1->setVisible(true);
            m_fields->playingText->setVisible(true);
            m_fields->clickFrame11->setVisible(true);
            m_fields->clickFrame12->setVisible(true);
            m_fields->clickcountText2->setVisible(true);
            m_fields->clickFrame21->setVisible(true);
            m_fields->clickFrame22->setVisible(true);
        }
        else if (this->getChildByID("MacroPlayback")) m_fields->playingText->setVisible(false);
    }
    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);
        if (!Recording) {
            m_fields->clickcountText1->setString(fmt::format("{}", m_fields->clickCount1).c_str());
            if (!holding1) m_fields->clickFrame11->setVisible(true);
            else m_fields->clickFrame11->setVisible(false);
            if (holding1) m_fields->clickFrame12->setVisible(true);
            else m_fields->clickFrame12->setVisible(false);

            m_fields->clickcountText2->setString(fmt::format("{}", m_fields->clickCount2).c_str());
            if (!holding2) m_fields->clickFrame21->setVisible(true);
            else m_fields->clickFrame21->setVisible(false);
            if (holding2) m_fields->clickFrame22->setVisible(true);
            else m_fields->clickFrame22->setVisible(false);
        }
        if (!Recording && PlayLayer::getCurrentPercent() != 100 && !playMacro1.empty()) {
            if (PlayLayer::getCurrentPercent() >= playMacro1[0][0] && !holding1) {
                gameInput(true, 1, true);
                holding1 = true;
                m_fields->clickCount1 += 1;
            }
            if (PlayLayer::getCurrentPercent() >= playMacro1[0][1]) {
                gameInput(false, 1, true);
                playMacro1.erase(playMacro1.begin());
                holding1 = false;
            }
        }
        if (!Recording && PlayLayer::getCurrentPercent() != 100 && !playMacro2.empty()) {
            if (PlayLayer::getCurrentPercent() >= playMacro2[0][0] && !holding2) {
                gameInput(true, 1, false);
                holding2 = true;
                m_fields->clickCount2 += 1;
            }
            if (PlayLayer::getCurrentPercent() >= playMacro2[0][1]) {
                gameInput(false, 1, false);
                playMacro2.erase(playMacro2.begin());
                holding2 = false;
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
        Recording = false;
        playMacro1 = recordingMacro1;
        playMacro2 = recordingMacro2;
        deathCount = -13;
        PlayLayer::get()->resetLevel();
    }
    void onExit() {
        PauseLayer::onExit();
        recordingMacro1.clear();
        recordingMacro2.clear();
        Recording = true;
        deathCount = -13;
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
        playMacro1 = recordingMacro1;
        playMacro2 = recordingMacro2;
        deathCount = -13;
    }
    void onExit() {
        EndLevelLayer::onExit();
        recordingMacro1.clear();
        recordingMacro2.clear();
        Recording = true;
        deathCount = -13;
    }
};

class $modify(GJBaseGameLayer) {
    void handleButton(bool hold, int clickType, bool plr1) {
        GJBaseGameLayer::handleButton(hold, clickType, plr1);
        if (Recording && plr1) {
            if (hold) start = PlayLayer::get()->getCurrentPercent();
            else {
                end = PlayLayer::get()->getCurrentPercent();
                std::vector<float> input = {start, end};
                recordingMacro1.push_back(input);
            }
        }
        if (Recording && !plr1) {
            if (hold) start = PlayLayer::get()->getCurrentPercent();
            else {
                end = PlayLayer::get()->getCurrentPercent();
                std::vector<float> input = {start, end};
                recordingMacro2.push_back(input);
            }
        }
    }
};