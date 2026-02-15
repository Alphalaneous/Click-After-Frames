#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer){

    struct Fields {
        bool m_doOriginal = false;
        bool m_softToggle = false;
        int m_lastRand = 0;
        bool m_isRandom = false;
        int m_randMin = 0;
        int m_randMax = 0;
        int m_delay = 0;
    };

    bool init(){
        if(!GJBaseGameLayer::init()) return false;
        auto fields = m_fields.self();
        fields->m_isRandom = Mod::get()->getSettingValue<bool>("random-toggle");
        fields->m_softToggle = Mod::get()->getSettingValue<bool>("soft-toggle");
        fields->m_randMin = Mod::get()->getSettingValue<int64_t>("rand-min");
        fields->m_randMax = Mod::get()->getSettingValue<int64_t>("rand-max");
        fields->m_delay = Mod::get()->getSettingValue<int64_t>("delay");
        return true;
    }

    void handleButton(bool down, int button, bool isPlayer1){
        auto fields = m_fields.self();

        if (fields->m_doOriginal || fields->m_softToggle) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
        else {
            int ms = 0;
            if (fields->m_isRandom) {
                if (down) {
                    if (fields->m_randMin <= fields->m_randMax) {
                        ms = utils::random::generate(fields->m_randMin, fields->m_randMax);
                        fields->m_lastRand = ms;
                    }
                }
                else ms = fields->m_lastRand;
            }
            else ms = fields->m_delay;

            runAction(CCSequence::create(
                CCDelayTime::create(ms / 1000.f),
                CallFuncExt::create([this, fields, down, button, isPlayer1] {
                    if (PlayLayer::get()) {
                        fields->m_doOriginal = true;
                        GJBaseGameLayer::handleButton(down, button, isPlayer1);
                        fields->m_doOriginal = false;
                    }
                }),
                nullptr
            ));
        }
    }
};