#include "Fgd.h"
#include "util.h"
#include <cstdio>
#include <iostream>

const char* fgdClassTypeToString(int classType)
{
    switch(classType)
    {
        case FGD_CLASS_BASE:
            return "@BaseClass";
        case FGD_CLASS_SOLID:
            return "@SolidClass";
        default:
            return "@PointClass";
    }
}

void writefgd(std::ostream& stream, const Fgd& fgd)
{
    stream << "//generated by fgd2fgd\n\n";

    for (FgdClass* fgdClass : fgd.classes)
    {
        stream << fgdClassTypeToString(fgdClass->classType) << " ";
        
        if (!fgdClass->baseClasses.empty())
        {
            stream << "base(";
            bool first = true;
            for (auto baseClass : fgdClass->baseClasses)
            {
                if (!first)
                    stream << ", ";
                stream << baseClass;
                first = false;
            }
            stream << ") ";
        }
        
        if (fgdClass->colorSet)
        {
            stream << "color(" << (int)fgdClass->color.r << ' ' << (int)fgdClass->color.g << ' ' << (int)fgdClass->color.b << ") ";
        }
        
        if (fgdClass->sizeSet)
        {
            stream << "size(" << fgdClass->mins.toKeyvalueString(true, " ", " ", "") << ", " << fgdClass->maxs.toKeyvalueString(true, " ", " ", "") << ") ";
        }
        
        stream << "= " << fgdClass->name;

        if (!fgdClass->keyvalues.empty())
            stream << "\n";
        else
            stream << " ";
        stream << "[";
        if (!fgdClass->keyvalues.empty())
            stream << "\n";
        
        for (KeyvalueDef keyvalue : fgdClass->keyvalues)
        {
            stream << "\t" << keyvalue.name << "(" << keyvalue.valueType << ")";
            if (keyvalue.name != "spawnflags")
            {
                stream <<  " : \"" << keyvalue.shortDescription << "\"";
            }
            if (!keyvalue.defaultValue.empty())
            {
                stream << " : ";
                if (keyvalue.iType == FGD_KEY_INTEGER || keyvalue.iType == FGD_KEY_CHOICES)
                    stream << keyvalue.defaultValue;
                else
                    stream << "\"" << keyvalue.defaultValue << "\"";
            }
            if (!keyvalue.choices.empty())
            {
                stream << " =\n";
                stream << "\t[\n";
                for (KeyvalueChoice& choice : keyvalue.choices)
                {
                    stream << "\t\t" << choice.ivalue << " : \"" << choice.name << "\"\n";
                }
                stream << "\t]";
            }
            stream << "\n";
        }
        
        stream << "]\n";
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Expected an file path as argument!\n");
        return 1;
    }
    const char* fileName = argv[1];
    Fgd fgd(fileName);
    if (!fgd.parse())
    {
        fprintf(stderr, "Couldn't read %s\n", fileName);
        return 1;
    }
    writefgd(std::cout, fgd);
    return 0;
}
