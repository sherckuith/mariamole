//-----------------------------------------------------------------------------
#ifndef MM_BUILDER__H
#define MM_BUILDER__H
//-----------------------------------------------------------------------------

#include <QProcess>
#include "workspace.h" 
#include "message_handler.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

//-----------------------------------------------------------------------------

class Builder
{
public:
	Builder(void);
	~Builder(void);

	void Cancel(void);
	bool Clean(void);
	int Build(bool upload);	
	bool BurnBootLoader(void);
	int GetLastBuildStatus(void);		
	int GetPercentage(void);
	int GetBuildType(void);
	void ConfigureBootloaderBurner(QString programmerName, QString boardName, QString SerialPort);

private:
	Project * project;
	QString buildPath;
	int percentage;
	bool cancel;
	QString coreLib;
	int buildType;
	int lastBuildStatus;
	QString blbProgrammerName, blbBoardName, blbSerialPort;

	bool Compile(int fileIndex);
	bool CompileFile(QString inputFile, bool testDate, bool silent);
	bool Link(void);
	void GetBinarySize(void);
	bool BuildCoreLib(void);
	bool Upload(void);
	QString GetLeonardoSerialPort(QString defaultPort);
	QString MangleFileName(QString inputFile);
	void ImportDeclarations(void);
};

//-----------------------------------------------------------------------------

extern Builder builder;

//-----------------------------------------------------------------------------
#endif
