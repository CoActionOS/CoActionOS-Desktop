#include "CBuildGenerator.h"

#include <QFile>
#include <QTextStream>


CBuildGenerator::CBuildGenerator(QObject *parent) :
    QObject(parent)
{
}


int CBuildGenerator::generateMain(QString projectPath, QString * errorMessage, bool cpp){
    QFile tmpFile;
    QString name;
    if( cpp ){
        name = "main.cpp";
    } else {
        name = "main.c";
    }
    tmpFile.setFileName(projectPath + "/src/" + name);
    if ( tmpFile.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/src/" + name;
        return -1;
    } else {
        QTextStream ts( &tmpFile );
        if( cpp ){
            ts << "\n#include <cstdio>\n";
        } else {
            ts << "\n#include <stdio.h>\n";
        }
        ts << "\n\n\n";
        ts << "int main(int argc, char * argv[]){\n";
        ts << "\tprintf(\"Hello World\\n\");\n";
        ts << "\treturn 0;\n";
        ts << "}\n";
        tmpFile.close();
    }
    //Generate the README file
    tmpFile.setFileName(projectPath + "/README");
    if ( tmpFile.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/README";
        return -1;
    } else {
        QTextStream ts( &tmpFile );
        ts << "The Readme File";
        tmpFile.close();
    }

    //Generate the files.am file
    tmpFile.setFileName(projectPath + "/src/files.am");
    if ( tmpFile.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/src/files.am";
        return -1;
    } else {
        QTextStream ts( &tmpFile );
        qDebug("wrote to files.am");

        ts << "user_defines = \n";
        ts << "user_libs = \n";
        ts << "user_includes = \n";
        ts << "user_cpp_sources = ";
        if( cpp == true ){
            ts << "main.cpp";
        }
        ts << "\n";
        ts << "user_sources = ";
        if( cpp == false ){
            ts << "main.c";
        }
        ts << "\n";

        tmpFile.close();
    }

    return 0;
}

int CBuildGenerator::generateConfigureAc(QString projectPath,
                                         QString projectName,
                                         QString version,
                                         QString contactEmail, QString * errorMessage){
    QFile configureAc;
    configureAc.setFileName(projectPath + "/configure.ac");

    if ( configureAc.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/configure.ac";
        return -1;
    }

    QTextStream ts( &configureAc );

    ts << "AC_INIT([" << projectName << "], [" << version << "], [" << contactEmail << "])\n";
    ts << "AC_SUBST(PROJECT,[" << projectName << "])\n";
    ts << "AC_CONFIG_AUX_DIR([build-aux])\n";
    ts << "AM_INIT_AUTOMAKE([-Wall -Werror foreign -Woverride])\n";
    ts << "AC_CANONICAL_BUILD\n";
    ts << "AC_CANONICAL_HOST\n";
    ts << "AC_NO_EXECUTABLES\n";
    ts << "AC_PROG_CXX\n";
    ts << "AC_PROG_CC\n";
    ts << "AM_PROG_CC_C_O\n";
    ts << "AC_CHECK_TOOL(AS, as, as)\n";
    ts << "AM_PROG_AS\n";
    ts << "AC_PROG_RANLIB\n";
    ts << "AC_CHECK_TOOL(AR, ar, ar)\n";
    ts << "AC_CHECK_TOOL(OBJCOPY, objcopy, objcopy)\n";
    ts << "AC_CHECK_TOOL(SIZE, size, size)\n";
    ts << "AC_CHECK_TOOL(OBJDUMP, objdump, objdump)\n";
    ts << "AC_SUBST(ARCH, ${ARCH})\n";
    ts << "AC_SUBST(DEVICE, ${DEVICE})\n";
    ts << "AC_SUBST(PGM_START, ${PGM_START})\n";
    ts << "AC_SUBST(RAM_START, ${RAM_START})\n";
    ts << "AC_SUBST(RAM_SIZE, ${RAM_SIZE})\n";
    ts << "AC_SUBST(TYPE, ${TYPE})\n";
    ts << "AC_CONFIG_HEADERS([config.h])\n";
    ts << "AC_CONFIG_FILES([Makefile src/Makefile])\n";
    ts << "AC_OUTPUT\n";

    configureAc.close();

    return 0;

}

