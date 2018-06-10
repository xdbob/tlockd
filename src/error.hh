/*
 * This file is part of tlockd.
 *
 * Copyright (C) 2018 Antoine Damhet
 *
 * tlockd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tlockd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tlockd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ERROR_HH
#define ERROR_HH

# include <cerrno>
# include <system_error>

static inline std::system_error make_system_error(const std::string& what_arg) {
	auto err = std::error_code(errno, std::system_category());
	return std::system_error(err, what_arg);
}

static inline std::system_error make_system_error(const char *what_arg) {
	auto err = std::error_code(errno, std::system_category());
	return std::system_error(err, what_arg);
}

#endif /* !ERROR_HH */
