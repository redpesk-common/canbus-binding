#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <numeric>
#include <iterator>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <json.hpp>
#include "openxc/message_set.hpp"

#define EXIT_SUCCESS				0
#define EXIT_UNKNOWN_ERROR			1
#define EXIT_COMMAND_LINE_ERROR		2
#define EXIT_PROGRAM_ERROR			3

template <typename T>
struct generator
{
	T v_;
	std::string line_prefix_;
	generator(T v, std::string line_prefix = "") : v_{v}, line_prefix_{line_prefix} {}
};

template <>
struct generator<openxc::signal>
{
	const openxc::signal& v_;
	std::uint32_t index_;
	std::string line_prefix_;
	generator(const openxc::signal& v, std::uint32_t index, std::string line_prefix = "")
		: v_{v}, index_{index}, line_prefix_{line_prefix}
	{
	}
};

template <typename T>
generator<T> gen(const T& v, std::string line_prefix = "") { return generator<T>(v, line_prefix); }

generator<openxc::signal> gen(const openxc::signal& v, std::uint32_t index, std::string line_prefix = "")
{
	return generator<openxc::signal>(v, index, line_prefix);
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const generator<T>& v)
{
	o << v.line_prefix_ << v.v_;
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<bool>& v)
{
	o << v.line_prefix_ << (v.v_ ? "true" : "false");
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<float>& v)
{
	o << v.line_prefix_ << std::showpoint << v.v_ << "f";
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<std::string>& v)
{
	o << v.line_prefix_ << '\"' << v.v_ << '\"';
	return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const generator<std::vector<T>>& v)
{
	o << v.line_prefix_ << "{\n";
	auto sz = v.v_.size();
	for(const T& i : v.v_)
	{
		o << gen(i, v.line_prefix_ + '\t');
		if (sz > 1) o << ",";
		--sz;
		o << '\n';
	}
	o << v.line_prefix_ << '}';
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<openxc::message_set>& v)
{
	o	<< v.line_prefix_
		<< '{'
		<< "0, "
		<< gen(v.v_.name()) << ", "
		<< v.v_.buses().size() << ", "
		<< v.v_.messages().size() << ", "
		<< std::accumulate(
			std::begin(v.v_.messages()),
			std::end(v.v_.messages()),
			0,
			[](int sum, const openxc::can_message& p) { return sum + p.signals().size(); }
			) << ", "
		<< v.v_.commands().size() << ", "
		<< v.v_.diagnostic_messages().size() << "}";
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<openxc::can_message>& v)
{
	o	<< v.line_prefix_
		<< "can_message_definition_t("
		<< "0, "
		<< gen(v.v_.bus()) << ", "
		<< v.v_.id() << ", "
		<< "can_message_format_t::STANDARD, "
		<< "frequency_clock_t(" << gen(v.v_.max_frequency()) << "), "
		<< gen(v.v_.force_send_changed())
		<< ')';
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<std::map<std::string, std::vector<std::uint32_t>>>& v)
{
	o << v.line_prefix_ << "{\n";
	std::uint32_t c1 = v.v_.size();
	for(const auto& state : v.v_)
	{
		std::uint32_t c2 = state.second.size();
		for(const auto& i : state.second)
		{
			o << v.line_prefix_ << "\t" << "{" << i << ", " << gen(state.first) << "}";
			if (c1 > 1 || c2 > 1) o << ',';
			o << '\n';
			--c2;
		}
		--c1;
	}
	o << v.line_prefix_ << "}";
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<openxc::signal>& v)
{
	o	<< v.line_prefix_ << "{\n"
		<< v.line_prefix_ << "\t0,\n"
		<< v.line_prefix_ << "\t" << v.index_ << ",\n"
		<< v.line_prefix_ << "\t" << gen(v.v_.generic_name()) << ",\n"
		<< v.line_prefix_ << "\t" << v.v_.bit_position() << ",\n"
		<< v.line_prefix_ << "\t" << v.v_.bit_size() << ",\n"
		<< v.line_prefix_ << "\t" << gen(v.v_.factor()) << ", \n"
		<< v.line_prefix_ << "\t" << v.v_.offset() << ", \n"
		<< v.line_prefix_ << "\t" << "0,\n"
		<< v.line_prefix_ << "\t" << "0,\n"
		<< v.line_prefix_ << "\tfrequency_clock_t(" << gen(v.v_.max_frequency()) << "),\n"
		<< v.line_prefix_ << "\t" << gen(v.v_.send_same()) << ",\n"
		<< v.line_prefix_ << "\t" << gen(v.v_.force_send_changed()) << ",\n"
		<< gen(v.v_.states(), v.line_prefix_ + '\t') << ",\n"
		<< v.line_prefix_ << '\t' << gen(v.v_.writable()) << ",\n"
		<< v.line_prefix_ << '\t' << (v.v_.decoder().size() ? v.v_.decoder() : "nullptr") << ",\n"
		<< v.line_prefix_ << '\t' << (v.v_.encoder().size() ? v.v_.encoder() : "nullptr") << ",\n"
		<< v.line_prefix_ << '\t' << "false\n"
		<< v.line_prefix_ << "}";
	return o;
}

