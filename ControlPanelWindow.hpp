#ifndef CONTROLPANELWINDOW_HPP
#define CONTROLPANELWINDOW_HPP

#include <unordered_map>
#include <string_view>

#include <cstdint>

#include <gtkmm.h>

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
    Gtk::Button _save_tonight_button;
    Gtk::Button _reset_button;

    Gtk::Button _pause_button;
    Gtk::Button _ff_button;
    Gtk::Button _rewind_button;

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

    std::string
    _sliderParamsToMockJSON();

    void
    _onSaveImageControlsClicked();

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
    };

    inline static const std::vector<_SliderTraits> _SLIDER_TRAITS{
        {_UIParamKeys::GAIN, 0, 200, 1, 75,
         &ControlPanelWindow::_onGainValueChanged},
        {_UIParamKeys::BLACK, -100, 100, 1, 0,
         &ControlPanelWindow::_onBlackValueChanged},
        {_UIParamKeys::COLOR, 0, 200, 1, 100,
         &ControlPanelWindow::_onColorValueChanged},
        {_UIParamKeys::HUE, -180, 180, 1, 0,
         &ControlPanelWindow::_onHueValueChanged},
        {_UIParamKeys::GAMMA, 0, 100, 1, 25,
         &ControlPanelWindow::_onGammaValueChanged},
        {_UIParamKeys::H_SHIFT, 0, 100, 1, 0,
         &ControlPanelWindow::_onHShiftValueChanged},
        {_UIParamKeys::ROTATE, -100, 100, 1, 0,
         &ControlPanelWindow::_onRotateValueChanged},
        {_UIParamKeys::SPEED, 0, 200, 1, 100,
         &ControlPanelWindow::_onSpeedValueChanged},
        {_UIParamKeys::FILTER, 0, 10, 1, 3,
         &ControlPanelWindow::_onFilterValueChanged}};

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
