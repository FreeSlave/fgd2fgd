#include "Fgd.h"
#include "util.h"
#include <set>
#include <regex>
#include <iostream>
#include <fstream>

std::vector<std::string> splitStringIgnoringQuotes(const std::string& str, const std::string& delimitter)
{
	std::vector<std::string> split;
	if (str.empty() || delimitter.empty())
		return split;
	std::string s = str;
	size_t delimitLen = delimitter.length();

	while (s.size())
	{
		bool foundUnquotedDelimitter = false;
		size_t searchOffset = 0;
		while (!foundUnquotedDelimitter && searchOffset < s.size())
		{
			size_t delimitPos = s.find(delimitter, searchOffset);

			if (delimitPos == std::string::npos || delimitPos > s.size())
			{
				split.push_back(s);
				return split;
			}
			size_t quoteCount = 0;
			for (size_t i = 0; i < delimitPos; i++)
			{
				quoteCount += s[i] == '"' && (i == 0 || s[i - 1] != '\\');
			}

			if (quoteCount % 2 == 1)
			{
				searchOffset = delimitPos + 1;
				continue;
			}
			if (delimitPos != 0)
				split.emplace_back(s.substr(0, delimitPos));

			s = s.substr(delimitPos + delimitLen);
			foundUnquotedDelimitter = true;
		}

		if (!foundUnquotedDelimitter)
		{
			break;
		}

	}

	return split;
}

std::vector<std::string> splitString(const std::string& str, const std::string& delimiter, int maxParts = 0)
{
	std::string s = str;
	std::vector<std::string> split;
	size_t pos;
	while ((pos = s.find(delimiter)) != std::string::npos && (maxParts == 0 || (int)split.size() < maxParts - 1)) {
		if (pos != 0) {
			split.push_back(s.substr(0, pos));
		}
		s.erase(0, pos + delimiter.length());
	}
	if (!s.empty())
		split.push_back(s);
	return split;
}

std::string getValueInParens(std::string s)
{
	if (s.length() <= 2)
	{
		replaceAll(s, "(", "");
		replaceAll(s, ")", "");
		return s;
	}

	auto find1 = s.find('(');
	auto find2 = s.rfind(')');
	if (find1 == std::string::npos || find1 >= find2)
	{
		replaceAll(s, "(", "");
		replaceAll(s, ")", "");
		return s;
	}
	return s.substr(find1 + 1, (find2 - find1) - 1);
}

std::string getValueInQuotes(std::string s)
{
	if (s.length() <= 2)
	{
		replaceAll(s, "\"", "");
		return s;
	}

	auto find1 = s.find('\"');
	auto find2 = s.rfind('\"');
	if (find1 == std::string::npos || find1 == find2)
	{
		replaceAll(s, "\"", "");
		return s;
	}
	return s.substr(find1 + 1, (find2 - find1) - 1);
}

vec3 parseVector(const std::string& s)
{
	vec3 v;
	std::vector<std::string> parts = splitString(s, " ");

	while (parts.size() < 3)
	{
		parts.push_back("0");
	}

	v.x = (float)atof(parts[0].c_str());
	v.y = (float)atof(parts[1].c_str());
	v.z = (float)atof(parts[2].c_str());
	return v;
}

enum class FIXUPPATH_SLASH
{
	FIXUPPATH_SLASH_CREATE,
	FIXUPPATH_SLASH_SKIP,
	FIXUPPATH_SLASH_REMOVE
};

void fixupPath(std::string& path, FIXUPPATH_SLASH startslash, FIXUPPATH_SLASH endslash)
{
	if (path.empty())
		return;
	replaceAll(path, "\"", "");
	replaceAll(path, "\'", "");
	replaceAll(path, "/", "\\");
	replaceAll(path, "\\\\", "\\");
	replaceAll(path, "\\", "/");
	replaceAll(path, "//", "/");
	if (startslash == FIXUPPATH_SLASH::FIXUPPATH_SLASH_CREATE)
	{
		if (path[0] != '\\' && path[0] != '/')
		{
			path = "/" + path;
		}
	}
	else if (startslash == FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE)
	{
		if (path[0] == '\\' || path[0] == '/')
		{
			path.erase(path.begin());
		}
	}

	if (endslash == FIXUPPATH_SLASH::FIXUPPATH_SLASH_CREATE)
	{
		if (path.empty() || (path.back() != '\\' && path.back() != '/'))
		{
			path = path + "/";
		}
	}
	else if (endslash == FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE)
	{
		if (path.empty())
			return;

		if (path.back() == '\\' || path.back() == '/')
		{
			path.pop_back();
		}
	}

	replaceAll(path, "/", "\\");
	replaceAll(path, "\\\\", "\\");
	replaceAll(path, "\\", "/");
	replaceAll(path, "//", "/");
}

