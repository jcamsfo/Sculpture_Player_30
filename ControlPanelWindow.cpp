#include <vector>
#include <cassert>
#include <filesystem>
#include <sstream>
#include <iomanip>

#include <fmt/format.h>
#include <gtkmm.h>

#include "ControlPanelWindow.hpp"
#include "globals.h"

namespace fs = std::filesystem;

bool ControlPanelWindow::_onDropLabelDrop(
    const Glib::ValueBase &value, double /*x*/, double /*y*/)
{
    assert(G_VALUE_HOLDS(value.gobj(), GDK_TYPE_FILE_LIST));

    Glib::Value<GdkFileList *> file_list_value;
    file_list_value.init(value.gobj());

    const std::vector<Glib::RefPtr<Gio::File>> files{
        Glib::SListHandler<Glib::RefPtr<Gio::File>>::slist_to_vector(
            gdk_file_list_get_files(file_list_value.get()),
            Glib::OWNERSHIP_SHALLOW)};

    fmt::print("{} files dropped:\n", files.size());

    for (const Glib::RefPtr<Gio::File> &file : files)
    {
        std::string dropped_path = file->get_path();

        _current_movie_name = fs::path(dropped_path).filename().string();
        Update_Current_File_Label();

        fmt::print("    {}\n", dropped_path);

        {
            std::lock_guard<std::mutex> lock(g_drop_path_mutex);
            g_drop_path = dropped_path;
        }

        g_new_drop_path = true;
        break;
    }

    return true;
}

void ControlPanelWindow::Update_Current_File_Label()
{
    int pos_m = current_position_mins.load();
    int pos_s = current_position_secs.load();

    int len_m = video_length_mins.load();
    int len_s = video_length_secs.load();

    std::ostringstream ss;

    ss << _current_movie_name
       << "    "
       << pos_m << ":"
       << std::setw(2) << std::setfill('0') << pos_s
       << " / "
       << len_m << ":"
       << std::setw(2) << std::setfill('0') << len_s;

    _current_file_label.set_text(ss.str());
}

void ControlPanelWindow::_onGainValueChanged()
{
    g_gui_params.Gain.store(_slider_params.at(_UIParamKeys::GAIN).get_value());
}

void ControlPanelWindow::_onBlackValueChanged()
{
    g_gui_params.Black_Level.store(_slider_params.at(_UIParamKeys::BLACK).get_value());
}

void ControlPanelWindow::_onColorValueChanged()
{
    g_gui_params.Color_Gain.store(_slider_params.at(_UIParamKeys::COLOR).get_value());
}

void ControlPanelWindow::_onHueValueChanged()
{
    g_gui_params.Color_Hue.store(_slider_params.at(_UIParamKeys::HUE).get_value());
}

void ControlPanelWindow::_onGammaValueChanged()
{
    g_gui_params.Image_Gamma.store(_slider_params.at(_UIParamKeys::GAMMA).get_value());
}

void ControlPanelWindow::_onHShiftValueChanged()
{
    g_gui_params.H_Shift.store(_slider_params.at(_UIParamKeys::H_SHIFT).get_value());
}

void ControlPanelWindow::_onRotateValueChanged()
{
    g_gui_params.Rotate.store(_slider_params.at(_UIParamKeys::ROTATE).get_value());
}

void ControlPanelWindow::_onSpeedValueChanged()
{
    g_gui_params.Speed.store(_slider_params.at(_UIParamKeys::SPEED).get_value());
}

void ControlPanelWindow::_onFilterValueChanged()
{
    g_gui_params.Filter_Type.store(_slider_params.at(_UIParamKeys::FILTER).get_value());
}

void ControlPanelWindow::_onGainLEDValueChanged()
{
    g_gui_LED_params.Gain.store(_slider_params.at(_UIParamKeys::GAIN_LED).get_value());
}

void ControlPanelWindow::_onBlackLEDValueChanged()
{
    g_gui_LED_params.Black_Level.store(_slider_params.at(_UIParamKeys::BLACK_LED).get_value());
}

void ControlPanelWindow::_onColorLEDValueChanged()
{
    g_gui_LED_params.Color_Gain.store(_slider_params.at(_UIParamKeys::COLOR_LED).get_value());
}

void ControlPanelWindow::_onHueLEDValueChanged()
{
    g_gui_LED_params.Color_Hue.store(_slider_params.at(_UIParamKeys::HUE_LED).get_value());
}

