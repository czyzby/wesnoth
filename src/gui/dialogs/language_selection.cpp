/*
	Copyright (C) 2008 - 2025
	by Mark de Wever <koraq@xs4all.nl>
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/language_selection.hpp"

#include "gui/widgets/listbox.hpp"
#include "gui/widgets/toggle_button.hpp"
#include "gui/widgets/window.hpp"
#include "game_config.hpp"
#include "gettext.hpp"
#include "language.hpp"
#include "preferences/preferences.hpp"
#include "serialization/markup.hpp"

namespace gui2::dialogs
{

namespace
{

const std::string translations_wiki_url = "https://wiki.wesnoth.org/WesnothTranslations";

const std::string translations_stats_url = "https://gettext.wesnoth.org/";

}

REGISTER_DIALOG(language_selection)

language_selection::language_selection()
	: modal_dialog(window_id())
	, langs_(get_languages(true))
	, complete_langs_(langs_.size())
{
	bool show_all = get_min_translation_percent() == 0;

	const language_def& current_language = get_language();

	// Build language list and completion filter
	for(std::size_t i = 0; i < langs_.size(); ++i) {
		if(langs_[i] == current_language) {
			// Always show the initial language regardless of completion. If it's under
			// threshold then it probably means we should start by showing all languages
			// too, regardless of what the initial threshold setting is.
			complete_langs_[i] = true;
			if(langs_[i].percent < get_min_translation_percent()) {
				show_all = true;
			}
		} else {
			complete_langs_[i] = langs_[i].percent >= get_min_translation_percent();
		}
	}

	register_bool("show_all", true, show_all);
	// Markup needs to be enabled for the link to be highlighted
	register_label("contrib_url", true, translations_wiki_url, true);
	register_label("stats_url", true, translations_stats_url, true);
}

void language_selection::shown_filter_callback()
{
	toggle_button& show_all_toggle = find_widget<toggle_button>("show_all");
	listbox& list = find_widget<listbox>("language_list");

	if(show_all_toggle.get_value_bool()) {
		list.set_row_shown(boost::dynamic_bitset<>{langs_.size(), ~0UL});
	} else {
		list.set_row_shown(complete_langs_);
	}
}

void language_selection::pre_show()
{
	listbox& list = find_widget<listbox>("language_list");
	keyboard_capture(&list);

	toggle_button& show_all_toggle = find_widget<toggle_button>("show_all");
	connect_signal_mouse_left_click(show_all_toggle, std::bind(
			&language_selection::shown_filter_callback, this));

	const language_def& current_language = get_language();

	for(const auto& lang : langs_) {
		std::string lang_label = game_config::debug && !lang.localename.empty()
			? lang.language + "\n" + markup::tag("small", markup::tag("tt", lang.localename))
			: lang.language;

		list.add_row(widget_data{
			{ "language", {
				{ "label", std::move(lang_label) },
				{ "use_markup", "true" }
			}},
			{ "translated_total", {
				{ "label", markup::span_color(game_config::red_to_green(lang.percent), lang.percent, "%") },
				{ "use_markup", "true" }
			}},
		});

		if(lang == current_language) {
			list.select_last_row();
		}
	}

	// The view filter needs to be set after building the list to take the Show
	// All toggle value + language completion stats into account as needed.
	shown_filter_callback();
}

void language_selection::post_show()
{
	if(get_retval() == retval::OK) {
		const int res = find_widget<listbox>("language_list")
								.get_selected_row();

		assert(res != -1);

		::set_language(langs_[res]);
		prefs::get().set_locale(langs_[res].localename);
	}
}

} // namespace dialogs
