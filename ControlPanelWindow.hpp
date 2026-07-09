#ifndef CONTROLPANELWINDOW_HPP
#define CONTROLPANELWINDOW_HPP

#include <unordered_map>
#include <string_view>

#include <cstdint>

#include <gtkmm.h>

#include "config.h"

class ControlPanelWindow : public Gtk::Window
{
private:
    static constexpr std::string_view _TITLE{"Control Panel"};

    //// Widgets (and their members)

    Gtk::ScrolledWindow _scroll;
    Gtk::Box _scroll_vbox;
    // TBD need to be class members or made with make_managed to work with
    //   signal callbacks (maybe move all other ctor-declared widgets into class?)
    Gtk::Button _save_image_button;
    Gtk::Button _load_image_button; 
    Gtk::Button _save_tonight_button;
    Gtk::Button _reset_button;

    Gtk::Button _pause_button;
    Gtk::Button _ff_button;
    Gtk::Button _rewind_button;

    Gtk::Button _save_LED_button;
    Gtk::Button _show_grid_button;
    Gtk::Button _enable_white_die_button;
    Gtk::Button _load_LED_button;
    

    Gtk::Label _current_file_label;

    Gtk::Label _tonights_movie_label;

    Gtk::Label _turn_on_time_label;

    Gtk::Label _current_time_label;

    Gtk::Label _video_position_label;

    //// Signal Callbacks (and helpers)

    bool
    _onDropLabelDrop(const Glib::ValueBase &, double, double);

    void
    _onGainValueChanged();

    void
    _onBlackValueChanged();

    void
    _onColorValueChanged();

    void
    _onHueValueChanged();

    void
    _onGammaValueChanged();

    void
    _onHShiftValueChanged();

    void
    _onRotateValueChanged();

    void
    _onSpeedValueChanged();

    void
    _onFilterValueChanged();

    void
    _onGainLEDValueChanged();

    void
    _onBlackLEDValueChanged();

    void
    _onColorLEDValueChanged();

    void
    _onHueLEDValueChanged();

    void
    _onGammaLEDValueChanged();

    void
    _onHShiftLEDValueChanged();


    void
    _onRedGainLEDValueChanged();

    void
    _onGreenGainLEDValueChanged();

    void
    _onBlueGainLEDValueChanged();
    
    

    std::string
    _sliderParamsToMockJSON();

    void
    _onSaveImageControlsClicked();

        void
    _onLoadImageControlsClicked();

    void
    _onSaveVideoForTonightClicked();

    bool
    _onCopyButtonTimeout();

    void
    _onCopyButtonClicked();

    void
    _onResetButtonClicked();

    void _onPauseClicked();
    void _onFastForwardClicked();
    void _onRewindClicked();

    void _onSaveLEDControlsClicked();
    void _onShowGridClicked();
    void _onEnableWhiteDieClicked();

    void _onResetLEDsClicked();

    //// Sliders Parameters

    struct _SliderTraits
    {
        std::string_view name;
        int32_t min_value;
        int32_t max_value;
        int32_t increment;
        int32_t default_value;
        void (ControlPanelWindow::*value_changed_callback)();

        _SliderTraits() = delete;
        _SliderTraits(
            const std::string_view name_,
            const int32_t min_value_, const int32_t max_value_,
            const int32_t increment_, const int32_t default_value_,
            void (ControlPanelWindow::*callback)()) : name(name_), min_value(min_value_), max_value(max_value_),
                                                      increment(increment_), default_value(default_value_),
                                                      value_changed_callback(callback) {}
    };

    void _updateSlidersFromGuiParams();

    std::string _current_movie_name = "No file loaded";

    void Update_Current_File_Label();

    struct _UIParamKeys
    {
        static constexpr std::string_view GAIN{"Gain"};
        static constexpr std::string_view BLACK{"Black"};
        static constexpr std::string_view COLOR{"Color"};
        static constexpr std::string_view HUE{"Hue"};
        static constexpr std::string_view GAMMA{"Gamma"};
        static constexpr std::string_view H_SHIFT{"H_Shift"};
        static constexpr std::string_view ROTATE{"Rotate"};
        static constexpr std::string_view SPEED{"Speed"};
        static constexpr std::string_view FILTER{"Filter"};

        static constexpr std::string_view GAIN_LED{"GAIN"};
        static constexpr std::string_view BLACK_LED{"BLACK"};
        static constexpr std::string_view COLOR_LED{"COLOR"};
        static constexpr std::string_view HUE_LED{"HUE"};
        static constexpr std::string_view GAMMA_LED{"GAMMA"};
        static constexpr std::string_view H_SHIFT_LED{"SHIFT"};