std::string basename(const std::string& path)
{
	size_t lastSlash = path.find_last_of("\\/");
	if (lastSlash != std::string::npos)
	{
		return path.substr(lastSlash + 1);
	}
	return path;
}

std::string stripExt(const std::string& path)
{
	size_t lastDot = path.find_last_of('.');
	if (lastDot != std::string::npos)
	{
		return path.substr(0, lastDot);
	}
	return path;
}

bool stringGroupStarts(const std::string& s)
{
	if (s.find('(') != std::string::npos)
	{
		return s.find(')') == std::string::npos;
	}

	size_t startStringPos = s.find('\"');
	if (startStringPos != std::string::npos)
	{
		size_t endStringPos = s.rfind('\"');
		return endStringPos == startStringPos || endStringPos == std::string::npos;
	}

	return false;
}

bool stringGroupEnds(const std::string& s)
{
	return s.find(')') != std::string::npos || s.find('\"') != std::string::npos;
}

std::vector<std::string> groupParts(std::vector<std::string>& ungrouped)
{
	std::vector<std::string> grouped;

	for (size_t i = 0; i < ungrouped.size(); i++)
	{
		if (stringGroupStarts(ungrouped[i]))
		{
			std::string groupedPart = ungrouped[i];
			i++;
			for (; i < ungrouped.size(); i++)
			{
				groupedPart += " " + ungrouped[i];
				if (stringGroupEnds(ungrouped[i]))
				{
					break;
				}
			}
			grouped.push_back(groupedPart);
		}
		else
		{
			grouped.push_back(ungrouped[i]);
		}
	}

	return grouped;
}

std::map<std::string, int> fgdKeyTypes{
	{"integer", FGD_KEY_INTEGER},
	{"choices", FGD_KEY_CHOICES},
	{"flags", FGD_KEY_FLAGS},
	{"color255", FGD_KEY_RGB},
	{"studio", FGD_KEY_STUDIO},
	{"sound", FGD_KEY_SOUND},
	{"sprite", FGD_KEY_SPRITE},
	{"target_source", FGD_KEY_TARGET_SRC},
	{"target_destination", FGD_KEY_TARGET_DST}
};

Fgd::Fgd(std::string _path)
{
	this->path = _path;
	this->name = stripExt(basename(path));
	this->lineNum = 0;
}

Fgd::~Fgd()
{
	for (size_t i = 0; i < classes.size(); i++)
	{
		delete classes[i];
	}
}