void ControlPanelWindow::_onGammaLEDValueChanged()
{
    g_gui_LED_params.Image_Gamma.store(_slider_params.at(_UIParamKeys::GAMMA_LED).get_value());
}

void ControlPanelWindow::_onHShiftLEDValueChanged()
{
    g_gui_LED_params.H_Shift.store(_slider_params.at(_UIParamKeys::H_SHIFT_LED).get_value());
}

void ControlPanelWindow::_onRedGainLEDValueChanged()
{
    g_gui_LED_params.Red_Gain.store(_slider_params.at(_UIParamKeys::RED_GAIN_LED).get_value());
}

void ControlPanelWindow::_onGreenGainLEDValueChanged()
{
    g_gui_LED_params.Green_Gain.store(_slider_params.at(_UIParamKeys::GREEN_GAIN_LED).get_value());
}

void ControlPanelWindow::_onBlueGainLEDValueChanged()
{
    g_gui_LED_params.Blue_Gain.store(_slider_params.at(_UIParamKeys::BLUE_GAIN_LED).get_value());
}

void ControlPanelWindow::_onWhiteGainLEDValueChanged()
{
    g_gui_LED_params.White_Gain.store(_slider_params.at(_UIParamKeys::WHITE_GAIN_LED).get_value());
}


std::string ControlPanelWindow::_sliderParamsToMockJSON()
{
    std::string mock_json;
    mock_json.append("-- mock JSON state (gtk4_demo_0 style):\n{\n");

    for (const auto &traits : _SLIDER_TRAITS)
    {
        mock_json.append(
            fmt::format("  {:?}: {},\n",
                        traits.name,
                        _slider_params.at(traits.name).get_value()));
    }

    mock_json.append("}");
    return mock_json;
}

void ControlPanelWindow::_onSaveImageControlsClicked()
{
    g_gui_save_image_params.store(true);
}

void ControlPanelWindow::_onLoadImageControlsClicked()
{
    g_gui_load_image_params.store(true);
}

void ControlPanelWindow::_onSaveVideoForTonightClicked()
{
    g_gui_save_video_for_tonight.store(true);
}

void ControlPanelWindow::_onResetButtonClicked()
{
    g_gui_reset_video_controls.store(true);
}

void ControlPanelWindow::_onPauseClicked()
{
    g_gui_pause.store(true);
}

void ControlPanelWindow::_onFastForwardClicked()
{
    g_gui_fast_forward.store(true);
}

void ControlPanelWindow::_onRewindClicked()
{
    g_gui_rewind.store(true);
}

void ControlPanelWindow::_onQuitAllClicked()
{
    g_gui_quit_all.store(true);
}

void ControlPanelWindow::_onSaveLEDControlsClicked()
{
    g_gui_save_LED_params.store(true);
}

void ControlPanelWindow::_onShowGridClicked()
{
    g_gui_show_grid.store(!g_gui_show_grid.load());
}

void ControlPanelWindow::_onEnableWhiteDieClicked()
{
    g_gui_enable_white_die.store(!g_gui_enable_white_die.load());
}

void ControlPanelWindow::_onResetLEDsClicked()
{
    g_gui_reset_LED_corrections.store(true);
}