template <>
std::ostream& operator<<(std::ostream& o, const generator<openxc::diagnostic_message>& v)
{
	o	<< v.line_prefix_ << "{\n"
		<< v.line_prefix_ << "\t" << v.v_.pid() << ",\n"
		<< v.line_prefix_ << "\t" << gen(v.v_.name()) << ",\n"
		<< v.line_prefix_ << "\t" << 0 << ",\n"
		<< v.line_prefix_ << "\t" << 0 << ",\n"
		<< v.line_prefix_ << "\t" << "UNIT::INVALID" << ",\n"
		<< v.line_prefix_ << "\t" << gen(v.v_.frequency()) << ",\n"
		<< v.line_prefix_ << "\t" << (v.v_.decoder().size() ? v.v_.decoder() : "nullptr") << ",\n"
		<< v.line_prefix_ << "\t" << (v.v_.callback().size() ? v.v_.callback() : "nullptr") << ",\n"
		<< v.line_prefix_ << "\t" << "true" << "\n"
		<< v.line_prefix_ << "}";
	return o;
}

/// @brief Generate the configuration code.
/// @param[in] header Content to be inserted as a header.
/// @param[in] footer Content to be inserted as a footer.
/// @param[in] message_set Configuration read from the json file.
/// @param[in] out Stream to write on.
void generate(const std::string& header, const std::string& footer, const openxc::message_set& message_set, std::ostream& out)
{
	out << "#include \"configuration.hpp\"\n"
		<< "#include \"can/can-decoder.hpp\"\n\n";

	if (header.size()) out << header << "\n";

	out	<< "configuration_t::configuration_t()\n"
		<< "	: can_message_set_{" << gen(message_set) << "}\n"
		<< "	, can_message_definition_\n"
		<< "	{\n"
				<< gen(message_set.messages(), "\t\t") << '\n'
		<< "	}\n"
		<< "	, can_signals_\n"
		<< "	{\n";
		std::uint32_t message_count = message_set.messages().size();
		std::uint32_t index = 0;
		for(const openxc::can_message& m : message_set.messages())
		{
			out << "		{\n";
			std::uint32_t signal_count = m.signals().size();
			for(const openxc::signal& s : m.signals())
			{
				out << gen(s, index, "			");
				if (signal_count > 1) out << ',';
				--signal_count;
				out << '\n';
			}
			out << "		}";
			if (index + 1 < message_count) out << ',';
			++index;
			out << '\n';
		}
		out << "	}\n"
			<< "	, diagnostic_messages_\n"
			<< "	{\n"
			<< gen(message_set.diagnostic_messages(), "		") << "\n"
			<< "	}\n"
			<< "{\n"
			<< "}\n\n"
			<< "const std::string configuration_t::get_diagnostic_bus() const\n"
			<< "{\n";

		std::string active_bus = "";
		for (const auto& d : message_set.diagnostic_messages())
		{
			if (d.bus().size() == 0) std::cerr << "ERROR: The bus name should be set for each diagnostic message." << std::endl;
			if (active_bus.size() == 0) active_bus = d.bus();
			if (active_bus != d.bus()) std::cerr << "ERROR: The bus name should be the same for each diagnostic message." << std::endl;
		}

		out	<< "	return " << gen(active_bus) << ";\n"
			<< "}\n\n";
	out << footer << std::endl;
}

