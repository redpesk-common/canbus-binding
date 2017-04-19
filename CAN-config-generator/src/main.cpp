/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <numeric>
#include <iterator>
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
		out << "		{\n";
		for(const openxc::can_message& m : message_set.messages())
		{
			std::uint32_t signal_count = m.signals().size();
			for(const openxc::signal& s : m.signals())
			{
				out << gen(s, index, "			");
				if (signal_count > 1) out << ',';
				--signal_count;
				out << '\n';
			}
			if (index + 1 < message_count) out << ',';
			++index;
		}
		out << "		}\n";
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

// function that show the help information
void showhelpinfo(char *s)
{
std::cout<<"Usage:   "<<s<<" <-m inpout.json> [-o configuration-generated.cpp]"<< std::endl;
std::cout<<"option:  "<<"-m  input.json : JSON file describing CAN messages and signals"<< std::endl;
std::cout<<"         "<<"-h header.cpp : header source file insert at the beginning of generated file"<< std::endl;
std::cout<<"         "<<"-f footer.cpp : footer source file append to generated file."<< std::endl;
std::cout<<"         "<<"-o configuration-generated.cpp : output source file. Name has to be configuration-generated.cpp"<< std::endl;
}

/// @brief Entry point.
/// @param[in] argc Argument's count.
/// @param[in] argv Argument's array.
/// @return Exit code, zero if success.
int main(int argc, char *argv[])
{
	try
	{
		std::string appName = argv[0];
		std::string message_set_file;
		std::string output_file;
		std::string header_file;
		std::string footer_file;

		char tmp;
		/*if the program is ran witout options ,it will show the usgage and exit*/
		if(argc == 1)
		{
			showhelpinfo(argv[0]);
			exit(1);
		}
		/*use function getopt to get the arguments with option."hu:p:s:v" indicate 
		that option h,v are the options without arguments while u,p,s are the
		options with arguments*/
		while((tmp=getopt(argc,argv,"m:h:f:o:"))!=-1)
		{
			switch(tmp)
			{
			case 'h':
				header_file = optarg;
				break;
			case 'f':
				footer_file = optarg;
				break;
			case 'm':
				message_set_file = optarg;
				break;
			case 'o':
				output_file = optarg;
				break;
			default:
				showhelpinfo(argv[0]);
			break;
			}
		}

		std::stringstream header;
		header << read_file(header_file);

		std::string footer = read_file(footer_file);
		openxc::message_set message_set;
		message_set.from_json(read_json(message_set_file));

		std::string message_set_path = dirname(strdup(message_set_file.c_str()));
		for(const auto& s : message_set.extra_sources())
		{
			std::string extra_source = s;
			extra_source = message_set_path + "/" + extra_source;
			header << "\n// >>>>> " << s << " >>>>>\n" << read_file(extra_source) << "\n// <<<<< " << s << " <<<<<\n";
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
	catch (std::exception& e)
	{
		std::cerr << "ERROR: Unhandled exception - " << e.what() << std::endl;
		return EXIT_UNKNOWN_ERROR;
	}
	return EXIT_SUCCESS;
}