ControlPanelWindow::ControlPanelWindow()
{
    set_title(_TITLE.data());

    set_default_size(350, 630);
    set_resizable(true);

    _scroll.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    set_child(_scroll);

    _scroll_vbox = Gtk::Box(Gtk::Orientation::VERTICAL, 4);
    _scroll_vbox.set_margin_top(16);
    _scroll_vbox.set_margin_bottom(16);
    _scroll_vbox.set_margin_start(20);
    _scroll_vbox.set_margin_end(20);
    _scroll.set_child(_scroll_vbox);

    Gtk::Box drop_row(Gtk::Orientation::HORIZONTAL, 10);

    Gtk::Label drop_label("Drop file here");
    drop_label.set_justify(Gtk::Justification::CENTER);
    drop_label.add_css_class("drop-zone");
    drop_label.set_size_request(180, -1);

    _current_movie_name = "No file loaded";
    _current_file_label.set_text("No file loaded");
    _current_file_label.set_xalign(0);
    _current_file_label.set_hexpand(true);
    _current_file_label.add_css_class("section-label");

    Glib::RefPtr<Gtk::DropTarget> dt =
        Gtk::DropTarget::create(GDK_TYPE_FILE_LIST, Gdk::DragAction::COPY);

    dt->signal_drop().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onDropLabelDrop),
        false);

    drop_label.add_controller(dt);

    drop_row.append(drop_label);
    drop_row.append(_current_file_label);

    _scroll_vbox.append(drop_row);

    Gtk::Separator drop_sliders_separator(Gtk::Orientation::HORIZONTAL);
    _scroll_vbox.append(drop_sliders_separator);

    auto is_LED_slider = [](const auto &name)
    {
        return name == _UIParamKeys::GAIN_LED ||
               name == _UIParamKeys::BLACK_LED ||
               name == _UIParamKeys::COLOR_LED ||
               name == _UIParamKeys::HUE_LED ||
               name == _UIParamKeys::GAMMA_LED ||
               name == _UIParamKeys::H_SHIFT_LED ||
               name == _UIParamKeys::RED_GAIN_LED ||
               name == _UIParamKeys::GREEN_GAIN_LED ||
               name == _UIParamKeys::BLUE_GAIN_LED ||
               name == _UIParamKeys::WHITE_GAIN_LED;
    };

    for (const _SliderTraits &traits : _SLIDER_TRAITS)
    {

        if (is_LED_slider(traits.name))
            continue;

        Gtk::Box row(Gtk::Orientation::HORIZONTAL, 4);
        row.set_margin_top(0);

        Gtk::Label label(traits.name.data());
        label.add_css_class("section-label");
        label.set_xalign(0);
        label.set_size_request(70, -1);

        Glib::RefPtr<Gtk::Adjustment> adjustment{
            Gtk::Adjustment::create(
                traits.default_value,
                traits.min_value,
                traits.max_value,
                traits.increment)};

        adjustment->signal_value_changed().connect(
            sigc::mem_fun(*this, traits.value_changed_callback),
            false);

        _slider_params.emplace(
            traits.name,
            _UIParamValue(adjustment, traits.default_value));

        Gtk::Scale slider{adjustment, Gtk::Orientation::HORIZONTAL};
        slider.set_hexpand(true);
        slider.set_draw_value(false);

        Gtk::SpinButton spin_button{adjustment};
        spin_button.set_size_request(65, -1);

        row.append(label);
        row.append(slider);
        row.append(spin_button);

        _scroll_vbox.append(row);
    }

    Gtk::Separator sliders_buttons_separator(Gtk::Orientation::HORIZONTAL);
    _scroll_vbox.append(sliders_buttons_separator);

    Gtk::Box button_box(Gtk::Orientation::HORIZONTAL, 10);
    button_box.set_halign(Gtk::Align::CENTER);

    _save_image_button.set_label("SAVE IMAGE\nPRESETS");
    _save_image_button.set_name("btn_save_image");
    _save_image_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onSaveImageControlsClicked),
        false);
    button_box.append(_save_image_button);

    _load_image_button.set_label("LOAD IMAGE\nPRESETS");
    _load_image_button.set_name("btn_load_image");
    _load_image_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onLoadImageControlsClicked),
        false);
    button_box.append(_load_image_button);

    _reset_button.set_label("RESET\nCONTROLS");
    _reset_button.set_name("btn_reset");
    _reset_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onResetButtonClicked),
        false);
    button_box.append(_reset_button);

    _save_tonight_button.set_label("SAVE VIDEO\nFOR TONIGHT");
    _save_tonight_button.set_name("btn_save_tonight");
    _save_tonight_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onSaveVideoForTonightClicked),
        false);
    button_box.append(_save_tonight_button);

    _scroll_vbox.append(button_box);

    Gtk::Box transport_box(Gtk::Orientation::HORIZONTAL, 10);
    transport_box.set_halign(Gtk::Align::START);
    transport_box.set_margin_top(12);

    _pause_button.set_label("Pause");
    _pause_button.set_name("btn_pause");
    _pause_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onPauseClicked));
    transport_box.append(_pause_button);

    _rewind_button.set_label("Rewind");
    _rewind_button.set_name("btn_rewind");
    _rewind_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onRewindClicked));
    transport_box.append(_rewind_button);

    _ff_button.set_label("Fast Forward");
    _ff_button.set_name("btn_ff");
    _ff_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onFastForwardClicked));
    transport_box.append(_ff_button);


    _quit_button.set_label("  Quit Application  ");
    _quit_button.set_name("btn_quit");
    _quit_button.set_margin_start(30);
    _quit_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onQuitAllClicked));
    transport_box.append(_quit_button);
    
    
    _scroll_vbox.append(transport_box);

    _tonights_movie_label.set_text("Tonight's Movie: None");
    _tonights_movie_label.set_xalign(0);
    _tonights_movie_label.set_valign(Gtk::Align::CENTER);
    _tonights_movie_label.add_css_class("tonight-movie");
    _tonights_movie_label.set_margin_top(10);

    _scroll_vbox.append(_tonights_movie_label);

    Gtk::Box time_row(Gtk::Orientation::HORIZONTAL, 24);
    time_row.set_margin_top(10);

    _current_time_label.set_text("Current: --:--");
    _current_time_label.set_xalign(0);
    _current_time_label.add_css_class("tonight-movie");

    _turn_on_time_label.set_text("Turn On: --:--");
    _turn_on_time_label.set_xalign(0);
    _turn_on_time_label.add_css_class("tonight-movie");

    time_row.append(_current_time_label);
    time_row.append(_turn_on_time_label);

    _scroll_vbox.append(time_row);

    Gtk::Label led_label("LED Calibration (Do Not Change)");
    led_label.add_css_class("section-label");
    led_label.add_css_class("led-header");
    led_label.set_halign(Gtk::Align::START);
    led_label.set_margin_top(50);
    led_label.set_margin_bottom(0);
    _scroll_vbox.append(led_label);

    Gtk::Separator LED_separator(Gtk::Orientation::HORIZONTAL);
    LED_separator.set_margin_top(5);
    LED_separator.set_margin_bottom(10);
    _scroll_vbox.append(LED_separator);

    for (const _SliderTraits &traits : _SLIDER_TRAITS)
    {
        if (!is_LED_slider(traits.name))
            continue;

        Gtk::Box row(Gtk::Orientation::HORIZONTAL, 4);
        row.set_margin_top(0);

        Gtk::Label label(traits.name.data());
        label.add_css_class("section-label");
        label.set_xalign(0);
        label.set_size_request(70, -1);

        Glib::RefPtr<Gtk::Adjustment> adjustment{
            Gtk::Adjustment::create(
                traits.default_value,
                traits.min_value,
                traits.max_value,
                traits.increment)};

        adjustment->signal_value_changed().connect(
            sigc::mem_fun(*this, traits.value_changed_callback),
            false);

        _slider_params.emplace(
            traits.name,
            _UIParamValue(adjustment, traits.default_value));

        Gtk::Scale slider{adjustment, Gtk::Orientation::HORIZONTAL};
        slider.set_hexpand(true);
        slider.set_draw_value(false);

        Gtk::SpinButton spin_button{adjustment};
        spin_button.set_size_request(65, -1);

        row.append(label);
        row.append(slider);
        row.append(spin_button);

        _scroll_vbox.append(row);
    }

    Gtk::Box LED_button_box(Gtk::Orientation::HORIZONTAL, 10);
    LED_button_box.set_halign(Gtk::Align::CENTER);
    LED_button_box.set_margin_top(10);

    _save_LED_button.set_label("SAVE LED\nPRESETS");
    _save_LED_button.set_name("btn_save_LED");
    _save_LED_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onSaveLEDControlsClicked),
        false);
    LED_button_box.append(_save_LED_button);

    _load_LED_button.set_label("RESET LED\nPRESETS");
    _load_LED_button.set_name("btn_load_LED");
    _load_LED_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onResetLEDsClicked),
        false);
    LED_button_box.append(_load_LED_button);

    _show_grid_button.set_label("SHOW\nSAMPLE GRID");
    _show_grid_button.set_name("btn_show_grid");
    _show_grid_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onShowGridClicked),
        false);
    LED_button_box.append(_show_grid_button);

    _enable_white_die_button.set_label("WHITE DIE\nCALIBRATE");
    _enable_white_die_button.set_name("btn_enable_white_die");
    _enable_white_die_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ControlPanelWindow::_onEnableWhiteDieClicked),
        false);
    LED_button_box.append(_enable_white_die_button);

    _scroll_vbox.append(LED_button_box);

    Glib::RefPtr<Gtk::CssProvider> css{Gtk::CssProvider::create()};

    css->load_from_data(R"(
        window { background-color: #2b2b2b; }

        .drop-zone {
            color: white;
            font-size: 12pt;
            background-color: #712e97ff;
            border-radius: 8px;
            padding: 14px;
        }

        .tonight-movie {
            font-size: 14pt;
            font-weight: bold;
            color: #80ffff;
        }

        .section-label {
            color: #dddddd;
            font-size: 11pt;
            font-weight: bold;
        }

        scale trough {
            background-color: #444;
            border-radius: 4px;
        }

        scale highlight {
            background-color: #4a9eff;
            border-radius: 4px;
        }

        spinbutton {
            background-color: #3c3f41;
            color: white;
            border-radius: 4px;
            border: 1px solid #555;
            padding: 0px 2px;
            min-height: 0;
            font-size: 8pt;
        }

        spinbutton text {
            min-height: 0;
            padding: 0;
        }

        spinbutton button {
            min-height: 0;
            padding: 0px 2px;
            color: #A0A0A0;
        }

        button {
            padding: 0;
            margin: 0;
            border: none;
            outline: none;
            box-shadow: none;
            background: none;
            min-height: 0;
            min-width: 0;
        }

        button:hover,
        button:active,
        button:focus {
            box-shadow: none;
        }

        #btn_save_image,
        #btn_save_image label {
            background-color: #4a9eff;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }

        #btn_save_image:hover,
        #btn_save_image:hover label {
            background-color: #3a8eef;
        }

        #btn_save_image:active,
        #btn_save_image:active label {
            background-color: #2a7edf;
        }


        #btn_load_image,
        #btn_load_image label {
            background-color: #4a9eff;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }

        #btn_load_image:hover,
        #btn_load_image:hover label {
            background-color: #3a8eef;
        }

        #btn_load_image:active,
        #btn_load_image:active label {
            background-color: #2a7edf;
        }
            
        

        #btn_save_tonight,
        #btn_save_tonight label {
            background-color: #4a9eff;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }

        #btn_save_tonight:hover,
        #btn_save_tonight:hover label {
            background-color: #3a8eef;
        }

        #btn_save_tonight:active,
        #btn_save_tonight:active label {
            background-color: #2a7edf;
        }



        #btn_reset,
        #btn_reset label {
            background-color: #4a9eff;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }

        #btn_reset:hover,
        #btn_reset:hover label {
            background-color: #3a8eef;
        }

        #btn_reset:active,
        #btn_reset:active label {
            background-color: #2a7edf;
        }




        #btn_pause,
        #btn_pause label,
        #btn_ff,
        #btn_ff label,
        #btn_rewind,
        #btn_rewind label {
            background-color: #3a7d44;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }

        #btn_pause:hover,
        #btn_pause:hover label,
        #btn_ff:hover,
        #btn_ff:hover label,
        #btn_rewind:hover,
        #btn_rewind:hover label {
            background-color: #2e6436;
        }
     
        #btn_pause:active,
        #btn_pause:active label,
        #btn_ff:active,
        #btn_ff:active label,
        #btn_rewind:active,
        #btn_rewind:active label {
            background-color: #255030;
        }


        #btn_quit,
        #btn_quit label,
        #btn_quit label {
            background-color: #a16b07ff;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }
        #btn_quit:hover,
        #btn_quit:hover label,
        #btn_quit:hover label {
            background-color: #684c01ff;
        }

        #btn_quit:active,
        #btn_quit:active label,  
        #btn_quit:active label {
            background-color: #3a2a00ff;
        }
            
        


        #btn_save_LED,
        #btn_save_LED label,
        #btn_load_LED,
        #btn_load_LED label,
        #btn_show_grid,
        #btn_show_grid label,
        #btn_enable_white_die,
        #btn_enable_white_die label {
            background-color: #4a9eff;
            color: white;
            border-radius: 4px;
            padding: 3px 8px;
            font-size: 8pt;
            font-weight: bold;
        }

        #btn_save_LED:hover,
        #btn_save_LED:hover label,
        #btn_load_LED:hover,
        #btn_load_LED:hover label,
        #btn_show_grid:hover,
        #btn_show_grid:hover label,
        #btn_enable_white_die:hover,
        #btn_enable_white_die:hover label {
            background-color: #3a8eef;
        }

        #btn_save_LED:active,
        #btn_save_LED:active label,
        #btn_load_LED:active,
        #btn_load_LED:active label,
        #btn_show_grid:active,
        #btn_show_grid:active label,
        #btn_enable_white_die:active,
        #btn_enable_white_die:active label {
            background-color: #2a7edf;
        }

            
        


        .led-header {
        font-size: 16pt;
}

    )");

    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(),
        css,
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    Glib::signal_timeout().connect(
        [this]()
        {
            if (g_gui_sliders_need_update.exchange(false))
            {
                _updateSlidersFromGuiParams();
            }

            if (g_current_movie_name_changed.exchange(false))
            {
                std::string name;

                {
                    std::lock_guard<std::mutex> lock(g_current_movie_name_mutex);
                    name = g_current_movie_name;
                }

                _current_movie_name = name;
                Update_Current_File_Label();
            }

            if (g_tonights_movie_name_changed.exchange(false))
            {
                std::string name;

                {
                    std::lock_guard<std::mutex> lock(g_tonights_movie_name_mutex);
                    name = g_tonights_movie_name;
                }

                _tonights_movie_label.set_text(
                    "Tonight's Movie:  " + name);
            }

            if (g_start_up_time_changed.exchange(false))
            {
                int on_h = start_up_hours.load();
                int on_m = start_up_mins.load();

                std::ostringstream ss;

                ss << "Turn On:  "
                   << on_h << ":"
                   << std::setw(2)
                   << std::setfill('0')
                   << on_m;

                _turn_on_time_label.set_text(ss.str());
            }

            if (g_current_time_changed.exchange(false))
            {
                int cur_h = current_hours.load();
                int cur_m = current_mins.load();

                std::ostringstream ss_time;

                ss_time << "Current:  "
                        << cur_h << ":"
                        << std::setw(2)
                        << std::setfill('0')
                        << cur_m;

                _current_time_label.set_text(ss_time.str());

                Update_Current_File_Label();
            }

            return true;
        },
        50);
}