/// @brief Read whole file content to a string.
/// @param[in] file Path to the file.
/// @return A std::string which contains the file content. If @c file is an empty string, the return value is also empty.
/// @exception std::runtime_error Throw this exception if the specified file is not found or cannot be opened.
std::string read_file(const std::string& file)
{
	if(file.size() == 0) return std::string();
	
	std::string content;
	std::ifstream stream(file);
	if (stream)
	{
		stream.seekg(0, std::ios::end);
		content.reserve(stream.tellg());
		stream.seekg(0, std::ios::beg);
		content.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		return content;
	}
	std::stringstream ss;
	ss << "The specified file (" << file << ") is not found!";
	throw std::runtime_error(ss.str());
}

/// @brief Read whole file content as a json document.
/// @param[in] file Path to the file.
/// @return A @c nlohmann::json object.
/// @exception std::runtime_error Throw this exception if the specified file is not found or cannot be opened.
nlohmann::json read_json(const std::string& file)
{
	std::ifstream stream(file);
	if (stream)
	{
		nlohmann::json result;
		stream >> result;
		return result;
	}
	std::stringstream ss;
	ss << "The specified file (" << file << ") is not found!";
	throw std::runtime_error(ss.str());
}

/// @brief Entry point.
/// @param[in] argc Argument's count.
/// @param[in] argv Argument's array.
/// @return Exit code, zero if success.
int main(int argc, char** argv)
{
	//std::ios::sync_with_stdio(false);
	
	try
	{
		std::string appName = boost::filesystem::basename(argv[0]);
		std::string message_set_file;
		std::string output_file;
		std::string header_file;
		std::string footer_file;

		namespace bpo = boost::program_options;
		bpo::options_description desc("Options");
		desc.add_options()
			("help,h", "Display this help.")
			("message-set,m", bpo::value<std::string>(&message_set_file)->required(), "The message set definition file.")
			("output,o", bpo::value<std::string>(&output_file), "An output file, if not specified stdout is used.")
			("header,h", bpo::value<std::string>(&header_file), "A file to copy at the top of the generated output.")
			("footer,f", bpo::value<std::string>(&footer_file), "A file to copy at the end of the generated output.");

		bpo::variables_map vm;
		try
		{
			bpo::store(bpo::parse_command_line(argc, argv, desc), vm);

			if (vm.count("help"))
			{
				std::cout << desc << std::endl;
				return EXIT_SUCCESS;
			}

			bpo::notify(vm);

			std::stringstream header;
			header << read_file(header_file);
						
			std::string footer = read_file(footer_file);
			openxc::message_set message_set;
			message_set.from_json(read_json(message_set_file));

			boost::filesystem::path message_set_path(message_set_file);
			message_set_path.remove_filename();
			for(const auto& s : message_set.extra_sources())
			{
				boost::filesystem::path extra_source(s);
				if (!extra_source.is_complete()) extra_source = message_set_path / extra_source;
				header << "\n// >>>>> " << s << " >>>>>\n" << read_file(extra_source.string()) << "\n// <<<<< " << s << " <<<<<\n";
			}

			std::ofstream out;
			if (output_file.size())
			{
				out.open(output_file);
				if(!out)
				{
					std::stringstream ss;
					ss << "Can't open the ouput file (" << output_file << ") for writing!";
					throw std::runtime_error(ss.str());
				}
			}
			
			generate(header.str(), footer, message_set, output_file.size() ? out : std::cout); 

		}
		catch (bpo::required_option& e)
		{
			std::cerr << "ERROR: Argument required - " << e.what() << std::endl;
			std::cout << desc << std::endl;
			return EXIT_COMMAND_LINE_ERROR;
		}
		catch (bpo::error& e)
		{
			std::cerr << "ERROR: Command line error - " << e.what() << std::endl;
			std::cout << desc << std::endl;
			return EXIT_COMMAND_LINE_ERROR;
		}
		catch(std::exception& e)
		{
			std::cerr << "ERROR: " << e.what() << std::endl;
			return EXIT_PROGRAM_ERROR;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: Unhandled exception - " << e.what() << std::endl;
		return EXIT_UNKNOWN_ERROR;
	}

	return EXIT_SUCCESS;
}
