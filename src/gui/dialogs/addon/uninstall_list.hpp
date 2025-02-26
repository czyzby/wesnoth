/*
	Copyright (C) 2011 - 2025
	by Iris Morelle <shadowm2006@gmail.com>
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#pragma once

#include "gui/dialogs/modal_dialog.hpp"

#include <map>

namespace gui2::dialogs
{

class addon_uninstall_list : public modal_dialog
{
public:
	/**
	 * Constructor.
	 *
	 * @param addon_titles_map
	 *                        Internal id <-> user-visible title mappings for
	 *                        the add-ons to display.
	 */
	explicit addon_uninstall_list(
			const std::map<std::string, std::string>& addon_titles_map)
		: modal_dialog(window_id())
		, titles_map_(addon_titles_map), ids_(), selections_()
	{
	}

	std::vector<std::string> selected_addons() const;

private:
	std::map<std::string, std::string> titles_map_;
	std::vector<std::string> ids_;
	std::map<std::string, bool> selections_;

	virtual const std::string& window_id() const override;

	virtual void pre_show() override;

	virtual void post_show() override;
};

} // namespace dialogs