bool Fgd::parse()
{
	std::regex brackEnd(R"(\s*\[\s*\]\s*$)");

	std::ifstream in(path);
	if (!in)
		return false;

	lineNum = 0;
	line.clear();

	std::vector<std::string> inputLines;
	std::vector<int> inputLineNums;
	while (std::getline(in, line))
	{
		lineNum++;
		line = trimSpaces(line);

		if (line.empty() || line.starts_with("//"))
			continue;

		if (line[0] == '[' || line[0] == ']')
		{
			inputLines.push_back(line);
			inputLineNums.push_back(lineNum);
		}
		else if (line.ends_with('['))
		{
			line.pop_back();
			inputLines.push_back(line);
			inputLines.push_back("[");
			inputLineNums.push_back(lineNum);
			inputLineNums.push_back(lineNum);
		}
		else
		{
			replaceAll(line, "//", " :: :: ");
			if (line.find_first_of('[') != std::string::npos)
			{
				auto split = splitStringIgnoringQuotes(line, "[");
				if (split.size())
				{
					bool added = false;
					for (auto& s : split)
					{
						s = trimSpaces(s);

						if (s.empty())
							continue;

						inputLines.push_back(s);
						inputLines.push_back("[");
						inputLineNums.push_back(lineNum);
						inputLineNums.push_back(lineNum);
						added = true;
					}
					if (added)
					{
						inputLineNums.pop_back();
						inputLines.pop_back();
					}
				}
			}
			else if (line.find_first_of(']') != std::string::npos)
			{
				auto split = splitStringIgnoringQuotes(line, "]");
				if (split.size())
				{
					bool added = false;
					for (auto& s : split)
					{
						s = trimSpaces(s);

						if (s.empty())
							continue;

						inputLines.push_back(s);
						inputLines.push_back("]");
						inputLineNums.push_back(lineNum);
						inputLineNums.push_back(lineNum);
						added = true;
					}
					if (added)
					{
						inputLineNums.pop_back();
						inputLines.pop_back();
					}
				}
			}
			else if (line.ends_with(']'))
			{
				line.pop_back();
				inputLines.push_back(line);
				inputLines.push_back("\n");
				inputLines.push_back("]");
				inputLineNums.push_back(lineNum);
				inputLineNums.push_back(lineNum);
				inputLineNums.push_back(lineNum);
			}
			else
			{
				inputLines.push_back(line);
				inputLineNums.push_back(lineNum);
			}
		}
	}

	//std::ostringstream outs;
	//for (const auto& s : inputLines)
	//{
	//	outs << s << "\n";
	//}
	//writeFile(path + "_test.fgd", outs.str());

	FgdClass* fgdClass = new FgdClass();
	int bracketNestLevel = 0;

	line.clear();
	for (size_t i = 0; i < inputLines.size(); i++)
	{
		line = inputLines[i];
		lineNum = inputLineNums[i];

		if (line[0] == '@')
		{
			if (bracketNestLevel)
			{
				print_log("ERROR: New FGD class definition starts before previous one ends (line {}) in FGD {}\n", lineNum, name);

				if (fgdClass->isSprite && !fgdClass->sprite.size())
				{
					for (auto& kv : fgdClass->keyvalues)
					{
						if (kv.name == "model" && kv.defaultValue.size())
						{
							fgdClass->sprite = kv.defaultValue;
							fixupPath(fgdClass->sprite, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE);
							break;
						}
					}
				}

				classes.push_back(fgdClass);
				fgdClass = new FgdClass();
				bracketNestLevel = 0;
			}

			parseClassHeader(*fgdClass);
		}

		if ((line.size() && line[0] == '['))
		{
			bracketNestLevel++;
		}

		if ((line.size() && (line[0] == ']' || line[line.size() - 1] == ']')))
		{
			bracketNestLevel--;
			if (bracketNestLevel == 0)
			{
				if (fgdClass->isSprite && !fgdClass->sprite.size())
				{
					for (auto& kv : fgdClass->keyvalues)
					{
						if (kv.name == "model" && kv.defaultValue.size())
						{
							fgdClass->sprite = kv.defaultValue;
							fixupPath(fgdClass->sprite, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE);
							break;
						}
					}
				}
				classes.push_back(fgdClass);
				fgdClass = new FgdClass(); //memory leak
			}
		}

		if (bracketNestLevel == 0 && (line.rfind('[') != std::string::npos && std::regex_search(line, brackEnd)))
		{
			if (fgdClass->isSprite && !fgdClass->sprite.size())
			{
				for (auto& kv : fgdClass->keyvalues)
				{
					if (kv.name == "model" && kv.defaultValue.size())
					{
						fgdClass->sprite = kv.defaultValue;
						fixupPath(fgdClass->sprite, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE);
						break;
					}
				}
			}
			classes.push_back(fgdClass);
			fgdClass = new FgdClass(); //memory leak
			continue;
		}

		if (line.size() && (line[0] == '[' || line[0] == ']' || line[line.size() - 1] == ']'))
		{
			continue;
		}

		if (bracketNestLevel == 1)
		{
			parseKeyvalue(*fgdClass);
		}

		if (bracketNestLevel == 2)
		{
			if (fgdClass->keyvalues.empty())
			{
				print_log("ERROR: Choice values begin before any keyvalue are defined (line {}) in FGD {}\n", lineNum, name);
				continue;
			}
			KeyvalueDef& lastKey = fgdClass->keyvalues[fgdClass->keyvalues.size() - 1];
			parseChoicesOrFlags(lastKey);
		}
	}

	return true;
}