        static constexpr std::string_view RED_GAIN_LED{"RED"};
        static constexpr std::string_view GREEN_GAIN_LED{"GREEN"};
        static constexpr std::string_view BLUE_GAIN_LED{"BLUE"};

    };

    

inline static const std::vector<_SliderTraits> _SLIDER_TRAITS{
    {_UIParamKeys::GAIN,
     Process_Default_Limits[GAIN].Min,
     Process_Default_Limits[GAIN].Max,
     1,
     Process_Default_Limits[GAIN].Default,
     &ControlPanelWindow::_onGainValueChanged},

    {_UIParamKeys::BLACK,
     Process_Default_Limits[BLACK_LEVEL].Min,
     Process_Default_Limits[BLACK_LEVEL].Max,
     1,
     Process_Default_Limits[BLACK_LEVEL].Default,
     &ControlPanelWindow::_onBlackValueChanged},

    {_UIParamKeys::COLOR,
     Process_Default_Limits[COLOR_GAIN].Min,
     Process_Default_Limits[COLOR_GAIN].Max,
     1,
     Process_Default_Limits[COLOR_GAIN].Default,
     &ControlPanelWindow::_onColorValueChanged},

    {_UIParamKeys::HUE,
     Process_Default_Limits[COLOR_HUE].Min,
     Process_Default_Limits[COLOR_HUE].Max,
     1,
     Process_Default_Limits[COLOR_HUE].Default,
     &ControlPanelWindow::_onHueValueChanged},

    {_UIParamKeys::GAMMA,
     Process_Default_Limits[IMAGE_GAMMA].Min,
     Process_Default_Limits[IMAGE_GAMMA].Max,
     1,
     Process_Default_Limits[IMAGE_GAMMA].Default,
     &ControlPanelWindow::_onGammaValueChanged},

    {_UIParamKeys::H_SHIFT,
     Process_Default_Limits[H_SHIFT].Min,
     Process_Default_Limits[H_SHIFT].Max,
     1,
     Process_Default_Limits[H_SHIFT].Default,
     &ControlPanelWindow::_onHShiftValueChanged},

    {_UIParamKeys::ROTATE,
     Process_Default_Limits[ROTATE].Min,
     Process_Default_Limits[ROTATE].Max,
     1,
     Process_Default_Limits[ROTATE].Default,
     &ControlPanelWindow::_onRotateValueChanged},

    {_UIParamKeys::SPEED,
     Process_Default_Limits[SPEED].Min,
     Process_Default_Limits[SPEED].Max,
     1,
     Process_Default_Limits[SPEED].Default,
     &ControlPanelWindow::_onSpeedValueChanged},

    {_UIParamKeys::FILTER,
     Process_Default_Limits[FILTER_TYPE].Min,
     Process_Default_Limits[FILTER_TYPE].Max,
     1,
     Process_Default_Limits[FILTER_TYPE].Default,
     &ControlPanelWindow::_onFilterValueChanged},

    {_UIParamKeys::GAIN_LED,
     Process_Default_Limits[GAIN].Min,
     Process_Default_Limits[GAIN].Max,
     1,
     Process_Default_Limits[GAIN].Default,
     &ControlPanelWindow::_onGainLEDValueChanged},

    {_UIParamKeys::BLACK_LED,
     Process_Default_Limits[BLACK_LEVEL].Min,
     Process_Default_Limits[BLACK_LEVEL].Max,
     1,
     Process_Default_Limits[BLACK_LEVEL].Default,
     &ControlPanelWindow::_onBlackLEDValueChanged},

    {_UIParamKeys::COLOR_LED,
     Process_Default_Limits[COLOR_GAIN].Min,
     Process_Default_Limits[COLOR_GAIN].Max,
     1,
     Process_Default_Limits[COLOR_GAIN].Default,
     &ControlPanelWindow::_onColorLEDValueChanged},

    {_UIParamKeys::HUE_LED,
     Process_Default_Limits[COLOR_HUE].Min,
     Process_Default_Limits[COLOR_HUE].Max,
     1,
     Process_Default_Limits[COLOR_HUE].Default,
     &ControlPanelWindow::_onHueLEDValueChanged},

    {_UIParamKeys::GAMMA_LED,
     Process_Default_Limits[IMAGE_GAMMA].Min,
     Process_Default_Limits[IMAGE_GAMMA].Max,
     1,
     Process_Default_Limits[IMAGE_GAMMA].Default,
     &ControlPanelWindow::_onGammaLEDValueChanged},

    {_UIParamKeys::H_SHIFT_LED,
     Process_Default_Limits[H_SHIFT].Min,
     Process_Default_Limits[H_SHIFT].Max,
     1,
     Process_Default_Limits[H_SHIFT].Default,
     &ControlPanelWindow::_onHShiftLEDValueChanged},

    {_UIParamKeys::RED_GAIN_LED,
     Process_Default_Limits[RED_GAIN].Min,
     Process_Default_Limits[RED_GAIN].Max,
     1,
     Process_Default_Limits[RED_GAIN].Default,
     &ControlPanelWindow::_onRedGainLEDValueChanged},

    {_UIParamKeys::GREEN_GAIN_LED,
     Process_Default_Limits[GREEN_GAIN].Min,
     Process_Default_Limits[GREEN_GAIN].Max,
     1,
     Process_Default_Limits[GREEN_GAIN].Default,
     &ControlPanelWindow::_onGreenGainLEDValueChanged},

    {_UIParamKeys::BLUE_GAIN_LED,
     Process_Default_Limits[BLUE_GAIN].Min,
     Process_Default_Limits[BLUE_GAIN].Max,
     1,
     Process_Default_Limits[BLUE_GAIN].Default,
     &ControlPanelWindow::_onBlueGainLEDValueChanged}
};



    class _UIParamValue
    {
    private:
        Glib::RefPtr<Gtk::Adjustment> _adjustment;
        int _default_value;
        // TBD for temporary block()/unblock() of signals as in Jim's on_reset
        // https://libsigcplusplus.github.io/libsigcplusplus/reference/html/structsigc_1_1connection.html
        // sigc::connection              _signal_value_changed_conn;

    public:
        _UIParamValue() = delete;
        _UIParamValue(
            const Glib::RefPtr<Gtk::Adjustment> adjustment_,
            const int default_value_) : _adjustment(adjustment_), _default_value(default_value_) {}

        inline int
        get_value() const { return _adjustment->get_value(); }

        inline void
        set_value(int value) { _adjustment->set_value(value); }

        inline void
        restore_default() { _adjustment->set_value(_default_value); }
    };
    std::unordered_map<
        std::string_view, _UIParamValue>
        _slider_params;

public:
    ControlPanelWindow();
};

#endif // CONTROLPANELWINDOW_HPP
