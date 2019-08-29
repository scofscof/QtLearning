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
	//�ļ��˵�
	void NewFile();//�½�
	void OpenFile();//��
	bool SaveFile();//����
	void SaveAsFile();//���Ϊ
	//�༭�˵�
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