void Fgd::parseClassHeader(FgdClass& fgdClass)
{
	std::vector<std::string> headerParts = splitString(line, "=", 2);

	if (headerParts.empty())
	{
		print_log("ERROR: Unexpected end of class header (line {}) in FGD {}\n", lineNum, name);
		return;
	}

	// group parts enclosed in parens or quotes
	std::vector<std::string> typeParts = splitString(trimSpaces(headerParts[0]), " ");
	typeParts = groupParts(typeParts);

	std::string classType = toLowerCase(typeParts[0]);

	if (classType == "@baseclass")
	{
		fgdClass.classType = FGD_CLASS_BASE;
	}
	else if (classType == "@solidclass")
	{
		fgdClass.classType = FGD_CLASS_SOLID;
	}
	else if (classType == "@pointclass")
	{
		fgdClass.classType = FGD_CLASS_POINT;
	}
	else
	{
		print_log("ERROR: Unrecognized FGD class type '{}' in FGD {}\n", typeParts[0], name);
	}

	// parse constructors/properties
	for (size_t i = 1; i < typeParts.size(); i++)
	{
		std::string lpart = toLowerCase(typeParts[i]);

		if (lpart.starts_with("base("))
		{
			std::vector<std::string> baseClassList = splitString(getValueInParens(typeParts[i]), ",");
			for (size_t k = 0; k < baseClassList.size(); k++)
			{
				std::string baseClass = trimSpaces(baseClassList[k]);
				fgdClass.baseClasses.push_back(baseClass);
			}
		}
		else if (lpart.starts_with("size("))
		{
			std::vector<std::string> sizeList = splitString(getValueInParens(typeParts[i]), ",");

			if (sizeList.size() == 1)
			{
				vec3 size = parseVector(sizeList[0]);
				fgdClass.mins = size * -0.5f;
				fgdClass.maxs = size * 0.5f;
			}
			else if (sizeList.size() == 2)
			{
				fgdClass.mins = parseVector(sizeList[0]);
				fgdClass.maxs = parseVector(sizeList[1]);
			}
			else
			{
				print_log("ERROR: Expected 2 vectors in size() property (line {}) in FGD {}\n", lineNum, name);
			}

			fgdClass.sizeSet = true;
		}
		else if (lpart.starts_with("color("))
		{
			std::vector<std::string> nums = splitString(getValueInParens(typeParts[i]), " ");

			if (nums.size() == 3)
			{
				fgdClass.color = { (unsigned char)atoi(nums[0].c_str()), (unsigned char)atoi(nums[1].c_str()), (unsigned char)atoi(nums[2].c_str()) };
			}
			else
			{
				print_log("ERROR: Expected 3 components in color() property (line {}) in FGD {}\n", lineNum, name);
			}
			fgdClass.colorSet = true;
		}
		else if (lpart.starts_with("offset("))
		{
			std::vector<std::string> nums = splitString(getValueInParens(typeParts[i]), " ");

			if (nums.size() == 3)
			{
				fgdClass.offset = { (float)atof(nums[0].c_str()), (float)atof(nums[1].c_str()),(float)atof(nums[2].c_str()) };
			}
			else
			{
				print_log("ERROR: Expected 3 components in offset() property (line {}) in FGD {}\n", lineNum, name);
			}
		}
		else if (lpart.starts_with("studio("))
		{
			std::string mdlpath = getValueInParens(typeParts[i]);
			if (mdlpath.size())
			{
				fgdClass.model = mdlpath;
				fixupPath(fgdClass.model, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE);
			}
			fgdClass.isModel = true;
		}
		else if (lpart.starts_with("sequence("))
		{
			fgdClass.modelSequence = atoi(getValueInParens(typeParts[i]).c_str());
		}
		else if (lpart.starts_with("body("))
		{
			fgdClass.modelBody = atoi(getValueInParens(typeParts[i]).c_str());
		}
		else if (lpart.starts_with("iconsprite("))
		{
			fgdClass.sprite = getValueInParens(typeParts[i]);
			fgdClass.isSprite = true;
			if (fgdClass.sprite.size())
				fixupPath(fgdClass.sprite, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE);
		}
		else if (lpart.starts_with("sprite("))
		{
			fgdClass.sprite = getValueInParens(typeParts[i]);
			fgdClass.isSprite = true;
			if (fgdClass.sprite.size())
				fixupPath(fgdClass.sprite, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE, FIXUPPATH_SLASH::FIXUPPATH_SLASH_REMOVE);
		}
		else if (lpart.starts_with("decal("))
		{
			fgdClass.isDecal = true;
		}
		else if (lpart.starts_with("flags("))
		{
			std::vector<std::string> flagsList = splitString(getValueInParens(typeParts[i]), ",");
			for (size_t k = 0; k < flagsList.size(); k++)
			{
				std::string flag = trimSpaces(flagsList[k]);
				if (flag == "Angle")
					fgdClass.hasAngles = true; // force using angles/angle key ?
				else if (flag == "Path" || flag == "Light")
					;
				else
					print_log("WARNING: Unrecognized type flags value {} (line {}) in FGD {}\n", flag, lineNum, name);
			}
		}
		else if (typeParts[i].find('(') != std::string::npos)
		{
			std::string typeName = typeParts[i].substr(0, typeParts[i].find('('));
			print_log("WARNING: Unrecognized type {} (line {}) in FGD {}\n", typeName, lineNum, name);
		}
	}

	if (headerParts.size() <= 1)
	{
		print_log("ERROR: Unexpected end of class header (line {}) in FGD {}\n", lineNum, name);
		return;
	}

	std::vector<std::string> nameParts = splitStringIgnoringQuotes(headerParts[1], ":");
	if (nameParts.size() >= 1)
	{
		fgdClass.name = trimSpaces(nameParts[0]);
		// strips brackets if they're there
		// fgdClass.name = fgdClass.name.substr(0, fgdClass.name.find(' '));
		nameParts.erase(nameParts.begin());
	}

	fgdClass.description = "";
	if (nameParts.size() >= 1)
	{
		for (size_t i = 0; i < nameParts.size(); i++)
		{
			std::string input = getValueInQuotes(nameParts[i]);
			trimSpaces(input);
			if (input.size())
				fgdClass.description += input + "\n";
		}

		if (fgdClass.description.size())
			fgdClass.description.pop_back();
	}
}

