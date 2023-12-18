#pragma once

#include "lib.rs.h"

namespace rlog
{

	template <typename... Args>
	struct [[maybe_unused]] critical
	{
		critical() = delete;

		explicit critical(fmt::format_string<Args...> fmtstr,
			Args&&... args,
			SKSE::stl::source_location loc = SKSE::stl::source_location::current())
		{
			const auto msg = fmt::format(fmtstr, std::forward<Args>(args)...);
			log_error(msg);
		}
	};

	template <typename... Args>
	critical(fmt::format_string<Args...>, Args&&...) -> critical<Args...>;

	// I just don't trust C-land macros.

	template <typename... Args>
	struct [[maybe_unused]] error
	{
		error() = delete;

		explicit error(fmt::format_string<Args...> fmtstr, Args&&... args)
		{
			const auto msg = fmt::format(fmtstr, std::forward<Args>(args)...);
			log_error(msg);
		}
	};

	template <typename... Args>
	error(fmt::format_string<Args...>, Args&&...) -> error<Args...>;

	// warn templates

	template <typename... Args>
	struct [[maybe_unused]] warn
	{
		warn() = delete;

		explicit warn(fmt::format_string<Args...> fmtstr, Args&&... args)
		{
			const auto msg = fmt::format(fmtstr, std::forward<Args>(args)...);
			log_warn(msg);
		}
	};

	template <typename... Args>
	warn(fmt::format_string<Args...>, Args&&...) -> warn<Args...>;

	// info templates

	template <typename... Args>
	struct [[maybe_unused]] info
	{
		info() = delete;

		explicit info(fmt::format_string<Args...> fmtstr, Args&&... args)
		{
			const auto msg = fmt::format(fmtstr, std::forward<Args>(args)...);
			log_info(msg);
		}
	};

	template <typename... Args>
	info(fmt::format_string<Args...>, Args&&...) -> info<Args...>;

	// deboog templates

	template <typename... Args>
	struct [[maybe_unused]] debug
	{
		debug() = delete;

		explicit debug(fmt::format_string<Args...> fmtstr,
			Args&&... args,
			SKSE::stl::source_location loc = SKSE::stl::source_location::current())
		{
			const std::filesystem::path p = loc.file_name();
			auto filename                 = p.filename().lexically_normal().generic_string();

			const auto msg      = fmt::format(fmtstr, std::forward<Args>(args)...);
			const auto with_loc = fmt::format("{}:{}: {}", filename, static_cast<int>(loc.line()), msg);
			log_debug(with_loc);
		}
	};

	template <typename... Args>
	debug(fmt::format_string<Args...>, Args&&...) -> debug<Args...>;

	// trace templates

	template <typename... Args>
	struct [[maybe_unused]] trace
	{
		trace() = delete;

		explicit trace(fmt::format_string<Args...> fmtstr,
			Args&&... args,
			SKSE::stl::source_location loc = SKSE::stl::source_location::current())
		{
			const std::filesystem::path p = loc.file_name();
			auto filename                 = p.filename().lexically_normal().generic_string();

			const auto msg      = fmt::format(fmtstr, std::forward<Args>(args)...);
			const auto with_loc = fmt::format("{}:{}: {}", filename, static_cast<int>(loc.line()), msg);
			log_trace(with_loc);
		}
	};

	template <typename... Args>
	trace(fmt::format_string<Args...>, Args&&...) -> trace<Args...>;
}