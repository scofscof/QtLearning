#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include "ui_QNotepad.h"
#include <QSettings>
#include <QFindDlg.h>

class QNotepad : public QMainWindow
{
	Q_OBJECT

public:
	QNotepad(QWidget *parent = Q_NULLPTR);
private slots:
	void UpdateFileStatus();
	//文件菜单
	void NewFile();//新建
	void OpenFile();//打开
	bool SaveFile();//保存
	void SaveAsFile();//另存为
	//编辑菜单
	void InsertSysDataTime();
	void Find();
	void FindNext();
private:
	void InitShortcut();
	void InitConnect();
	bool PreCheck();
private:
	Ui::QNotepadClass ui;
	QString mCurrentFilePath;
	QSettings mSettings;
	bool mChanged;
	QFindDlg mFindDlg;
	QTextCursor mCurrentCursor;
};