void Fgd::parseKeyvalue(FgdClass& outClass)
{
	std::vector<std::string> keyParts = splitStringIgnoringQuotes(line, ":");

	KeyvalueDef def;

	def.name = keyParts[0].substr(0, keyParts[0].find('('));
	def.valueType = toLowerCase(getValueInParens(keyParts[0]));

	def.iType = FGD_KEY_STRING;
	if (fgdKeyTypes.find(def.valueType) != fgdKeyTypes.end())
	{
		def.iType = fgdKeyTypes[def.valueType];
	}

	if (keyParts.size() > 1)
		def.shortDescription = getValueInQuotes(keyParts[1]);
	else
	{
		def.shortDescription = def.name;

		// capitalize (infodecal)
		if ((def.shortDescription[0] > 96) && (def.shortDescription[0] < 123))
			def.shortDescription[0] = def.shortDescription[0] - 32;
	}

	if (keyParts.size() > 2)
	{
		if (keyParts[2].find('=') != std::string::npos)
		{ // choice
			def.defaultValue = trimSpaces(keyParts[2].substr(0, keyParts[2].find('=')));
		}
		else if (keyParts[2].find('\"') != std::string::npos)
		{ // std::string
			def.defaultValue = getValueInQuotes(keyParts[2]);
		}
		else
		{ // integer
			def.defaultValue = trimSpaces(keyParts[2]);
		}
		def.fullDescription = "";
		if (keyParts.size() > 3)
		{
			for (size_t i = 3; i < keyParts.size(); i++)
			{
				std::string input = getValueInQuotes(keyParts[i]);
				trimSpaces(input);
				if (input.size())
					def.fullDescription += input + "\n";
			}

			if (def.fullDescription.size())
				def.fullDescription.pop_back();
		}
	}

	outClass.keyvalues.push_back(def);
}

void Fgd::parseChoicesOrFlags(KeyvalueDef& outKey)
{
	std::vector<std::string> keyParts = splitStringIgnoringQuotes(line, ":");

	KeyvalueChoice def;

	if (keyParts[0].find('\"') != std::string::npos)
	{
		def.svalue = getValueInQuotes(keyParts[0]);
		def.ivalue = 0;
		def.isInteger = false;
	}
	else
	{
		def.svalue = trimSpaces(keyParts[0]);
		def.ivalue = atoi(keyParts[0].c_str());
		def.isInteger = true;
	}

	if (keyParts.size() > 1)
		def.name = getValueInQuotes(keyParts[1]);

	if (keyParts.size() > 2)
		def.sdefvalue = keyParts[2];

	def.fullDescription = "";

	if (keyParts.size() > 3)
	{
		for (size_t i = 3; i < keyParts.size(); i++)
		{
			std::string input = getValueInQuotes(keyParts[i]);
			trimSpaces(input);
			if (input.size())
				def.fullDescription += input + "\n";
		}
		if (def.fullDescription.size())
			def.fullDescription.pop_back();
	}

	outKey.choices.push_back(def);
}
