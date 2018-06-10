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