int CBuildGenerator::generateMakefileAm(QString projectPath,
                                        QString projectName,
                                        BuildFlags * flags,
                                        QString sources,
                                        QString * errorMessage){

    //Generate projectPath/Makefile and projectPath/src/Makefile
    QFile makefileAmFile;
    makefileAmFile.setFileName(projectPath + "/Makefile.am");


    if ( makefileAmFile.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/Makefile.am";
        return -1;
    } else {

        QTextStream ts( &makefileAmFile );

        ts << "SUBDIRS = src \n";
        ts << "dist_noinst_DATA = README settings.INI\n\n";

        ts << "size : all\n";
        ts << "\t$(SIZE) $(top_builddir)/src/fw.elf > $(top_builddir)/$(PROJECT)-size.txt\n";
        ts << "\t$(SIZE) $(top_builddir)/src/fw.elf\n\n";

        ts << "asm : size\n";
        ts << "\t$(OBJDUMP) -j .text -j .data -j .bss -d $(top_builddir)/src/fw.elf -j .text -j .data -j .bss > $(top_builddir)/$(PROJECT).s\n\n";

        ts << "bin : asm\n";
        ts << "\t$(OBJCOPY) -j .text -j .data -O binary $(top_builddir)/src/fw.elf $(top_builddir)/$(PROJECT)\n";


        makefileAmFile.close();
    }

    makefileAmFile.setFileName(projectPath + "/src/Makefile.am");
    if ( makefileAmFile.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/src/Makefile.am";
        return -1;
    } else {
        QTextStream ts( &makefileAmFile );
        ts << "include $(srcdir)/files.am\n";


        ts << "AM_CFLAGS= " << flags->cflags << "\n";
        ts << "AM_CXXFLAGS= " << flags->cxxflags << "\n";

        ts << "bin_PROGRAMS = fw.elf\n";
        ts << "fw_elf_CFLAGS= $(CFLAGS) $(AM_CFLAGS)\n";
        ts << "fw_elf_CXXFLAGS= $(CXXFLAGS) $(AM_CXXFLAGS)\n";
        ts << "fw_elf_LDFLAGS= " << flags->ldflags << "\n";
        ts << "fw_elf_SOURCES = " << sources << "\n";
        ts << "fw_elf_LDADD = " << flags->ldadd << "\n";
        makefileAmFile.close();
    }

    //To avoid a compiler warning:
    projectName.clear();

    return 0;
}

int CBuildGenerator::generateMakefile(QString projectPath,
                                      QString projectName,
                                      BuildFlags * flags,
                                      QString pgmStart,
                                      QString dataStart,
                                      QString dataSize,
                                      QString prefix,
                                      QString arch,
                                      QString * errorMessage){

    //Generate projectPath/Makefile and projectPath/src/Makefile
    QFile makefileFile;
    //QFile filesAm;
    makefileFile.setFileName(projectPath + "/build/Makefile");


    if ( makefileFile.open(QIODevice::WriteOnly) == false ){
        *errorMessage = "Could not create file " + projectPath + "/build/Makefile";
        return -1;
    } else {

        QTextStream ts( &makefileFile );


        ts << "include ../src/files.am\n";

        ts << "top_builddir=.\n";
        ts << "PROJECT="<< projectName << "\n";
        ts << "PGM_START=" << pgmStart << "\n";
        ts << "RAM_START=" << dataStart << "\n";
        ts << "RAM_SIZE=" << dataSize << "\n";
        ts << "ARCH=" << arch << "\n";
        ts << "PREFIX=" << prefix << "\n";
        ts << "SIZE=$(PREFIX)-size\n";
        ts << "OBJDUMP=$(PREFIX)-objdump\n";
        ts << "OBJCOPY=$(PREFIX)-objcopy\n";
        ts << "CC=$(PREFIX)-gcc\n";
        ts << "CXX=$(PREFIX)-g++\n";
        ts << "C_SOURCES=$(patsubst %.c, ../src/%.c, $(user_sources))\n";
        ts << "CPP_SOURCES=$(patsubst %.cpp, ../src/%.cpp, $(user_cpp_sources))\n";
        ts << "EXECUTABLE=fw.elf\n";
        ts << "OBJECTS=$(patsubst ../src/%.cpp, %.o, $(CPP_SOURCES))\n";
        ts << "OBJECTS+= $(patsubst ../src/%.c, %.o, $(C_SOURCES))\n";

        ts << "CFLAGS=" << flags->cflags << "\n";
        ts << "CXXFLAGS=" << flags->cxxflags << "\n";
        ts << "LDFLAGS=" << flags->ldflags << " " << flags->ldadd << "\n";

        ts << "all: $(C_SOURCES) $(CPP_SOURCES) $(EXECUTABLE)\n\n";

        ts << "$(EXECUTABLE): $(OBJECTS)\n";
        ts << "\t$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@  $(LDFLAGS)\n\n";

        ts << "%.o: ../src/%.cpp\n";
        ts << "\t$(CXX) $(CXXFLAGS) -c $< -o $@\n\n";

        ts << "%.o: ../src/%.c\n";
        ts << "\t$(CC) $(CFLAGS) -c $< -o $@\n\n";

        ts << "all: fw.elf\n\n";
        ts << "size : all\n";
        ts << "\t$(SIZE) $(top_builddir)/fw.elf > $(top_builddir)/$(PROJECT)-size.txt\n";
        ts << "\t$(SIZE) $(top_builddir)/fw.elf\n\n";
        ts << "asm : size\n";
        ts << "\t$(OBJDUMP) -j .text -j .data -j .bss -d $(top_builddir)/fw.elf -j .text -j .data -j .bss > $(top_builddir)/$(PROJECT).s\n\n";
        ts << "bin : asm\n";
        ts << "\t$(OBJCOPY) -j .text -j .data -O binary $(top_builddir)/fw.elf $(top_builddir)/$(PROJECT)\n\n";
        ts << "clean:\n";
        ts << "\trm -f $(top_builddir)/$(PROJECT) $(top_builddir)/$(PROJECT).s *.o\n\n";

        makefileFile.close();
    }

    return 0;
}
