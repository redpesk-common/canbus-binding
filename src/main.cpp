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

/// @brief Generate the configuration code.
/// @param[in] header Content to be inserted as a header.
/// @param[in] footer Content to be inserted as a footer.
/// @param[in] message_set Configuration read from the json file.
/// @param[in] out Stream to write on.
void generate(const std::string& header, const std::string& footer, const openxc::message_set& message_set, std::ostream& out)
{
	if (header.size()) out << header << "\n";
	else out << "#pragma once\n\n";
	
	out << "namespace generated {\n"
		<< "	class configuration_generated\n"
		<< "	{\n"
		<< "	private:\n"
		<< "		can_message_set_t can_message_set_;\n"
		<< "		std::vector<std::vector<can_message_definition_t>> can_message_definition_;\n"
		<< "		std::vector<std::vector<can_signal_t>> can_signals_;\n"
		<< "		std::vector<std::vector<obd2_signal_t>> obd2_signals_;\n"
		<< "	public:\n"
		<< "		configuration_generated()\n"
		<< "			: message_set_{0, \""
											<< message_set.name() << "\", "
											<< message_set.buses().size() << ", "
											<< message_set.messages().size() << ", "
											<< std::accumulate(
													std::begin(message_set.messages()),
													std::end(message_set.messages()),
													0,
													[](int sum, const std::map<std::string, openxc::can_message>::value_type& p) { return sum + p.second.signals().size(); }
											) << ", "
											<< message_set.commands().size() << ", "
											<< message_set.diagnostic_messages().size() << "}\n"
		<< "			, can_message_definition_{\n";

		std::uint32_t count = message_set.messages().size();
		for(const std::map<std::string, openxc::can_message>::value_type& m : message_set.messages())
		{
			out << "				{{\""	<< m.second.bus() << "\", "
											<< m.first << ", "
											<< "can_message_format_t::STANDARD, frequency_clock_t(), false}}";
			if (count > 1) out << ",";
			out << "\n";
			--count;
		}
		out << "			}\n";

		
		out << "			, can_signals_" << "..." << "\n"
			<< "			, obd2_signals_" << "..." << "\n"
			<< "		{\n"
			<< "		}\n"
			<< "	};\n"
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
	std::ios::sync_with_stdio(false);
	
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
			
			std::string header = read_file(header_file);
			std::string footer = read_file(footer_file);
			openxc::message_set message_set;
			message_set.from_json(read_json(message_set_file));
			std::ofstream out;
			if (output_file.size())
			{
				out = std::ofstream(output_file);
				if(!out)
				{
					std::stringstream ss;
					ss << "Can't open the ouput file (" << output_file << ") for writing!";
					throw std::runtime_error(ss.str());
				}
			}
			
			generate(header, footer, message_set, output_file.size() ? out : std::cout); 

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