void ControlPanelWindow::_updateSlidersFromGuiParams()
{
    _slider_params.at(_UIParamKeys::GAIN)
        .set_value(g_gui_params.Gain.load());

    _slider_params.at(_UIParamKeys::BLACK)
        .set_value(g_gui_params.Black_Level.load());

    _slider_params.at(_UIParamKeys::COLOR)
        .set_value(g_gui_params.Color_Gain.load());

    _slider_params.at(_UIParamKeys::HUE)
        .set_value(g_gui_params.Color_Hue.load());

    _slider_params.at(_UIParamKeys::GAMMA)
        .set_value(g_gui_params.Image_Gamma.load());

    _slider_params.at(_UIParamKeys::H_SHIFT)
        .set_value(g_gui_params.H_Shift.load());

    _slider_params.at(_UIParamKeys::ROTATE)
        .set_value(g_gui_params.Rotate.load());

    _slider_params.at(_UIParamKeys::SPEED)
        .set_value(g_gui_params.Speed.load());

    _slider_params.at(_UIParamKeys::FILTER)
        .set_value(g_gui_params.Filter_Type.load());

    _slider_params.at(_UIParamKeys::GAIN_LED)
        .set_value(g_gui_LED_params.Gain.load());

    _slider_params.at(_UIParamKeys::BLACK_LED)
        .set_value(g_gui_LED_params.Black_Level.load());

    _slider_params.at(_UIParamKeys::COLOR_LED)
        .set_value(g_gui_LED_params.Color_Gain.load());

    _slider_params.at(_UIParamKeys::HUE_LED)
        .set_value(g_gui_LED_params.Color_Hue.load());

    _slider_params.at(_UIParamKeys::GAMMA_LED)
        .set_value(g_gui_LED_params.Image_Gamma.load());

    _slider_params.at(_UIParamKeys::H_SHIFT_LED)
        .set_value(g_gui_LED_params.H_Shift.load());

    _slider_params.at(_UIParamKeys::RED_GAIN_LED)
        .set_value(g_gui_LED_params.Red_Gain.load());

    _slider_params.at(_UIParamKeys::GREEN_GAIN_LED)
        .set_value(g_gui_LED_params.Green_Gain.load());

    _slider_params.at(_UIParamKeys::BLUE_GAIN_LED)
        .set_value(g_gui_LED_params.Blue_Gain.load());          

    _slider_params.at(_UIParamKeys::WHITE_GAIN_LED)
        .set_value(g_gui_LED_params.White_Gain.load());     
}